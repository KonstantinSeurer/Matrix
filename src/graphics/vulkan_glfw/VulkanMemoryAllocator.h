/*
 * VulkanMemoryAllocator.h
 *
 *  Created on: Jun 30, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANMEMORYALLOCATOR_H_
#define GRAPHICS_VULKAN_GLFW_VULKANMEMORYALLOCATOR_H_

#include <vulkan/vulkan.h>

#include <Base.h>
#include <graphics/Memory.h>
#include <graphics/Image.h>

namespace matrix
{

	namespace graphics
	{

		struct VulkanAccelerationStructureData
		{
		public:
			VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
			VkDeviceAddress address = 0;
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkBuffer buffer = VK_NULL_HANDLE;
		};

		class VulkanMemoryAllocator
		{
		private:
			VkDevice device;
			VkPhysicalDevice physicalDevice;
			u32 queueFamilyIndex;

			PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
			PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;

		public:
			VulkanMemoryAllocator(VkDevice device, VkPhysicalDevice physicalDevice, u32 queueFamilyIndex,
								  PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR,
								  PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR) : device(device), physicalDevice(physicalDevice), queueFamilyIndex(queueFamilyIndex), vkCreateAccelerationStructureKHR(vkCreateAccelerationStructureKHR), vkGetAccelerationStructureDeviceAddressKHR(vkGetAccelerationStructureDeviceAddressKHR)
			{
			}

			Pair<VkBuffer, VkDeviceMemory> createBuffer(u32 size, MemoryType memoryType, VkBufferUsageFlags usage, VkMemoryAllocateFlags allocateFlags);

			Pair<VkImage, VkDeviceMemory> createImage2D(u32 width, u32 height, u32 levels, VkFormat format, ImageUsage usage);

			Pair<VkImage, VkDeviceMemory> createImage3D(u32 width, u32 height, u32 length, u32 levels, VkFormat format, ImageUsage usage);

			VulkanAccelerationStructureData createAccelerationStructure(VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSize);
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANMEMORYALLOCATOR_H_ */
