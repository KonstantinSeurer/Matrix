
function vec3 orthogonal(vec3 v) {
	// http://lolengine.net/blog/2013/09/21/picking-orthogonal-vector-combing-coconuts
	return mix(vec3(-v.y, v.x, 0.0), vec3(0.0, -v.z, v.y), step(abs(v.x), abs(v.z)));
}

function vec3 aroundZ(vec3 a, vec3 z)
{
	const vec3 tangent = orthogonal(z);
	const vec3 bitangent = cross(tangent, z);
	return tangent * a.x + bitangent * a.y + z * a.z;
}

const float HALF_PI = 1.5707963268;
const float INV_HALF_PI = 0.636619772368;

const float PI = 3.14159265359;
const float INV_PI = 0.318309886184;

const float TWO_PI = 6.28318530718;
const float INV_TWO_PI = 0.159154943092;

function float mix(float a, float b, float c, vec2 barycentrics)
{
	return (1.0 - barycentrics.x - barycentrics.y) * a + barycentrics.x * b + barycentrics.y * c;
}

function vec2 mix(vec2 a, vec2 b, vec2 c, vec2 barycentrics)
{
	return (1.0 - barycentrics.x - barycentrics.y) * a + barycentrics.x * b + barycentrics.y * c;
}

function vec3 mix(vec3 a, vec3 b, vec3 c, vec2 barycentrics)
{
	return (1.0 - barycentrics.x - barycentrics.y) * a + barycentrics.x * b + barycentrics.y * c;
}

function vec4 mix(vec4 a, vec4 b, vec4 c, vec2 barycentrics)
{
	return (1.0 - barycentrics.x - barycentrics.y) * a + barycentrics.x * b + barycentrics.y * c;
}

function vec2 directionToSphereCoords(vec3 D)
{
	return vec2(atan(D.z, D.x), asin(D.y)) * vec2(INV_TWO_PI, INV_PI) + 0.5;
}

function vec3 sphereCoordsToDirection(vec2 C)
{
	const vec2 radiensC = (C - 0.5) * vec2(TWO_PI, PI);
	const float cosY = cos(radiensC.y);
	return vec3(cos(radiensC.x) * cosY, sin(radiensC.y), sin(radiensC.x) * cosY);
}

