/*
 * VulkanAccelerationStructure.cpp
 *
 *  Created on: Aug 11, 2021
 *      Author: konstantin
 */

#include "VulkanAccelerationStructure.h"
#include "VulkanBuffer.h"

namespace matrix {

namespace graphics {

VulkanAccelerationStructure::VulkanAccelerationStructure(VulkanDeviceInstance *device, VulkanMemoryAllocator *allocator, VkCommandBuffer accessCommandBuffer) :
	device(device), allocator(allocator), accessCommandBuffer(accessCommandBuffer) {

}

VulkanAccelerationStructure::~VulkanAccelerationStructure() {
	destroyData();
}

void VulkanAccelerationStructure::destroyData() {
	if (data.accelerationStructure) {
		device->vkDestroyAccelerationStructureKHR(device->getDevice(), data.accelerationStructure, null);
		vkDestroyBuffer(device->getDevice(), data.buffer, null);
		vkFreeMemory(device->getDevice(), data.memory, null);
	}
}

VulkanBottomLevelAccelerationStructure::VulkanBottomLevelAccelerationStructure(VulkanDeviceInstance *device, VulkanMemoryAllocator *allocator,
	VkCommandBuffer accessCommandBuffer) :
	VulkanAccelerationStructure(device, allocator, accessCommandBuffer) {

}

VulkanBottomLevelAccelerationStructure::~VulkanBottomLevelAccelerationStructure() {

}

void VulkanBottomLevelAccelerationStructure::access(Function<void(AccelerationStructureGeometry&)> accessFunction) {
	destroyData();

	AccelerationStructureGeometry accessData;
	accessFunction(accessData);

	VkDeviceOrHostAddressConstKHR vertexBufferAddress { };
	vertexBufferAddress.deviceAddress = CastDown<VulkanArrayBuffer>(accessData.positionBuffer)->getAddress();

	VkDeviceOrHostAddressConstKHR indexBufferAddress { };
	indexBufferAddress.deviceAddress = CastDown<VulkanArrayBuffer>(accessData.indexBuffer)->getAddress();

	u32 triangleCount = accessData.indexBuffer->length / 3;
	u32 vertexCount = accessData.positionBuffer->length;

	VkAccelerationStructureGeometryKHR geometry { };
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

	geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;

	geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;

	geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	geometry.geometry.triangles.vertexData = vertexBufferAddress;
	geometry.geometry.triangles.vertexStride = sizeof(math::Vec3);
	geometry.geometry.triangles.maxVertex = vertexCount;

	geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	geometry.geometry.triangles.indexData = indexBufferAddress;

	geometry.geometry.triangles.transformData.deviceAddress = 0;
	geometry.geometry.triangles.transformData.hostAddress = null;

	VkAccelerationStructureBuildGeometryInfoKHR buildGeometry { };
	buildGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	buildGeometry.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;

	buildGeometry.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

	buildGeometry.geometryCount = 1;
	buildGeometry.pGeometries = &geometry;

	VkAccelerationStructureBuildSizesInfoKHR buildSizes { };
	buildSizes.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

	device->vkGetAccelerationStructureBuildSizesKHR(device->getDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildGeometry, &triangleCount,
		&buildSizes);

	data = allocator->createAccelerationStructure(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, buildSizes);

	auto scratchBuffer = allocator->createBuffer(buildSizes.buildScratchSize, MemoryType::GPU,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT);

	VkBufferDeviceAddressInfo scratchAddressInfo { };
	scratchAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	scratchAddressInfo.buffer = scratchBuffer.first;

	VkDeviceAddress scratchAddress = vkGetBufferDeviceAddress(device->getDevice(), &scratchAddressInfo);

	VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo { };
	buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	buildGeometryInfo.dstAccelerationStructure = data.accelerationStructure;
	buildGeometryInfo.geometryCount = 1;
	buildGeometryInfo.pGeometries = &geometry;
	buildGeometryInfo.scratchData.deviceAddress = scratchAddress;

	VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo { };
	buildRangeInfo.primitiveCount = triangleCount;
	buildRangeInfo.primitiveOffset = 0;
	buildRangeInfo.firstVertex = 0;
	buildRangeInfo.transformOffset = 0;

	const VkAccelerationStructureBuildRangeInfoKHR *const pBuildRangeInfo = &buildRangeInfo;
	const VkAccelerationStructureBuildRangeInfoKHR *const*ppBuildRangeInfo = &pBuildRangeInfo;

	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = null;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = null;

	assertVkResult(vkBeginCommandBuffer(accessCommandBuffer, &beginInfo), SOURCE_LOCATION());

	device->vkCmdBuildAccelerationStructuresKHR(accessCommandBuffer, 1, &buildGeometryInfo, ppBuildRangeInfo);

	assertVkResult(vkEndCommandBuffer(accessCommandBuffer), SOURCE_LOCATION());

	VkSubmitInfo submit;
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = null;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &accessCommandBuffer;

	Array<VkSemaphore> waitSemaphores = device->getSemaphoreChain()->pollWaitSemaphores();
	submit.waitSemaphoreCount = waitSemaphores.size();
	submit.pWaitSemaphores = waitSemaphores.data();

	Array<VkPipelineStageFlags> waitStages(waitSemaphores.size());
	for (u32 i = 0; i < waitSemaphores.size(); i++) {
		waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	}
	submit.pWaitDstStageMask = waitStages.data();

	VkSemaphore signalSemaphore = device->getSemaphoreChain()->getSemaphore();
	device->getSemaphoreChain()->addWaitSemaphore(signalSemaphore);
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &signalSemaphore;

	assertVkResult(vkQueueSubmit(device->getQueue(), 1, &submit, VK_NULL_HANDLE), SOURCE_LOCATION());

	assertVkResult(vkDeviceWaitIdle(device->getDevice()), SOURCE_LOCATION());

	vkDestroyBuffer(device->getDevice(), scratchBuffer.first, null);
	vkFreeMemory(device->getDevice(), scratchBuffer.second, null);
}

VulkanTopLevelAccelerationStructure::VulkanTopLevelAccelerationStructure(VulkanDeviceInstance *device, VulkanMemoryAllocator *allocator,
	VkCommandBuffer accessCommandBuffer, u32 maxInstanceCount) :
	TopLevelAccelerationStructure(maxInstanceCount), VulkanAccelerationStructure(device, allocator, accessCommandBuffer) {

	auto instanceBufferMemory = allocator->createBuffer(maxInstanceCount * sizeof(VkAccelerationStructureInstanceKHR), MemoryType::CPU,
		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
		VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT);

	instanceBuffer = instanceBufferMemory.first;
	instanceMemory = instanceBufferMemory.second;

	VkBufferDeviceAddressInfo instanceAddressInfo { };
	instanceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	instanceAddressInfo.buffer = instanceBuffer;

	instanceAddress = vkGetBufferDeviceAddress(device->getDevice(), &instanceAddressInfo);
}

VulkanTopLevelAccelerationStructure::~VulkanTopLevelAccelerationStructure() {
	vkDestroyBuffer(device->getDevice(), instanceBuffer, null);
	vkFreeMemory(device->getDevice(), instanceMemory, null);
}

void VulkanTopLevelAccelerationStructure::access(Function<void(Array<AccelerationStructureInstance>&)> accessFunction) {
	destroyData();

	Array<AccelerationStructureInstance> instances;
	accessFunction(instances);

	u32 instanceCount = instances.size();

	void *instanceDataVoid;
	assertVkResult(
		vkMapMemory(device->getDevice(), instanceMemory, 0, getMaxInstanceCount() * sizeof(VkAccelerationStructureInstanceKHR), 0, &instanceDataVoid),
		SOURCE_LOCATION());
	VkAccelerationStructureInstanceKHR *instanceData = (VkAccelerationStructureInstanceKHR*) instanceDataVoid;

	for (u32 instanceIndex = 0; instanceIndex < instanceCount; instanceIndex++) {
		AccelerationStructureInstance *srcInstance = &(instances[instanceIndex]);
		VkAccelerationStructureInstanceKHR *dstInstance = &(instanceData[instanceIndex]);

		math::Mat4 transform = srcInstance->transform;
		dstInstance->transform = { transform.x.x, transform.y.x, transform.z.x, transform.w.x, transform.x.y, transform.y.y, transform.z.y, transform.w.y,
			transform.x.z, transform.y.z, transform.z.z, transform.w.z };
		dstInstance->instanceCustomIndex = srcInstance->index;
		dstInstance->mask = 0xFF;
		dstInstance->instanceShaderBindingTableRecordOffset = 0;
		dstInstance->flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		dstInstance->accelerationStructureReference = CastDown<VulkanBottomLevelAccelerationStructure>(srcInstance->accelerationStructure)->getData().address;
	}

	vkUnmapMemory(device->getDevice(), instanceMemory);

	VkDeviceOrHostAddressConstKHR instanceBufferAddress { };
	instanceBufferAddress.deviceAddress = instanceAddress;

	VkAccelerationStructureGeometryKHR geometry { };
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	geometry.geometry.instances.arrayOfPointers = VK_FALSE;
	geometry.geometry.instances.data = instanceBufferAddress;

	VkAccelerationStructureBuildGeometryInfoKHR buildGeometry { };
	buildGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	buildGeometry.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	buildGeometry.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	buildGeometry.geometryCount = 1;
	buildGeometry.pGeometries = &geometry;

	VkAccelerationStructureBuildSizesInfoKHR buildSizes { };
	buildSizes.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

	device->vkGetAccelerationStructureBuildSizesKHR(device->getDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildGeometry, &instanceCount,
		&buildSizes);

	data = allocator->createAccelerationStructure(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, buildSizes);

	auto scratchBuffer = allocator->createBuffer(buildSizes.buildScratchSize, MemoryType::GPU,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT);

	VkBufferDeviceAddressInfo scratchAddressInfo { };
	scratchAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	scratchAddressInfo.buffer = scratchBuffer.first;

	VkDeviceAddress scratchAddress = vkGetBufferDeviceAddress(device->getDevice(), &scratchAddressInfo);

	VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo { };
	buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	buildGeometryInfo.dstAccelerationStructure = data.accelerationStructure;
	buildGeometryInfo.geometryCount = 1;
	buildGeometryInfo.pGeometries = &geometry;
	buildGeometryInfo.scratchData.deviceAddress = scratchAddress;

	VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo { };
	buildRangeInfo.primitiveCount = instanceCount;
	buildRangeInfo.primitiveOffset = 0;
	buildRangeInfo.firstVertex = 0;
	buildRangeInfo.transformOffset = 0;

	const VkAccelerationStructureBuildRangeInfoKHR *const pBuildRangeInfo = &buildRangeInfo;
	const VkAccelerationStructureBuildRangeInfoKHR *const*ppBuildRangeInfo = &pBuildRangeInfo;

	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = null;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = null;

	assertVkResult(vkBeginCommandBuffer(accessCommandBuffer, &beginInfo), SOURCE_LOCATION());

	device->vkCmdBuildAccelerationStructuresKHR(accessCommandBuffer, 1, &buildGeometryInfo, ppBuildRangeInfo);

	assertVkResult(vkEndCommandBuffer(accessCommandBuffer), SOURCE_LOCATION());

	VkSubmitInfo submit;
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = null;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &accessCommandBuffer;

	Array<VkSemaphore> waitSemaphores = device->getSemaphoreChain()->pollWaitSemaphores();
	submit.waitSemaphoreCount = waitSemaphores.size();
	submit.pWaitSemaphores = waitSemaphores.data();

	Array<VkPipelineStageFlags> waitStages(waitSemaphores.size());
	for (u32 i = 0; i < waitSemaphores.size(); i++) {
		waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	}
	submit.pWaitDstStageMask = waitStages.data();

	VkSemaphore signalSemaphore = device->getSemaphoreChain()->getSemaphore();
	device->getSemaphoreChain()->addWaitSemaphore(signalSemaphore);
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &signalSemaphore;

	assertVkResult(vkQueueSubmit(device->getQueue(), 1, &submit, VK_NULL_HANDLE), SOURCE_LOCATION());

	assertVkResult(vkDeviceWaitIdle(device->getDevice()), SOURCE_LOCATION());

	vkDestroyBuffer(device->getDevice(), scratchBuffer.first, null);
	vkFreeMemory(device->getDevice(), scratchBuffer.second, null);
}

}

}
