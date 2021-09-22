
include math.Random;

function float dither8(float value, input output uint seed)
{
	const float value255 = value * 255.0;
	if (fract(value255) < randomFloat(seed)) {
		return max(floor(value255) / 255.0, 0.0);
	}
	return min(floor(value255 + 1.0) / 255.0, 1.0);
}

function vec2 dither8(vec2 value, input output uint seed)
{
	return vec2(dither8(value.x, seed), dither8(value.y, seed));
}

function vec3 dither8(vec3 value, input output uint seed)
{
	return vec3(dither8(value.x, seed), dither8(value.y, seed), dither8(value.z, seed));
}

function vec4 dither8(vec4 value, input output uint seed)
{
	return vec4(dither8(value.x, seed), dither8(value.y, seed), dither8(value.z, seed), dither8(value.w, seed));
}
