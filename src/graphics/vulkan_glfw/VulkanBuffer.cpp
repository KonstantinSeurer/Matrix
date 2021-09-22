/*
 * VulkanBuffer.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: konstantin
 */

#include "VulkanBuffer.h"

namespace matrix {

namespace graphics {

static VkBufferUsageFlags getVkBufferUsageFlags(StructBufferType type) {
	switch (type) {
		case StructBufferType::UNIFORM:
			return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		case StructBufferType::STORAGE:
			return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		default:
			err("Unsupported StructBufferType " + toString((u32) type) + "!");
			return 0;
	}
}

VulkanCPUStructBuffer::VulkanCPUStructBuffer(Ref<const StructType> source, const Array<Ref<const StructType>> &structs, BufferLayout layout,
	StructBufferType type, u32 unsizedArrayLength, VkDevice device, VulkanMemoryAllocator *allocator) :
	VulkanStructBuffer(type, MemoryType::CPU), device(device) {
	blueprint = createStructAccessorBlueprint(source, structs, layout, unsizedArrayLength);

	auto bufferMemory = allocator->createBuffer(blueprint->size, MemoryType::CPU, getVkBufferUsageFlags(type), 0);

	buffer = bufferMemory.first;
	memory = bufferMemory.second;

	assertVkResult(vkMapMemory(device, memory, 0, blueprint->size, 0, &data), SOURCE_LOCATION());
}

VulkanCPUStructBuffer::~VulkanCPUStructBuffer() {
	vkDestroyBuffer(device, buffer, null);
	vkFreeMemory(device, memory, null);
}

void VulkanCPUStructBuffer::access(Function<void(StructAccessor)> accessCallback) {
	accessCallback(StructAccessor(blueprint.get(), data));
}

static VkBufferUsageFlags getVkBufferUsageFlags(ArrayBufferType type) {
	switch (type) {
		case ArrayBufferType::VERTEX:
			return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		case ArrayBufferType::INDEX:
			return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		case ArrayBufferType::ACCELERATION_STRUCTURE_SOURCE:
			return VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
				| VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		default:
			err("Unsupported ArrayBufferType " + toString((u32) type) + "!");
			return 0;
	}
}

static VkMemoryAllocateFlags getVkMemoryAllocateFlags(ArrayBufferType type) {
	switch (type) {
		case ArrayBufferType::VERTEX:
			return 0;
		case ArrayBufferType::INDEX:
			return 0;
		case ArrayBufferType::ACCELERATION_STRUCTURE_SOURCE:
			return VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
		default:
			err("Unsupported ArrayBufferType " + toString((u32) type) + "!");
			return 0;
	}
}

VulkanCPUArrayBuffer::VulkanCPUArrayBuffer(Ref<const DataType> elementType, u32 elementCount, ArrayBufferType type, VkDevice device,
	VulkanMemoryAllocator *allocator) :
	VulkanArrayBuffer(type, MemoryType::CPU, elementCount, device), device(device) {

	blueprint = allocate<ArrayAccessorBlueprint>();
	blueprint->elementType = createArrayBufferElementBlueprint(elementType);
	blueprint->size = blueprint->elementType->size * elementCount;
	blueprint->alignment = 1;
	blueprint->length = elementCount;

	auto bufferMemory = allocator->createBuffer(blueprint->size, MemoryType::CPU, getVkBufferUsageFlags(type), getVkMemoryAllocateFlags(type));

	buffer = bufferMemory.first;
	memory = bufferMemory.second;

	VkBufferDeviceAddressInfoKHR deviceAddresInfo { };
	deviceAddresInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	deviceAddresInfo.buffer = buffer;
	address = vkGetBufferDeviceAddress(device, &deviceAddresInfo);

	assertVkResult(vkMapMemory(device, memory, 0, blueprint->size, 0, &data), SOURCE_LOCATION());
}

VulkanCPUArrayBuffer::~VulkanCPUArrayBuffer() {
	vkDestroyBuffer(device, buffer, null);
	vkFreeMemory(device, memory, null);
}

void VulkanCPUArrayBuffer::access(Function<void(ArrayAccessor)> accessCallback) {
	accessCallback(ArrayAccessor(blueprint.get(), data));
}

VulkanGPUArrayBuffer::VulkanGPUArrayBuffer(Ref<const DataType> elementType, u32 elementCount, ArrayBufferType type, VkDevice device, VkQueue queue,
	VulkanMemoryAllocator *allocator, VulkanSemaphoreChain *semaphoreChain, VkCommandBuffer accessCommandBuffer) :
	VulkanArrayBuffer(type, MemoryType::GPU, elementCount, device), device(device), queue(queue), semaphoreChain(semaphoreChain), accessCommandBuffer(
		accessCommandBuffer) {

	blueprint = allocate<ArrayAccessorBlueprint>();
	blueprint->elementType = createArrayBufferElementBlueprint(elementType);
	blueprint->size = blueprint->elementType->size * elementCount;
	blueprint->alignment = 1;
	blueprint->length = elementCount;

	auto stagingBufferMemory = allocator->createBuffer(blueprint->size, MemoryType::CPU, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 0);

	stagingBuffer = stagingBufferMemory.first;
	stagingMemory = stagingBufferMemory.second;

	assertVkResult(vkMapMemory(device, stagingMemory, 0, blueprint->size, 0, &data), SOURCE_LOCATION());

	auto bufferMemory = allocator->createBuffer(blueprint->size, MemoryType::GPU, VK_BUFFER_USAGE_TRANSFER_DST_BIT | getVkBufferUsageFlags(type),
		getVkMemoryAllocateFlags(type));

	buffer = bufferMemory.first;
	memory = bufferMemory.second;

	if (type == ArrayBufferType::ACCELERATION_STRUCTURE_SOURCE) {
		VkBufferDeviceAddressInfoKHR deviceAddresInfo { };
		deviceAddresInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		deviceAddresInfo.buffer = buffer;
		address = vkGetBufferDeviceAddress(device, &deviceAddresInfo);
	}
}

VulkanGPUArrayBuffer::~VulkanGPUArrayBuffer() {
	vkDestroyBuffer(device, buffer, null);
	vkFreeMemory(device, memory, null);

	vkDestroyBuffer(device, stagingBuffer, null);
	vkFreeMemory(device, stagingMemory, null);
}

void VulkanGPUArrayBuffer::access(Function<void(ArrayAccessor)> accessCallback) {
	accessCallback(ArrayAccessor(blueprint.get(), data));

	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = null;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = null;

	assertVkResult(vkBeginCommandBuffer(accessCommandBuffer, &beginInfo), SOURCE_LOCATION());

	VkBufferCopy region;
	region.srcOffset = 0;
	region.dstOffset = 0;
	region.size = blueprint->size;

	vkCmdCopyBuffer(accessCommandBuffer, stagingBuffer, buffer, 1, &region);

	assertVkResult(vkEndCommandBuffer(accessCommandBuffer), SOURCE_LOCATION());

	VkSubmitInfo submit;
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = null;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &accessCommandBuffer;

	Array<VkSemaphore> waitSemaphores = semaphoreChain->pollWaitSemaphores();
	submit.waitSemaphoreCount = waitSemaphores.size();
	submit.pWaitSemaphores = waitSemaphores.data();

	Array<VkPipelineStageFlags> waitStages(waitSemaphores.size());
	for (u32 i = 0; i < waitSemaphores.size(); i++) {
		waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	}
	submit.pWaitDstStageMask = waitStages.data();

	VkSemaphore signalSemaphore = semaphoreChain->getSemaphore();
	semaphoreChain->addWaitSemaphore(signalSemaphore);
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &signalSemaphore;

	assertVkResult(vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE), SOURCE_LOCATION());

	vkDeviceWaitIdle(device);
}

}

}
