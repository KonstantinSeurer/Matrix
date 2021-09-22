/*
 * VulkanDescriptorSetLayout.h
 *
 *  Created on: Jun 29, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANDESCRIPTORSETLAYOUT_H_
#define GRAPHICS_VULKAN_GLFW_VULKANDESCRIPTORSETLAYOUT_H_

#include "VulkanUtil.h"

namespace matrix {

namespace graphics {

class VulkanDescriptorSetLayout;

struct VulkanDescriptorSetAllocation {
	VkDescriptorPool pool;
	VkDescriptorSet descriptorSet;
	u32 poolIndex;
};

class VulkanDescriptorSetLayout {
private:
	VkDescriptorSetLayout layout;
	Array<VkDescriptorPool> pools;
	Array<u32> poolAllocCounts;
	Array<VkDescriptorPoolSize> poolSizes;
	VkDescriptorPoolCreateInfo poolCreateInfo;
	VkDevice device;
	u32 poolSize;
public:
	VulkanDescriptorSetLayout(Ref<const DescriptorSetSource> source, VkDevice device, u32 poolSize);

	~VulkanDescriptorSetLayout();

	VulkanDescriptorSetAllocation alloc();
	void free(VulkanDescriptorSetAllocation alloc);

	VkDescriptorSetLayout getLayout() const {
		return layout;
	}
};

class VulkanDescriptorSetLayoutCache {
private:
	VkDevice device;
	UnorderedMap<Ref<const DescriptorSetSource>, Ref<VulkanDescriptorSetLayout>> data;
public:
	VulkanDescriptorSetLayoutCache(VkDevice device);

	VulkanDescriptorSetLayout* get(Ref<const DescriptorSetSource> source);
};

}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANDESCRIPTORSETLAYOUT_H_ */
