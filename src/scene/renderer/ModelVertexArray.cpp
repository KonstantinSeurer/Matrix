/*
 * ModelVertexArray.cpp
 *
 *  Created on: Aug 8, 2021
 *      Author: konstantin
 */

#include "ModelVertexArray.h"
#include "../../Logging.h"

namespace matrix {

using namespace graphics;
using namespace math;

namespace scene {

void ModelVertexArray::init(const ModelType *type, Ref<graphics::DeviceInstance> device, const Array<void*> &attributes, u32 vertexCount,
	const Array<u32> &indices) {
	Array<VariableSource> layout;
	for (auto attribute : type->getAttributes()) {
		layout.emplace_back(getPrimitiveType(attribute.first), attribute.second);
	}

	vertexBuffer = device->createArrayBuffer(layout, vertexCount, ArrayBufferType::VERTEX, MemoryType::GPU);
	vertexBuffer->access([&](ArrayAccessor accessor) {
		const auto &typeAttributes = type->getAttributes();
		for (u32 attributeIndex = 0; attributeIndex < typeAttributes.size(); attributeIndex++) {
			const auto &attribute = typeAttributes[attributeIndex];
			switch (attribute.first) {
				case PrimitiveTypeType::FLOAT: {
					f32 *data = (f32*) attributes[attributeIndex];
					for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
						accessor.getStruct(vertexIndex).setFloat(attribute.second, data[vertexIndex]);
					}
					break;
				}
				case PrimitiveTypeType::VEC2: {
					Vec2 *data = (Vec2*) attributes[attributeIndex];
					for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
						accessor.getStruct(vertexIndex).setVec2(attribute.second, data[vertexIndex]);
					}
					break;
				}
				case PrimitiveTypeType::VEC3: {
					Vec3 *data = (Vec3*) attributes[attributeIndex];
					for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
						accessor.getStruct(vertexIndex).setVec3(attribute.second, data[vertexIndex]);
					}
					break;
				}
				case PrimitiveTypeType::VEC4: {
					Vec4 *data = (Vec4*) attributes[attributeIndex];
					for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
						accessor.getStruct(vertexIndex).setVec4(attribute.second, data[vertexIndex]);
					}
					break;
				}
				default:
					err("Unsupported vertex attribute type " + toString((u32) attribute.first) + "!");
					break;
			}
		}
	});

	indexBuffer = device->createArrayBuffer(getPrimitiveType(PrimitiveTypeType::INT), indices.size(), ArrayBufferType::INDEX, MemoryType::GPU);
	indexBuffer->access([&] (ArrayAccessor accessor) {
		for (u32 indexIndex = 0; indexIndex < indices.size(); indexIndex++) {
			accessor.setInt(indexIndex, indices[indexIndex]);
		}
	});
}

}

}
