
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
		
		outputs.tc = inputs.tc;
		outputs.normal = inputs.normal;
		outputs.tangent = inputs.tangent.xyz;
		outputs.bitangent = normalize(cross(inputs.normal, inputs.tangent.xyz)) * inputs.tangent.w;
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
		vec4 color;
	}
	
	main
	{
		const MaterialReference materialRef = getReference(uniforms.material);
		
		const vec4 baseColor = sampleBaseColor(materialRef, inputs.tc);
		if (baseColor.a < 0.5)
		{
			discard;
		}
		
		outputs.color = baseColor;
	}
}
