
include scene.MaterialBuffer.GLTF;
include scene.CameraBuffer;

uniforms
{
	MaterialIndex material;
	mat4 transform;
	int camera;
}

vertexShader
{
	inputs
	{
		vec3 pos;
		vec2 tc;
		vec3 normal;
		vec4 tangent;
	}
	
	outputs
	{
		vec2 tc;
		vec3 normal;
		vec3 tangent;
		vec3 bitangent;
	}
	
	main
	{
		CameraTransform camera = getCamera(uniforms.camera);
		vertexPosition = (camera.viewProjection * uniforms.transform * vec4(inputs.pos, 1.0)) * vec4(1.0, -1.0, 1.0, 1.0);
		
		mat3 normalMatrix = mat3(uniforms.transform);
		
		outputs.tc = inputs.tc;
		outputs.normal = normalMatrix * inputs.normal;
		outputs.tangent = normalMatrix * inputs.tangent.xyz;
		outputs.bitangent = normalMatrix * (cross(inputs.normal, inputs.tangent.xyz) * inputs.tangent.w);
	}
}

fragmentShader
{
	inputs
	{
		vec2 tc;
		vec3 normal;
		vec3 tangent;
		vec3 bitangent;
	}
	
	outputs
	{
		vec4 albedoMetallic;
		vec4 normalRoughness;
		vec4 emissiveOcclusion;
	}
	
	main
	{
		const MaterialReference materialRef = getReference(uniforms.material);
		
		const vec4 baseColor = sampleBaseColor(materialRef, inputs.tc);
		if (baseColor.a < 0.5)
		{
			discard; // doubles the fragment shader invocation count
		}
		
		const vec2 metallicRoughness = sampleMetallicRoughness(materialRef, inputs.tc);
		
		outputs.albedoMetallic.rgb = baseColor.rgb;
		outputs.albedoMetallic.a = metallicRoughness.x;
		
		const vec3 normalSample = sampleNormal(materialRef, inputs.tc);
		outputs.normalRoughness.rgb = normalize(normalize(inputs.tangent) * normalSample.x + normalize(inputs.bitangent) * normalSample.y + normalize(inputs.normal) * normalSample.z) * 0.5 + 0.5;
		outputs.normalRoughness.w = metallicRoughness.y;
		
		outputs.emissiveOcclusion.rgb = sampleEmissive(materialRef, inputs.tc);
		outputs.emissiveOcclusion.a = sampleOcclusion(materialRef, inputs.tc);
	}
}
