
include scene.MaterialBuffer;

function vec4 getBaseColorFactor(MaterialReference material)
{
	return vec4(getFloat(material, 0), getFloat(material, 1), getFloat(material, 2), getFloat(material, 3));
}

function float getMetallicFactor(MaterialReference material)
{
	return getFloat(material, 4);
}

function float getRoughnessFactor(MaterialReference material)
{
	return getFloat(material, 5);
}

function vec3 getEmissiveFactor(MaterialReference material)
{
	return vec3(getFloat(material, 6), getFloat(material, 7), getFloat(material, 8));
}

function vec4 sampleBaseColorTexture(MaterialReference material, vec2 tc)
{
	return sampleTexture2D(material, 0, tc);
}

function vec2 sampleMetallicRoughnessTexture(MaterialReference material, vec2 tc)
{
	return sampleTexture2D(material, 1, tc).xy;
}

function vec3 sampleNormalTexture(MaterialReference material, vec2 tc)
{
	return sampleTexture2D(material, 2, tc).xyz;
}

function float sampleOcclusionTexture(MaterialReference material, vec2 tc)
{
	return sampleTexture2D(material, 3, tc).x;
}

function vec3 sampleEmissiveTexture(MaterialReference material, vec2 tc)
{
	return sampleTexture2D(material, 4, tc).rgb;
}

function vec4 sampleBaseColor(MaterialReference material, vec2 tc)
{
	return sampleBaseColorTexture(material, tc) * getBaseColorFactor(material);
}

function vec2 sampleMetallicRoughness(MaterialReference material, vec2 tc)
{
	return sampleMetallicRoughnessTexture(material, tc) * vec2(getMetallicFactor(material), getRoughnessFactor(material));
}

function vec3 sampleNormal(MaterialReference material, vec2 tc)
{
	return sampleNormalTexture(material, tc) * 2.0 - 1.0;
}

function float sampleOcclusion(MaterialReference material, vec2 tc)
{
	return sampleOcclusionTexture(material, tc);
}

function vec3 sampleEmissive(MaterialReference material, vec2 tc)
{
	return sampleEmissiveTexture(material, tc);
}

