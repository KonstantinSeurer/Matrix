
include math.Base;

function vec3 sampleHemisphereEven(vec2 xi) {
	const float r = sqrt(1.0 - xi.x * xi.x);
	const float phi = TWO_PI * xi.y;
	return vec3(cos(phi) * r, sin(phi) * r, xi.x);
}

function vec3 sampleHemisphereCos(vec2 xi) {
	const float theta = acos(xi.x);
	const float phi = TWO_PI * xi.y;
	const float sinTheta = sin(theta);
	return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cos(theta));
}

function vec3 sampleHemisphereGGX(vec2 xi, float a) {
	const float phi = TWO_PI * xi.x;
	const float theta = acos(sqrt((1.0 - xi.y) / ((a * a - 1.0) * xi.y + 1.0)));
	const float sinTheta = sin(theta);
	return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cos(theta));
}

function vec3 sampleAngleEven(vec2 xi, float angle) {
	const float theta = acos(1.0 - xi.x + xi.x * cos(angle));
	const float phi = TWO_PI * xi.y;
	const float sinTheta = sin(theta);
	return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cos(theta));
}
