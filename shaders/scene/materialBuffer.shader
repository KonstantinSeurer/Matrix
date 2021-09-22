
struct MaterialType {
	int floatAttributesOffset;
	int floatAttributeCount;
	
	int intAttributesOffset;
	int intAttributeCount;
	
	int imageSamplers2DOffset;
	int imageSampler2DCount;
}

struct MaterialIndex {
	int type;
	int index;
}

struct MaterialReference {
	int floatAttributesOffset;
	int intAttributesOffset;
	int imageSamplers2DOffset;
}

set materialBuffer {
	storageBuffer STANDARD {
		MaterialType[1000] types;
	} typeTable;
	
	storageBuffer STANDARD {
		float[10000] floats;
		int[10000] ints;
	} attributes;
	
	texture2D[10000] imageSamplers2D;
}

function MaterialReference getReference(MaterialIndex index) {
	MaterialType type = materialBuffer.typeTable.types[index.type];
	return MaterialReference(type.floatAttributesOffset + index.index * type.floatAttributeCount,
								type.intAttributesOffset + index.index * type.intAttributeCount,
								type.imageSamplers2DOffset + index.index * type.imageSampler2DCount);
}

function float getFloat(MaterialReference material, int index) {
	return materialBuffer.attributes.floats[material.floatAttributesOffset + index];
}

function int getInt(MaterialReference material, int index) {
	return materialBuffer.attributes.ints[material.intAttributesOffset + index];
}

function vec4 sampleTexture2D(MaterialReference material, int index, vec2 coord) {
	return texture(materialBuffer.imageSamplers2D[material.imageSamplers2DOffset + index], coord);
}
