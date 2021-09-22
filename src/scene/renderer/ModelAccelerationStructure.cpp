/*
 * ModelAccelerationStructure.cpp
 *
 *  Created on: Aug 14, 2021
 *      Author: konstantin
 */

#include "ModelAccelerationStructure.h"

namespace matrix {

using namespace graphics;
using namespace math;

namespace scene {

void ModelAccelerationStructure::init(const ModelType *type, Ref<graphics::DeviceInstance> device, const Array<void*> &attributes, u32 vertexCount,
	const Array<u32> &indices) {

	auto positionBuffer = device->createArrayBuffer(getPrimitiveType(PrimitiveTypeType::VEC3), vertexCount, ArrayBufferType::ACCELERATION_STRUCTURE_SOURCE,
		MemoryType::GPU);
	positionBuffer->access([&](ArrayAccessor accessor) {
		Vec3 *data = (Vec3*) attributes[type->getPositionAttributeIndex()];
		for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
			accessor.setVec3(vertexIndex, data[vertexIndex]);
		}
	});

	auto indexBuffer = device->createArrayBuffer(getPrimitiveType(PrimitiveTypeType::INT), indices.size(), ArrayBufferType::ACCELERATION_STRUCTURE_SOURCE,
		MemoryType::GPU);
	indexBuffer->access([&](ArrayAccessor accessor) {
		for (u32 indexIndex = 0; indexIndex < indices.size(); indexIndex++) {
			accessor.setInt(indexIndex, indices[indexIndex]);
		}
	});

	accelerationStructure = device->createBottomLevelAccelerationStructure();
	accelerationStructure->access([&](AccelerationStructureGeometry &geometry) {
		geometry.positionBuffer = positionBuffer;
		geometry.indexBuffer = indexBuffer;
	});
}

}

}

