
include scene.ModelBuffer;

function vec3 getPosition(VertexReference vertex)
{
	return vec3(getFloat(vertex, 0), getFloat(vertex, 1), getFloat(vertex, 2));
}

function vec2 getTextureCoords(VertexReference vertex)
{
	return vec2(getFloat(vertex, 3), getFloat(vertex, 4));
}

function vec3 getNormal(VertexReference vertex)
{
	return vec3(getFloat(vertex, 5), getFloat(vertex, 6), getFloat(vertex, 7));
}

function vec4 getTangent(VertexReference vertex)
{
	return vec4(getFloat(vertex, 8), getFloat(vertex, 9), getFloat(vertex, 10), getFloat(vertex, 11));
}
