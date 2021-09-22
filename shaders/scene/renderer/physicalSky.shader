
include math.Base;
include math.Random;
include math.Intersection;
include scene.LightBuffer;

localSize.x = 8;
localSize.y = 8;
localSize.z = 1;

set data
{
	image2D
	{
		componentCount = 4;
		componentSize = 4;
		floatingPoint = true;
	} result;
	
	image2D
	{
		componentCount = 4;
		componentSize = 1;
		floatingPoint = false;
	} transmittance;
}

const float atmosphereInnerRadius = 6371000.0;
const float atmosphereOuterRadius = atmosphereInnerRadius + 100000.0;
const Sphere atmosphereSphere = Sphere(vec3(0.0, -atmosphereInnerRadius, 0.0), atmosphereOuterRadius);
const Sphere planetSphere = Sphere(vec3(0.0, -atmosphereInnerRadius, 0.0), atmosphereInnerRadius * 0.999);

function vec2 sampleDensity(vec3 P)
{
	float height = length(P - atmosphereSphere.center) - atmosphereInnerRadius;
	return exp(-height / vec2(8000.0, 1200.0));
}

const int OPTICAL_DEPTH_SAMPLE_COUNT = 10;
const vec4 SCATTER_COEFFICIENTS = vec4(5.5e-6, 13.0e-6, 22.4e-6, 21e-6);

function vec2 calculateOpticalDepth(Ray ray, float tMax)
{
	if (intersectSphere(ray, planetSphere).y > 0.0)
	{
		return vec2(1.0e10);
	}

	const float sectionSize = tMax / float(OPTICAL_DEPTH_SAMPLE_COUNT);
	vec2 result = vec2(0.0);
	for (int sampleIndex = 0; sampleIndex < OPTICAL_DEPTH_SAMPLE_COUNT; sampleIndex++)
	{
		const vec3 samplePosition = ray.origin + ray.direction * sectionSize * float(sampleIndex);
		result += sampleDensity(samplePosition);
	}
	return result * tMax / float(OPTICAL_DEPTH_SAMPLE_COUNT);
}

function vec3 calculateTransmittance(vec2 opticalDepth)
{
	return exp(-SCATTER_COEFFICIENTS.xyz * opticalDepth.x - SCATTER_COEFFICIENTS.www * opticalDepth.y);
}

function DirectionalLight sampleLight(input output uint seed)
{
	return lightBuffer.lights.directionalLights[int(randomFloat(seed) * float(lightBuffer.lights.directionalLightCount))];
}

const float RAYLEIGH_CONSTANT = 3.0 / (16.0 * PI);
const float MIE_CONSTANT = 3.0 / (8.0 * PI);

function float rayleighPhase(float cosTheta)
{
	return RAYLEIGH_CONSTANT * (1.0 + cosTheta * cosTheta);
}

function float miePhase(float cosTheta, float g)
{
	const float g2 = g * g;
	return MIE_CONSTANT * (1.0 - g2) * (cosTheta * cosTheta + 1.0) / (pow(1.0 + g2 - 2.0 * cosTheta * g, 1.5) * (2.0 + g2));
}

const int SAMPLE_COUNT = 100;

main
{
	const ivec2 size = ivec2(imageSize(data.result).xy);
	
	if (globalCoords.x >= size.x || globalCoords.y >= size.y)
	{
		return;
	}
	
	const vec3 D = sphereCoordsToDirection(vec2(globalCoords) / vec2(size));
	
	const Ray ray = Ray(vec3(0.0), D);
	
	if (intersectSphere(ray, planetSphere).y > 0.0)
	{
		imageStore(data.result, ivec2(globalCoords.xy), vec4(0.0, 0.0, 0.0, 1.0));
		imageStore(data.transmittance, ivec2(globalCoords.xy), vec4(0.0, 0.0, 0.0, 1.0));
		return;
	}
	
	uint seed = globalCoords.x + globalCoords.y * size.x;
	seed = hash(seed);
	
	const vec2 intersection = max(intersectSphere(ray, atmosphereSphere), vec2(0.0));

	const float sectionSize = (intersection.y - intersection.x) / float(SAMPLE_COUNT);
	vec2 opticalDepth = vec2(0.0);
	vec3 rayleigh = vec3(0.0);
	vec3 mie = vec3(0.0);
	for (int sampleIndex = 0; sampleIndex < SAMPLE_COUNT; sampleIndex++)
	{
		const vec3 samplePosition = ray.origin + ray.direction * sectionSize * float(sampleIndex);
		const vec2 density = sampleDensity(samplePosition);
		const vec2 currentOpticalDepth = density * sectionSize;
		opticalDepth += currentOpticalDepth;
		
		const DirectionalLight light = sampleLight(seed);
		const Ray lightRay = Ray(samplePosition, -light.direction);
		const vec2 intersection = intersectSphere(lightRay, atmosphereSphere);
		const vec2 lightOpticalDepth = calculateOpticalDepth(lightRay, intersection.y - max(intersection.x, 0.0));
		
		const float cosTheta = -dot(ray.direction, light.direction);
		
		const vec3 transmittance = calculateTransmittance(opticalDepth + lightOpticalDepth);
		
		const float planetReflectionFactor = max(-light.direction.y, 0.0) * 0.5;
		
		rayleigh += light.color * transmittance * (rayleighPhase(cosTheta) + planetReflectionFactor) * currentOpticalDepth.x;
		mie += light.color * transmittance * miePhase(cosTheta, 0.758) * currentOpticalDepth.y;
	}

	imageStore(data.result, ivec2(globalCoords.xy), vec4(rayleigh * SCATTER_COEFFICIENTS.xyz + mie * SCATTER_COEFFICIENTS.www, 1.0));
	imageStore(data.transmittance, ivec2(globalCoords.xy), vec4(calculateTransmittance(opticalDepth), 1.0));
}
