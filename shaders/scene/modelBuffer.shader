
struct ModelType
{
	int floatAttributeCount;
	int intAttributeCount;
}

struct VertexReference {
	int type;
	int floatAttributeOffset;
	int intAttributeOffset;
}

set modelBuffer
{
	storageBuffer STANDARD
	{
		ModelType[1000] types;
	} typeTable;

	storageBuffer STANDARD
	{
		float[] data;
	}[1000] floats;
	
	storageBuffer STANDARD
	{
		int[] data;
	}[1000] ints;
	
	storageBuffer STANDARD
	{
		int[] data;	
	}[1000] indices;
}

function VertexReference getReference(int type, int index)
{
	const int vertexIndex = modelBuffer.indices[type].data[index];
	const ModelType modelType = modelBuffer.typeTable.types[type];
	return VertexReference(type, modelType.floatAttributeCount * vertexIndex, modelType.intAttributeCount * vertexIndex);
}

function float getFloat(VertexReference vertex, int index) {
	return modelBuffer.floats[vertex.type].data[vertex.floatAttributeOffset + index];
}

function int getInt(VertexReference vertex, int index) {
	return modelBuffer.ints[vertex.type].data[vertex.intAttributeOffset + index];
}
