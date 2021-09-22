/*
 * VulkanDescriptorSet.cpp
 *
 *  Created on: Jun 26, 2021
 *      Author: konstantin
 */

#include "VulkanDescriptorSet.h"
#include "VulkanUtil.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanAccelerationStructure.h"

namespace matrix {

namespace graphics {

VulkanDescriptorSet::VulkanDescriptorSet(Ref<const DescriptorSetSource> source, VkDevice device, VulkanDescriptorSetLayout *layout) :
	DescriptorSet(source), device(device), layout(layout) {
	alloc = layout->alloc();
}

VulkanDescriptorSet::~VulkanDescriptorSet() {
	layout->free(alloc);
}

void VulkanDescriptorSet::access(Function<void(DescriptorSetAccessor&)> accessCallback) {
	Array<DescriptorSetAccess> accesses;
	VulkanDescriptorSetAccessor accessor(&accesses);
	accessCallback(accessor);

	if (accesses.empty()) {
		return;
	}

	Array<VkDescriptorBufferInfo*> buffers;
	Array<VkDescriptorImageInfo*> images;
	Array<VkWriteDescriptorSetAccelerationStructureKHR*> accelerationStructures;
	Array<VkWriteDescriptorSet> writes;
	for (const auto &access : accesses) {
		VkWriteDescriptorSet write { };
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorCount = 1;
		write.descriptorType = getVkDescriptorType(access.descriptor->getDescriptorType());
		write.dstArrayElement = access.index;
		bool found = false;
		for (u32 binding = 0; binding < source->descriptors.size(); binding++) {
			if (access.name == source->descriptors[binding]->name) {
				write.dstBinding = binding;
				found = true;
				break;
			}
		}
		if (!found) {
			err("Could not find descriptor " + access.name + "!");
		}
		write.dstSet = alloc.descriptorSet;

		switch (access.descriptor->getDescriptorType()) {
			case DescriptorType::UNIFORM_BUFFER:
			case DescriptorType::STORAGE_BUFFER: {
				auto buffer = CastDown<VulkanStructBuffer>(access.descriptor);
				VkDescriptorBufferInfo *bufferInfo = new VkDescriptorBufferInfo();
				buffers.push_back(bufferInfo);
				bufferInfo->buffer = buffer->getBuffer();
				bufferInfo->offset = 0;
				bufferInfo->range = VK_WHOLE_SIZE;
				write.pBufferInfo = bufferInfo;
				break;
			}
			case DescriptorType::IMAGE_SAMPLER_2D: {
				VkDescriptorImageInfo *imageInfo = new VkDescriptorImageInfo();
				images.push_back(imageInfo);
				auto imageSampler = CastDown<ImageSampler2D>(access.descriptor);
				auto vulkanImageView = CastDown<VulkanImageView2D>(imageSampler->image);
				imageInfo->imageLayout = getVkImageLayout(ImageLayout::SHADER_READ_ONLY, vulkanImageView->getType());
				imageInfo->imageView = vulkanImageView->getView();
				imageInfo->sampler = CastDown<VulkanSampler2D>(imageSampler->sampler)->getSampler();
				write.pImageInfo = imageInfo;
				break;
			}
			case DescriptorType::IMAGE_2D: {
				VkDescriptorImageInfo *imageInfo = new VkDescriptorImageInfo();
				images.push_back(imageInfo);
				imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				auto image = CastDown<VulkanImageView2D>(access.descriptor);
				imageInfo->imageView = image->getView();
				imageInfo->sampler = VK_NULL_HANDLE;
				write.pImageInfo = imageInfo;
				break;
			}
			case DescriptorType::TOP_LEVEL_ACCELERATION_STRUCTURE: {
				VkWriteDescriptorSetAccelerationStructureKHR *asInfo = new VkWriteDescriptorSetAccelerationStructureKHR();
				accelerationStructures.push_back(asInfo);
				auto accelerationStructure = CastDown<VulkanTopLevelAccelerationStructure>(access.descriptor);
				asInfo->pNext = null;
				asInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
				asInfo->accelerationStructureCount = 1;
				asInfo->pAccelerationStructures = &(accelerationStructure->getData().accelerationStructure);
				write.pNext = asInfo;
				break;
			}
		}

		writes.push_back(write);
	}

	vkUpdateDescriptorSets(device, writes.size(), writes.data(), 0, null);

	for (auto buffer : buffers) {
		delete buffer;
	}
	for (auto image : images) {
		delete image;
	}
	for (auto accelerationStructure : accelerationStructures) {
		delete accelerationStructure;
	}
}

void VulkanDescriptorSetAccessor::set(const String &name, u32 index, Descriptor* descriptor) {
	DescriptorSetAccess access;
	access.name = name;
	access.index = index;
	access.descriptor = descriptor;

	accesses->push_back(access);
}

}

}
