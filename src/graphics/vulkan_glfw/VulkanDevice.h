/*
 * VulkanGraphicsContext.h
 *
 *  Created on: Jun 3, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANDEVICE_H_
#define GRAPHICS_VULKAN_GLFW_VULKANDEVICE_H_

#include <vulkan/vulkan.h>
#include <graphics/Device.h>
#include "VulkanSemaphoreChain.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanImage.h"

namespace matrix
{

	namespace graphics
	{

		class VulkanDeviceInstance : public DeviceInstance
		{
		private:
			VkDevice device;
			VkQueue queue;
			VkPhysicalDevice physicalDevice;
			VkCommandPool commandPool;
			u32 queueFamilyIndex;
			Ref<VulkanSemaphoreChain> semaphoreChain;
			Ref<VulkanDescriptorSetLayoutCache> setLayoutCache;
			Ref<VulkanMemoryAllocator> allocator;
			VkCommandBuffer tempCommandBuffer;
			Ref<VulkanImageFormatTable> imageFormats;

		public:
#define VULKAN_DECLARE_FUNCTION(function) PFN_##function function

			VULKAN_DECLARE_FUNCTION(vkDestroyAccelerationStructureKHR);
			VULKAN_DECLARE_FUNCTION(vkGetAccelerationStructureBuildSizesKHR);
			VULKAN_DECLARE_FUNCTION(vkCmdBuildAccelerationStructuresKHR);

		public:
			VulkanDeviceInstance(VkPhysicalDevice physicalDevice);

			virtual ~VulkanDeviceInstance();

			VkDevice getDevice() const
			{
				return device;
			}

			VkQueue getQueue() const
			{
				return queue;
			}

			VulkanSemaphoreChain *getSemaphoreChain()
			{
				return semaphoreChain.get();
			}

			virtual Ref<Swapchain> createSwapchain(Ref<WindowSurface> surface);

			virtual Ref<ImageView2D> createImageView2D(Ref<Image2D> image, u32 baseLevel, u32 levelCount);

			virtual Ref<RenderPass> createRenderpass(const Array<Attachment> &attachments, const Array<Subpass> &subpasses);

			virtual Ref<FrameBuffer> createFrameBuffer(u32 width, u32 height, Ref<RenderPass> renderPass, const Array<Ref<ImageView2D>> &imageViews);

			virtual Ref<CommandBuffer> createCommandBuffer();

			virtual Ref<GraphicsPipeline> createGraphicsPipeline(Ref<GraphicsPipelineSource> source, Ref<RenderPass> renderPass, u32 subpass);

			virtual Ref<ComputePipeline> createComputePipeline(Ref<ComputePipelineSource> source);

			virtual void submit(Ref<CommandBuffer> commandBuffer, Ref<Fence> fence);

			virtual Ref<StructBuffer> createStructBuffer(Ref<const StructType> source, const Array<Ref<const StructType>> &structs, BufferLayout layout,
														 StructBufferType type, MemoryType memory, u32 unsizedArrayLength = 0);

			virtual Ref<ArrayBuffer> createArrayBuffer(Ref<const DataType> elementType, u32 elementCount, ArrayBufferType type, MemoryType memory);

			virtual Ref<DescriptorSet> createDescriptorSet(Ref<const DescriptorSetSource> source);

			virtual Ref<Image2D> createImage2D(u32 width, u32 height, u32 levels, Ref<const ImageFormat> format, ImageUsage usage);

			virtual Ref<Sampler2D> createSampler2D(SamplingMode samplingMode, SamplingMode levelSelectionMode, WrapMode xWrappingMode, WrapMode yWrappingMode);

			virtual Ref<Image3D> createImage3D(u32 width, u32 height, u32 length, u32 levels, Ref<const ImageFormat> format, ImageUsage usage);

			virtual Ref<ImageView3D> createImageView3D(Ref<Image3D> image, u32 baseLevel, u32 levelCount);

			virtual Ref<Sampler3D> createSampler3D(SamplingMode samplingMode, SamplingMode levelSelectionMode, WrapMode xWrappingMode, WrapMode yWrappingMode,
												   WrapMode zWrappingMode);

			Ref<ImageFormat> createImageFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint);

			virtual Ref<Fence> createFence();

			virtual Ref<BottomLevelAccelerationStructure> createBottomLevelAccelerationStructure();

			virtual Ref<TopLevelAccelerationStructure> createTopLevelAccelerationStructure(u32 maxInstanceCount);

			virtual void wait();
			virtual void wait(Array<Ref<Fence>> fences);
		};

		class VulkanDevice : public Device
		{
		private:
			VkPhysicalDevice device;
			String name;
			DeviceType type;
			DeviceFeatures features;

		public:
			VulkanDevice(VkPhysicalDevice device);

			virtual ~VulkanDevice();

			virtual Ref<DeviceInstance> createInstance();

			virtual const String &getName() const;

			virtual DeviceType getType() const;

			virtual DeviceFeatures getFeatures() const;
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANDEVICE_H_ */
