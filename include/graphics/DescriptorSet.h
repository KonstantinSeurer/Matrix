/*
 * DescriptorSet.h
 *
 *  Created on: Jun 26, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_DESCRIPTORSET_H_
#define GRAPHICS_DESCRIPTORSET_H_

#include "Descriptor.h"
#include "Pipeline.h"
#include "../Base.h"

namespace matrix {

namespace graphics {

class DescriptorSetAccessor {
public:
	virtual ~DescriptorSetAccessor() {
	}

	void set(const String &name, Descriptor* descriptor) {
		set(name, 0, descriptor);
	}

	virtual void set(const String &name, u32 index, Descriptor* descriptor) = 0;
};

class DescriptorSet {
public:
	const Ref<const DescriptorSetSource> source;

	DescriptorSet(Ref<const DescriptorSetSource> source) : source(source) {
	}

	virtual ~DescriptorSet() {
	}

	virtual void access(Function<void(DescriptorSetAccessor&)> accessCallback) = 0;
};

}

}

#endif /* GRAPHICS_DESCRIPTORSET_H_ */
