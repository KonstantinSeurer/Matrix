/*
 * VulkanAccelerationStructure.h
 *
 *  Created on: Aug 11, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANACCELERATIONSTRUCTURE_H_
#define GRAPHICS_VULKAN_GLFW_VULKANACCELERATIONSTRUCTURE_H_

#include <graphics/AccelerationStructure.h>
#include "VulkanDevice.h"

namespace matrix
{

	namespace graphics
	{

		class VulkanAccelerationStructure
		{
		protected:
			VulkanDeviceInstance *device;
			VulkanMemoryAllocator *allocator;
			VulkanAccelerationStructureData data;
			VkCommandBuffer accessCommandBuffer;

		public:
			VulkanAccelerationStructure(VulkanDeviceInstance *device, VulkanMemoryAllocator *allocator, VkCommandBuffer accessCommandBuffer);
			~VulkanAccelerationStructure();

			void destroyData();

			const VulkanAccelerationStructureData &getData() const
			{
				return data;
			}
		};

		class VulkanBottomLevelAccelerationStructure : public BottomLevelAccelerationStructure, public VulkanAccelerationStructure
		{
		public:
			VulkanBottomLevelAccelerationStructure(VulkanDeviceInstance *device, VulkanMemoryAllocator *allocator, VkCommandBuffer accessCommandBuffer);

			virtual ~VulkanBottomLevelAccelerationStructure();

			virtual void access(Function<void(AccelerationStructureGeometry &)> accessFunction);
		};

		class VulkanTopLevelAccelerationStructure : public TopLevelAccelerationStructure, public VulkanAccelerationStructure
		{
		private:
			VkBuffer instanceBuffer;
			VkDeviceMemory instanceMemory;
			VkDeviceAddress instanceAddress;

		public:
			VulkanTopLevelAccelerationStructure(VulkanDeviceInstance *device, VulkanMemoryAllocator *allocator, VkCommandBuffer accessCommandBuffer, u32 maxInstanceCount);
			virtual ~VulkanTopLevelAccelerationStructure();

			virtual void access(Function<void(Array<AccelerationStructureInstance> &)> accessFunction);
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANACCELERATIONSTRUCTURE_H_ */
