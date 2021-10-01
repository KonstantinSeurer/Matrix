/*
 * VulkanDescriptorSet.h
 *
 *  Created on: Jun 26, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANDESCRIPTORSET_H_
#define GRAPHICS_VULKAN_GLFW_VULKANDESCRIPTORSET_H_

#include <graphics/DescriptorSet.h>
#include "VulkanDescriptorSetLayout.h"
#include <vulkan/vulkan.h>

namespace matrix
{

	namespace graphics
	{

		struct DescriptorSetAccess
		{
			String name;
			u32 index;
			Descriptor *descriptor;
		};

		class VulkanDescriptorSetAccessor : public DescriptorSetAccessor
		{
		private:
			Array<DescriptorSetAccess> *accesses;

		public:
			VulkanDescriptorSetAccessor(Array<DescriptorSetAccess> *accesses) : accesses(accesses)
			{
			}

			virtual void set(const String &name, u32 index, Descriptor *descriptor);
		};

		class VulkanDescriptorSet : public DescriptorSet
		{
		private:
			VulkanDescriptorSetAllocation alloc;
			VkDevice device;
			VulkanDescriptorSetLayout *layout;

		public:
			VulkanDescriptorSet(Ref<const DescriptorSetSource> source, VkDevice device, VulkanDescriptorSetLayout *layout);

			virtual ~VulkanDescriptorSet();

			virtual void access(Function<void(DescriptorSetAccessor &)> accessCallback);

			VkDescriptorSet getDescriptorSet() const
			{
				return alloc.descriptorSet;
			}
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANDESCRIPTORSET_H_ */
