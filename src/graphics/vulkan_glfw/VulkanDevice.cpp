/*
 * VulkanGraphicsDevice.cpp
 *
 *  Created on: Jun 4, 2021
 *      Author: konstantin
 */

#include <vulkan/vulkan.h>

#include "VulkanUtil.h"
#include <cstring>
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "GLFWVulkanWindow.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"
#include "VulkanFrameBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanFence.h"
#include "../../Time.h"
#include "VulkanPipeline.h"
#include "VulkanAccelerationStructure.h"

namespace matrix
{

namespace graphics
{

VulkanDeviceInstance::VulkanDeviceInstance(VkPhysicalDevice physicalDevice) :
	physicalDevice(physicalDevice)
{
	u32 queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
	null);

	Array<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	bool foundQueueFamily = false;

	for (u32 i = 0; i < queueFamilyCount; i++)
	{
		const VkQueueFamilyProperties &family = queueFamilyProperties[i];
		VkQueueFlags flags = family.queueFlags;

		bool graphics = flags & VK_QUEUE_GRAPHICS_BIT;
		bool compute = flags & VK_QUEUE_COMPUTE_BIT;
		bool transfer = flags & VK_QUEUE_TRANSFER_BIT;

		if (graphics && compute && transfer)
		{
			queueFamilyIndex = i;
			foundQueueFamily = true;
			break;
		}
	}

	if (!foundQueueFamily)
	{
		std::cerr << "Could not find suitable queue family!" << std::endl;
		return;
	}

	Array<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, VK_KHR_RAY_QUERY_EXTENSION_NAME,
	VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME };
	Array<const char*> layers;

	VkDeviceQueueCreateInfo queueInfo;
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = null;
	queueInfo.flags = 0;
	queueInfo.queueCount = 1;
	queueInfo.queueFamilyIndex = queueFamilyIndex;
	f32 queuePriority = 1.0f;
	queueInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures { };
	rayQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
	rayQueryFeatures.rayQuery = VK_TRUE;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures { };
	accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	accelerationStructureFeatures.pNext = &rayQueryFeatures;
	accelerationStructureFeatures.accelerationStructure = VK_TRUE;

	VkPhysicalDeviceVulkan12Features vulkan12Features { };
	vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	vulkan12Features.pNext = &accelerationStructureFeatures;
	vulkan12Features.descriptorBindingPartiallyBound = VK_TRUE;
	vulkan12Features.separateDepthStencilLayouts = VK_TRUE;
	vulkan12Features.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceFeatures features { };
	features.tessellationShader = VK_TRUE;
	features.geometryShader = VK_TRUE;

	VkDeviceCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pNext = &vulkan12Features;
	info.flags = 0;

	info.enabledExtensionCount = extensions.size();
	info.ppEnabledExtensionNames = extensions.data();

	info.enabledLayerCount = layers.size();
	info.ppEnabledLayerNames = layers.data();

	info.pEnabledFeatures = &features;

	info.queueCreateInfoCount = 1;
	info.pQueueCreateInfos = &queueInfo;

	assertVkResult(vkCreateDevice(physicalDevice, &info, null, &device), SOURCE_LOCATION());

	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

	VkCommandPoolCreateInfo commandPoolInfo { };
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = null;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndex;

	assertVkResult(vkCreateCommandPool(device, &commandPoolInfo, null, &commandPool), SOURCE_LOCATION());

	VkCommandBufferAllocateInfo tempAllocInfo;
	tempAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	tempAllocInfo.pNext = null;
	tempAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	tempAllocInfo.commandPool = commandPool;
	tempAllocInfo.commandBufferCount = 1;

	assertVkResult(vkAllocateCommandBuffers(device, &tempAllocInfo, &tempCommandBuffer), SOURCE_LOCATION());

	semaphoreChain = allocate<VulkanSemaphoreChain>(device, 1000);

	setLayoutCache = allocate<VulkanDescriptorSetLayoutCache>(device);

	auto createAS = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
	auto getASAddress = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(device,
		"vkGetAccelerationStructureDeviceAddressKHR"));
	allocator = allocate<VulkanMemoryAllocator>(device, physicalDevice, queueFamilyIndex, createAS, getASAddress);

	imageFormats = allocate<VulkanImageFormatTable>(physicalDevice);

	vkDeviceWaitIdle(device);

#define VULKAN_LOAD_FUNCTION(function) function = reinterpret_cast<PFN_##function>(vkGetDeviceProcAddr(device, #function))

	VULKAN_LOAD_FUNCTION(vkDestroyAccelerationStructureKHR);
	VULKAN_LOAD_FUNCTION(vkGetAccelerationStructureBuildSizesKHR);
	VULKAN_LOAD_FUNCTION(vkCmdBuildAccelerationStructuresKHR);
}

VulkanDeviceInstance::~VulkanDeviceInstance()
{
	for (auto &freeFunction : freeFunctions)
	{
		freeFunction();
	}
	freeFunctions.clear();

	semaphoreChain = null;
	setLayoutCache = null;
	colorSampler = null;

	vkDestroyCommandPool(device, commandPool, null);
	vkDestroyDevice(device, null);
}

Ref<Swapchain> VulkanDeviceInstance::createSwapchain(Ref<WindowSurface> surface)
{
	auto vulkanSurface = CastDown<VulkanWindowSurface>(surface);
	return allocate<VulkanSwapchain>(physicalDevice, device, queue, vulkanSurface->getSurface(), vulkanSurface->getWindow(), queueFamilyIndex, semaphoreChain);
}

Ref<ImageView2D> VulkanDeviceInstance::createImageView2D(Ref<Image2D> image, u32 baseLevel, u32 levelCount)
{
	return allocate<VulkanImageView2D>(device, image->width, image->height, baseLevel, levelCount, image, image->format);
}

Ref<RenderPass> VulkanDeviceInstance::createRenderpass(const Array<Attachment> &attachments, const Array<Subpass> &subpasses)
{
	return allocate<VulkanRenderPass>(device, attachments, subpasses);
}

Ref<FrameBuffer> VulkanDeviceInstance::createFrameBuffer(u32 width, u32 height, Ref<RenderPass> renderPass, const Array<Ref<ImageView2D>> &imageViews)
{
	return allocate<VulkanFrameBuffer>(device, width, height, renderPass, imageViews);
}

Ref<CommandBuffer> VulkanDeviceInstance::createCommandBuffer()
{
	return allocate<VulkanCommandBuffer>(device, commandPool);
}

Ref<GraphicsPipeline> VulkanDeviceInstance::createGraphicsPipeline(Ref<GraphicsPipelineSource> source, Ref<RenderPass> renderPass, u32 subpass)
{
	return allocate<VulkanGraphicsPipeline>(device, source, renderPass, subpass, setLayoutCache.get());
}

Ref<ComputePipeline> VulkanDeviceInstance::createComputePipeline(Ref<ComputePipelineSource> source)
{
	return allocate<VulkanComputePipeline>(device, source, setLayoutCache.get());
}

Ref<StructBuffer> VulkanDeviceInstance::createStructBuffer(Ref<const StructType> source, const Array<Ref<const StructType>> &structs, BufferLayout layout,
	StructBufferType type, MemoryType memory, u32 unsizedArrayLength)
{
	switch (memory)
	{
		case MemoryType::CPU:
			return allocate<VulkanCPUStructBuffer>(source, structs, layout, type, unsizedArrayLength, device, allocator.get());
		case MemoryType::GPU:
			return null;
	}
	return null;
}

Ref<ArrayBuffer> VulkanDeviceInstance::createArrayBuffer(Ref<const DataType> elementType, u32 elementCount, ArrayBufferType type, MemoryType memory)
{
	switch (memory)
	{
		case MemoryType::CPU:
			return allocate<VulkanCPUArrayBuffer>(elementType, elementCount, type, device, allocator.get());
		case MemoryType::GPU:
			return allocate<VulkanGPUArrayBuffer>(elementType, elementCount, type, device, queue, allocator.get(), semaphoreChain.get(), tempCommandBuffer);
	}
	return null;
}

Ref<DescriptorSet> VulkanDeviceInstance::createDescriptorSet(Ref<const DescriptorSetSource> source)
{
	return allocate<VulkanDescriptorSet>(source, device, setLayoutCache->get(source));
}

Ref<Image2D> VulkanDeviceInstance::createImage2D(u32 width, u32 height, u32 levels, Ref<const ImageFormat> format, ImageUsage usage)
{
	return allocate<VulkanImage2D>(device, queue, semaphoreChain.get(), width, height, levels, format, usage, allocator.get(), tempCommandBuffer);
}

Ref<Sampler2D> VulkanDeviceInstance::createSampler2D(SamplingMode samplingMode, SamplingMode levelSelectionMode, WrapMode xWrappingMode, WrapMode yWrappingMode)
{
	return allocate<VulkanSampler2D>(device, samplingMode, levelSelectionMode, xWrappingMode, yWrappingMode);
}

Ref<ImageFormat> VulkanDeviceInstance::createImageFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint)
{
	if (type == ImageFormatType::COLOR)
	{
		return imageFormats->getFormat(type, componentCount, componentSize, floatingPoint);
	}
	return allocate<VulkanImageFormat>(type, componentCount, componentSize, floatingPoint, physicalDevice);
}

Ref<Fence> VulkanDeviceInstance::createFence()
{
	return allocate<VulkanFence>(device);
}

Ref<BottomLevelAccelerationStructure> VulkanDeviceInstance::createBottomLevelAccelerationStructure()
{
	return allocate<VulkanBottomLevelAccelerationStructure>(this, allocator.get(), tempCommandBuffer);
}

Ref<TopLevelAccelerationStructure> VulkanDeviceInstance::createTopLevelAccelerationStructure(u32 maxInstanceCount)
{
	return allocate<VulkanTopLevelAccelerationStructure>(this, allocator.get(), tempCommandBuffer, maxInstanceCount);
}

void VulkanDeviceInstance::submit(Ref<CommandBuffer> commandBuffer, Ref<Fence> fence)
{
	auto vulkanCommandBuffer = CastDown<VulkanCommandBuffer>(commandBuffer);

	VkSubmitInfo info { };
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.pNext = null;

	Array<VkSemaphore> waitSemaphores = semaphoreChain->pollWaitSemaphores();
	info.waitSemaphoreCount = waitSemaphores.size();
	info.pWaitSemaphores = waitSemaphores.data();

	Array<VkPipelineStageFlags> waitStages(waitSemaphores.size());
	for (u32 i = 0; i < waitSemaphores.size(); i++)
	{
		waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	}
	info.pWaitDstStageMask = waitStages.data();

	info.commandBufferCount = 1;
	info.pCommandBuffers = vulkanCommandBuffer->getCommandBufferPointer();

	VkSemaphore signalSemaphore = semaphoreChain->getSemaphore();
	semaphoreChain->addWaitSemaphore(signalSemaphore);
	info.signalSemaphoreCount = 1;
	info.pSignalSemaphores = &signalSemaphore;

	VkFence vkFence = VK_NULL_HANDLE;
	if (fence)
	{
		vkFence = CastDown<VulkanFence>(fence)->getFence();
	}

	assertVkResult(vkQueueSubmit(queue, 1, &info, vkFence), SOURCE_LOCATION());
}

void VulkanDeviceInstance::wait()
{
	vkDeviceWaitIdle(device);
}

void VulkanDeviceInstance::wait(Array<Ref<Fence>> fences)
{
	Array<VkFence> vkFences(fences.size());
	for (u32 i = 0; i < fences.size(); i++)
	{
		vkFences[i] = CastDown<VulkanFence>(fences[i])->getFence();
	}

	assertVkResult(vkWaitForFences(device, vkFences.size(), vkFences.data(), VK_TRUE, Time::seconds(10.0).nanoSeconds()), SOURCE_LOCATION());
	assertVkResult(vkResetFences(device, vkFences.size(), vkFences.data()), SOURCE_LOCATION());
}

VulkanDevice::VulkanDevice(VkPhysicalDevice device) :
	device(device)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	name = properties.deviceName;

	switch (properties.deviceType)
	{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			type = DeviceType::DISCRETE;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			type = DeviceType::INTEGRATED;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			type = DeviceType::VIRTUAL;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			type = DeviceType::CPU;
			break;
		default:
			type = DeviceType::OTHER;
	}

	VkPhysicalDeviceFeatures f;
	vkGetPhysicalDeviceFeatures(device, &f);

	u32 extensionCount;
	vkEnumerateDeviceExtensionProperties(device, null, &extensionCount, null);

	Array<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, null, &extensionCount, extensions.data());

	auto isExtensionSupported = [&extensions](const char *name) -> bool
	{
		for (const auto &extension : extensions)
		{
			if (strcmp(name, extension.extensionName) == 0)
			{
				return true;
			}
		}
		return false;
	};

	out(name + ":");
	for (const auto &extension : extensions)
	{
		out(String("\t") + extension.extensionName);
	}

	features.swapchain = isExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	features.raytracing = isExtensionSupported(
	VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
}

VulkanDevice::~VulkanDevice()
{
}

Ref<DeviceInstance> VulkanDevice::createInstance()
{
	return allocate<VulkanDeviceInstance>(device);
}

const String& VulkanDevice::getName() const
{
	return name;
}

DeviceType VulkanDevice::getType() const
{
	return type;
}

DeviceFeatures VulkanDevice::getFeatures() const
{
	return features;
}

}

}
