
set noise
{
	texture2D[64] blue64;
}

struct NoiseSeed2D
{
	uint x;
	uint y;
	uint index;
}

function float blueNoiseFloat(input output NoiseSeed2D seed)
{
	const float result = texelFetch(noise.blue64[seed.index % 64], ivec2(seed.x % 256, seed.y % 256), 0).x;
	seed.index++;
	return result;
}

function vec2 blueNoiseVec2(input output NoiseSeed2D seed)
{
	return vec2(blueNoiseFloat(seed), blueNoiseFloat(seed));
}

function vec3 blueNoiseVec3(input output NoiseSeed2D seed)
{
	return vec3(blueNoiseFloat(seed), blueNoiseFloat(seed), blueNoiseFloat(seed));
}

function vec4 blueNoiseVec4(input output NoiseSeed2D seed)
{
	return vec4(blueNoiseFloat(seed), blueNoiseFloat(seed), blueNoiseFloat(seed), blueNoiseFloat(seed));
}
