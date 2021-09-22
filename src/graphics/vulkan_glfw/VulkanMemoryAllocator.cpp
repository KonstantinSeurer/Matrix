/*
 * VulkanMemoryAllocator.cpp
 *
 *  Created on: Jun 30, 2021
 *      Author: konstantin
 */

#include "VulkanMemoryAllocator.h"
#include "VulkanUtil.h"

namespace matrix {

namespace graphics {

static u32 getMemoryType(VkPhysicalDevice physicalDevice, u32 type, VkMemoryPropertyFlags flags) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((type & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
			return i;
		}
	}

	err("Could not find memory type with type=" + toString(type) + " flags=" + toString((u32) flags) + "!");
	return 0;
}

static UnorderedMap<MemoryType, VkMemoryPropertyFlags> memoryTypes = { { MemoryType::CPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }, { MemoryType::GPU, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT } };

Pair<VkBuffer, VkDeviceMemory> VulkanMemoryAllocator::createBuffer(u32 size, MemoryType memoryType, VkBufferUsageFlags usage,
	VkMemoryAllocateFlags allocateFlags) {
	VkBufferCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext = null;
	createInfo.flags = 0;

	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.size = size;

	createInfo.queueFamilyIndexCount = 1;
	createInfo.pQueueFamilyIndices = &queueFamilyIndex;

	VkBuffer buffer;
	assertVkResult(vkCreateBuffer(device, &createInfo, null, &buffer), SOURCE_LOCATION());

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	VkMemoryAllocateFlagsInfo allocateFlagsInfo { };
	allocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	allocateFlagsInfo.flags = allocateFlags;

	VkMemoryAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = &allocateFlagsInfo;
	allocateInfo.memoryTypeIndex = getMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, memoryTypes[memoryType]);
	allocateInfo.allocationSize = memoryRequirements.size;

	VkDeviceMemory memory;
	assertVkResult(vkAllocateMemory(device, &allocateInfo, null, &memory), SOURCE_LOCATION());

	assertVkResult(vkBindBufferMemory(device, buffer, memory, 0), SOURCE_LOCATION());

	return {buffer, memory};
}

static VkImageUsageFlags getVkImageUsage(ImageUsage usage) {
	VkImageUsageFlags result = 0;
	if (usage & ImageUsage::ACCESS) {
		result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	if (usage & ImageUsage::PRESENT) {
		err("Unsupported ImageUsage IMAGE_USAGE_PRESENT_BIT!");
	}
	if (usage & ImageUsage::SAMPLED) {
		result |= VK_IMAGE_USAGE_SAMPLED_BIT;
	}
	if (usage & ImageUsage::TRANSFER_DESTINATION) {
		result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	if (usage & ImageUsage::TRANSFER_SOURCE) {
		result |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (usage & ImageUsage::COLOR_ATTACHMENT) {
		result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	if ((usage & ImageUsage::DEPTH_ATTACHMENT) || (usage & ImageUsage::STENCIL_ATTACHMENT)) {
		result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}
	if (usage & ImageUsage::STORAGE) {
		result |= VK_IMAGE_USAGE_STORAGE_BIT;
	}
	return result;
}

Pair<VkImage, VkDeviceMemory> VulkanMemoryAllocator::createImage2D(u32 width, u32 height, u32 levels, VkFormat format, ImageUsage usage) {
	VkImageCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = null;
	createInfo.flags = 0;

	createInfo.extent = { width, height, 1 };
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.mipLevels = levels;
	createInfo.queueFamilyIndexCount = 1;
	createInfo.pQueueFamilyIndices = &queueFamilyIndex;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = getVkImageUsage(usage);

	VkImage image;
	assertVkResult(vkCreateImage(device, &createInfo, null, &image), SOURCE_LOCATION());

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = null;
	allocateInfo.memoryTypeIndex = getMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	allocateInfo.allocationSize = memoryRequirements.size;

	VkDeviceMemory memory;
	assertVkResult(vkAllocateMemory(device, &allocateInfo, null, &memory), SOURCE_LOCATION());

	assertVkResult(vkBindImageMemory(device, image, memory, 0), SOURCE_LOCATION());

	return {image, memory};
}

VulkanAccelerationStructureData VulkanMemoryAllocator::createAccelerationStructure(VkAccelerationStructureTypeKHR type,
	VkAccelerationStructureBuildSizesInfoKHR buildSize) {
	VulkanAccelerationStructureData result;

	auto bufferMemory = createBuffer(buildSize.accelerationStructureSize, MemoryType::GPU,
		VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR);

	result.buffer = bufferMemory.first;
	result.memory = bufferMemory.second;

	VkAccelerationStructureCreateInfoKHR createInfo { };
	createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	createInfo.buffer = result.buffer;
	createInfo.size = buildSize.accelerationStructureSize;
	createInfo.type = type;

	assertVkResult(vkCreateAccelerationStructureKHR(device, &createInfo, nullptr, &(result.accelerationStructure)), SOURCE_LOCATION());

	VkAccelerationStructureDeviceAddressInfoKHR deviceAddressInfo { };
	deviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	deviceAddressInfo.accelerationStructure = result.accelerationStructure;

	result.address = vkGetAccelerationStructureDeviceAddressKHR(device, &deviceAddressInfo);

	return result;
}

}

}
