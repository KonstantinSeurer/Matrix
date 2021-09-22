
function uint hash(input output uint seed)
{
	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 5;
	return seed * 2133739036;
}

function float randomFloat(input output uint seed)
{
	return uintBitsToFloat(0x3F800000 | (hash(seed) >> 9)) - 1.0;
}

function float randomFloat(input output uint seed, float minimum, float maximum)
{
	return randomFloat(seed) * (maximum - minimum) + minimum;
}

function vec2 randomVec2(input output uint seed)
{
	return vec2(randomFloat(seed), randomFloat(seed));
}

function vec2 randomVec2(input output uint seed, vec2 minimum, vec2 maximum)
{
	return randomVec2(seed) * (maximum - minimum) + minimum;
}

function vec3 randomVec3(input output uint seed)
{
	return vec3(randomFloat(seed), randomFloat(seed), randomFloat(seed));
}

function vec3 randomVec3(input output uint seed, vec3 minimum, vec3 maximum)
{
	return randomVec3(seed) * (maximum - minimum) + minimum;
}

function vec4 randomVec4(input output uint seed)
{
	return vec4(randomFloat(seed), randomFloat(seed), randomFloat(seed), randomFloat(seed));
}

function vec4 randomVec4(input output uint seed, vec4 minimum, vec4 maximum)
{
	return randomVec4(seed) * (maximum - minimum) + minimum;
}
