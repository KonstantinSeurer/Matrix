
struct Ray
{
	vec3 origin;
	vec3 direction;
}

struct Sphere
{
	vec3 center;
	float radius;
}

function vec2 intersectSphere(Ray ray, Sphere sphere) {
	vec3 v = ray.origin - sphere.center;
	float B = 2.0 * dot(ray.direction, v);
	float C = dot(v, v) - sphere.radius * sphere.radius;
	float B2 = B * B;
	float f = B2 - 4.0 * C;
	
	if (f < 0.0) {
		return vec2(-1.0);
	}
	
	float sqrtF = sqrt(f);
	return 0.5 * (-B + vec2(-sqrtF, sqrtF));
}

struct AABB
{
	vec3 min;
	vec3 max;
}

function vec2 intersectAABB(Ray ray, AABB aabb) {
	const vec3 tMin = (aabb.min - ray.origin) / ray.direction;
	const vec3 tMax = (aabb.max - ray.origin) / ray.direction;
	const vec3 t1 = min(tMin, tMax);
	const vec3 t2 = max(tMin, tMax);
	const float tNear = max(max(t1.x, t1.y), t1.z);
	const float tFar = min(min(t2.x, t2.y), t2.z);
	return vec2(tNear, tFar);
}
