
struct DirectionalLight
{
	vec3 color;
	vec3 direction;
}

set lightBuffer
{
	storageBuffer STANDARD
	{
		DirectionalLight[10] directionalLights;
		uint directionalLightCount;
	} lights;
}
