/*
 * AccelerationStructure.h
 *
 *  Created on: Aug 11, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_ACCELERATIONSTRUCTURE_H_
#define GRAPHICS_ACCELERATIONSTRUCTURE_H_

#include "Descriptor.h"
#include "Buffer.h"
#include "../math/Mat4.h"
#include "../math/Vec3.h"

namespace matrix {

namespace graphics {

struct AccelerationStructureGeometry {
	Ref<ArrayBuffer> positionBuffer;
	Ref<ArrayBuffer> indexBuffer;
};

class BottomLevelAccelerationStructure {
public:
	BottomLevelAccelerationStructure() {
	}

	virtual ~BottomLevelAccelerationStructure() {
	}

	virtual void access(Function<void(AccelerationStructureGeometry&)> accessFunction) = 0;
};

struct AccelerationStructureInstance {
	Ref<BottomLevelAccelerationStructure> accelerationStructure;
	math::Mat4 transform;
	u32 index;
};

class TopLevelAccelerationStructure: public Descriptor {
private:
	u32 maxInstanceCount;
public:
	TopLevelAccelerationStructure(u32 maxInstanceCount) :
		Descriptor(DescriptorType::TOP_LEVEL_ACCELERATION_STRUCTURE), maxInstanceCount(maxInstanceCount) {
	}

	u32 getMaxInstanceCount() const {
		return maxInstanceCount;
	}

	virtual ~TopLevelAccelerationStructure() {
	}

	virtual void access(Function<void(Array<AccelerationStructureInstance>&)> accessFunction) = 0;
};

}

}

#endif /* GRAPHICS_ACCELERATIONSTRUCTURE_H_ */
