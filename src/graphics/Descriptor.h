/*
 * Descriptor.h
 *
 *  Created on: Jun 26, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_DESCRIPTOR_H_
#define GRAPHICS_DESCRIPTOR_H_

#include "../Base.h"

namespace matrix {

namespace graphics {

enum class DescriptorType {
	UNIFORM_BUFFER, STORAGE_BUFFER, IMAGE_SAMPLER_2D, IMAGE_2D, IMAGE_3D, IMAGE_SAMPLER_3D, TOP_LEVEL_ACCELERATION_STRUCTURE
};

class Descriptor {
private:
	DescriptorType descriptorType;
public:
	Descriptor() :
			descriptorType(DescriptorType::IMAGE_SAMPLER_2D) {
	}

	Descriptor(DescriptorType descriptorType) :
			descriptorType(descriptorType) {
	}

	virtual ~Descriptor() {
	}

	DescriptorType getDescriptorType() const {
		return descriptorType;
	}
};

}

}

#endif /* GRAPHICS_DESCRIPTOR_H_ */
