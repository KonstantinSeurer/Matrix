
include scene.MaterialBuffer.GLTF;
include scene.CameraBuffer;
include scene.AccelerationStructure;
include scene.LightBuffer;
include scene.ModelBuffer.GLTF;
include math.Random;
include math.ImportanceSampler;
include math.Base;
include math.Tonemapper;
include math.Dither;
include math.Noise;
include math.Intersection;

localSize.x = 8;
localSize.y = 8;
localSize.z = 1;

uniforms
{
	int camera;
	uint seed;
}

set data
{
	image2D
	{
		componentCount = 4;
		componentSize = 1;
		floatingPoint = false;
	} result;
	
	texture2D albedoMetallic;
	texture2D normalRoughness;
	texture2D emissiveOcclusion;
	texture2D depth;
	
	texture2D environment;
	texture2D transmittance;

	texture3D volume;
}

struct Intersection
{
	float t;
	int modelIndex;
	int primitiveIndex;
	vec2 barycentrics;
	mat4 transform;
}

function Intersection intersectScene(vec3 P, vec3 D)
{
	rayQueryEXT rayQuery;
	rayQueryInitializeEXT(rayQuery, sceneAccelerationStructure.accelerationStructure, gl_RayFlagsOpaqueEXT | gl_RayFlagsCullBackFacingTrianglesEXT, 0xFF, P, 0.01, D, 100.0);
	
	while (rayQueryProceedEXT(rayQuery)) { }
	
	if (rayQueryGetIntersectionTypeEXT(rayQuery, true) == gl_RayQueryCommittedIntersectionTriangleEXT)
		return Intersection(rayQueryGetIntersectionTEXT(rayQuery, true), rayQueryGetIntersectionInstanceCustomIndexEXT(rayQuery, true),
							rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true), rayQueryGetIntersectionBarycentricsEXT(rayQuery, true),
							mat4(rayQueryGetIntersectionObjectToWorldEXT(rayQuery, true)));
	return Intersection(-1.0, 0, 0, vec2(0.0), mat4(0.0));
}

function bool intersectsScene(vec3 P, vec3 D)
{
	rayQueryEXT rayQuery;
	rayQueryInitializeEXT(rayQuery, sceneAccelerationStructure.accelerationStructure, gl_RayFlagsTerminateOnFirstHitEXT, 0xFF, P, 0.01, D, 1000.0);
	
	while (rayQueryProceedEXT(rayQuery)) { }
	
	return rayQueryGetIntersectionTypeEXT(rayQuery, true) == gl_RayQueryCommittedIntersectionTriangleEXT;
}

const float SUN_ANGLE = 0.01;

function vec3 sampleSkyTransmittance(vec3 D)
{
	return texture(data.transmittance, directionToSphereCoords(D)).rgb;
}

function vec3 calculateSunIrradience(vec3 P, vec3 N, input output uint seed)
{
	const DirectionalLight light = lightBuffer.lights.directionalLights[int(randomFloat(seed) * float(lightBuffer.lights.directionalLightCount))];
	
	const float NdotL = -dot(light.direction, N);
	if (NdotL < 0.0)
	{
		return vec3(0.0);
	}
	
	const vec3 D = aroundZ(sampleAngleEven(randomVec2(seed), SUN_ANGLE), -light.direction);
	return intersectsScene(P, D) ? vec3(0.0) : light.color * NdotL * sampleSkyTransmittance(-light.direction) / PI;
}

function vec3 sampleSky(vec3 D)
{
	return texture(data.environment, directionToSphereCoords(D)).rgb;
}

function float sampleVolumeDensity(vec3 P)
{
	return texture(data.volume, P * 0.5 + 0.5).r;
}

function float sampleVolumeDensity(Ray ray, float tMax, input output uint seed)
{
	float totalDensity = 0.0;
	const int sampleCount = 5;
	const float segmentSize = tMax / float(sampleCount);
	for (int sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++)
	{
		totalDensity += sampleVolumeDensity(ray.origin + ray.direction * segmentSize * (float(sampleIndex) + randomFloat(seed))) * segmentSize;
	}
	return totalDensity;
}

main
{
	const ivec2 size = ivec2(imageSize(data.result).xy);
	
	if (globalCoords.x >= size.x || globalCoords.y >= size.y)
	{
		return;
	}
	
	const float depth = texelFetch(data.depth, ivec2(globalCoords.xy), 0).r;
	
	const vec2 normalizedCoords = vec2(float(globalCoords.x) / float(size.x) * 2.0 - 1.0, 1.0 - float(globalCoords.y) / float(size.y) * 2.0);
	const CameraTransform camera = cameraBuffer.cameras.transforms[uniforms.camera];
	const vec3 cameraPosition = camera.invView[3].xyz;
	
	const vec3 albedo = texelFetch(data.albedoMetallic, ivec2(globalCoords.xy), 0).rgb;
	const vec3 normal = texelFetch(data.normalRoughness, ivec2(globalCoords.xy), 0).rgb * 2.0 - 1.0;
	
	const vec4 P4 = camera.invViewProjection * vec4(normalizedCoords, depth, 1.0);
	const vec3 P = P4.xyz / P4.w + normal * 0.01;
	
	const vec3 V = normalize(cameraPosition - P);
	
	uint seed = (globalCoords.x + globalCoords.y * size.x) * 12345;
	seed = hash(seed);
	// seed *= uint(uniforms.time * 39548.48);
	seed = hash(seed);
	
	if (depth < 0.999)
	{
		vec3 indirectIrradience = vec3(0.0);
		
		const int sampleCount = 16;
		
		NoiseSeed2D noiseSeed = NoiseSeed2D(globalCoords.x, globalCoords.y, uniforms.seed * sampleCount * 2);
		
		for (int sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++)
		{
			const vec3 D = aroundZ(sampleHemisphereCos(blueNoiseVec2(noiseSeed)), normal);
			
			const Intersection intersection = intersectScene(P, D);
			if (intersection.t > 0.0)
			{
				const vec3 hitP = P + D * intersection.t;
				const VertexReference v0 = getReference(intersection.modelIndex, intersection.primitiveIndex * 3);
				const vec3 normal = normalize(getNormal(v0) * mat3(intersection.transform));
				indirectIrradience += calculateSunIrradience(hitP, normal, seed);
			}
			else
			{
				indirectIrradience += sampleSky(D);
			}
		}
		
		vec3 directIrradience = vec3(0.0);
		for (int sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++)
		{
			directIrradience += calculateSunIrradience(P, normal, seed);
		}
		
		const vec3 result = albedo * ((indirectIrradience + directIrradience) / float(sampleCount) + 0.02);
		imageStore(data.result, ivec2(globalCoords.xy), vec4(tonemapACES(result), 1.0));
	}
	else
	{
		vec3 result = sampleSky(-V);
		for (int lightIndex = 0; lightIndex < lightBuffer.lights.directionalLightCount; lightIndex++)
		{
			const DirectionalLight light = lightBuffer.lights.directionalLights[lightIndex];
			const float angle = acos(dot(V, light.direction));
			if (angle < SUN_ANGLE)
			{
				result += light.color * sampleSkyTransmittance(-V);
			}
		}
		
		const Ray ray = Ray(cameraPosition, -V);
		const AABB volume = AABB(vec3(-1.0), vec3(1.0));
		const vec2 volumeIntersection = intersectAABB(ray, volume);
		if (volumeIntersection.y < 0.0 || volumeIntersection.x > volumeIntersection.y)
		{
			imageStore(data.result, ivec2(globalCoords.xy), vec4(dither8(tonemapACES(result), seed), 1.0));
			return;
		}
		const float rayTravelDistance = volumeIntersection.y - max(volumeIntersection.x, 0.0);
		
		const int sampleCount = 5;
		NoiseSeed2D noiseSeed = NoiseSeed2D(globalCoords.x, globalCoords.y, uniforms.seed * sampleCount * 4);
		
		const float segmentSize = rayTravelDistance / float(sampleCount);
		float totalDensity = 0.0;
		const vec3 nearIntersection = ray.origin + ray.direction * max(volumeIntersection.x, 0.0);
		vec3 inscattering = vec3(0.0);
		const vec3 scatterCoefficients = vec3(0.5, 0.5, 0.5);
		const float absorbtion = 1.0;
		for (int sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++)
		{
			const vec3 samplePosition = nearIntersection + ray.direction * segmentSize * (float(sampleIndex) + blueNoiseFloat(noiseSeed));
			const float currentDensity = sampleVolumeDensity(samplePosition) * segmentSize;
			totalDensity += currentDensity;
			
			const DirectionalLight light = lightBuffer.lights.directionalLights[int(randomFloat(seed) * float(lightBuffer.lights.directionalLightCount))];
			const Ray sampleRay = Ray(samplePosition, -light.direction);
			const vec2 sampleIntersection = intersectAABB(sampleRay, volume);
			const float sunDensity = sampleVolumeDensity(sampleRay, sampleIntersection.y, seed);
			const vec3 sunInscattering = exp(-sunDensity * (scatterCoefficients + absorbtion)) * light.color;
			
			const Ray skySampleRay = Ray(samplePosition, normalize(blueNoiseVec3(noiseSeed) * 2.0 - 1.0));
			const float skySampleIntersection = intersectAABB(skySampleRay, volume).y;
			const vec3 skyInscattering = sampleSky(skySampleRay.direction) * exp(-sampleVolumeDensity(skySampleRay, skySampleIntersection, seed) * (scatterCoefficients + absorbtion));
			
			inscattering += exp(-totalDensity * (scatterCoefficients + absorbtion)) * currentDensity * (sunInscattering + skyInscattering);
		}
		
		result = result * exp(-totalDensity * absorbtion) + inscattering * scatterCoefficients;
		
		imageStore(data.result, ivec2(globalCoords.xy), vec4(dither8(tonemapACES(result), seed), 1.0));
	}
}
