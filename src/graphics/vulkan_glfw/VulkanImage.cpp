/*
 * VulkanImage.cpp
 *
 *  Created on: Jun 12, 2021
 *      Author: konstantin
 */

#include "VulkanImage.h"
#include "VulkanUtil.h"

namespace matrix
{

	namespace graphics
	{

		static UnorderedMap<WrapMode, VkSamplerAddressMode> addressModes = {{WrapMode::CLAMP, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE}, {WrapMode::MIRRORED_REPEAT, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT}, {WrapMode::REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT}};

		static UnorderedMap<SamplingMode, VkSamplerMipmapMode> mipmapModes = {{SamplingMode::LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR}, {SamplingMode::NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST}};

		VulkanSampler::VulkanSampler(VkDevice device, SamplingMode samplingMode, SamplingMode levelSelectionMode, WrapMode xWrappingMode, WrapMode yWrappingMode,
									 WrapMode zWrappingMode) : device(device)
		{
			VkFilter samplingFilter = getVkFilter(samplingMode);

			VkSamplerCreateInfo createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			createInfo.pNext = null;
			createInfo.flags = 0;

			createInfo.addressModeU = addressModes[xWrappingMode];
			createInfo.addressModeV = addressModes[yWrappingMode];
			createInfo.addressModeW = addressModes[zWrappingMode];

			createInfo.anisotropyEnable = VK_FALSE;
			createInfo.maxAnisotropy = 1.0f;

			createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

			createInfo.compareEnable = VK_FALSE;
			createInfo.compareOp = VK_COMPARE_OP_ALWAYS;

			createInfo.magFilter = samplingFilter;
			createInfo.minFilter = samplingFilter;
			createInfo.maxLod = 0.0f;
			createInfo.minLod = 0.0f;
			createInfo.mipLodBias = 0.0f;
			createInfo.mipmapMode = mipmapModes[levelSelectionMode];

			createInfo.unnormalizedCoordinates = false;

			assertVkResult(vkCreateSampler(device, &createInfo, null, &sampler), SOURCE_LOCATION());
		}

		VulkanSampler::~VulkanSampler()
		{
			vkDestroySampler(device, sampler, null);
		}

		VulkanImageFormat::VulkanImageFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint, VkFormat format)
		{
			this->type = type;
			this->componentCount = componentCount;
			this->componentSize = componentSize;
			this->floatingPoint = floatingPoint;
			this->format = format;
		}

		VulkanImageFormat::VulkanImageFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint, VkPhysicalDevice physicalDevice)
		{
			this->type = type;
			this->componentCount = componentCount;
			this->componentSize = componentSize;
			this->floatingPoint = floatingPoint;
			format = getVkFormat(physicalDevice, *this);
		}

		static bool isFormatSupported(VkPhysicalDevice device, ImageFormatDescription desc)
		{
			ImageFormat format;
			format.type = desc.type;
			format.componentCount = desc.componentCount;
			format.componentSize = desc.componentSize;
			format.floatingPoint = desc.floatingPoint;

			VkImageFormatProperties properties;
			return vkGetPhysicalDeviceImageFormatProperties(device, getVkFormat(device, format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT,
															0, &properties) != VK_ERROR_FORMAT_NOT_SUPPORTED;
		}

		VulkanImageFormatTable::VulkanImageFormatTable(VkPhysicalDevice device)
		{
			addFormats(device, 1, false);
			addFormats(device, 2, false);

			addFormats(device, 2, true);
			addFormats(device, 4, true);
		}

		void VulkanImageFormatTable::addFormats(VkPhysicalDevice device, u8 componentSize, bool floatingPoint)
		{
			for (u8 componentCount = 1; componentCount <= 4; componentCount++)
			{
				ImageFormatDescription format(ImageFormatType::COLOR, componentCount, componentSize, floatingPoint);
				for (u8 testComponentCount = componentCount; testComponentCount <= 4; testComponentCount++)
				{
					ImageFormatDescription testFormat(ImageFormatType::COLOR, testComponentCount, componentSize, floatingPoint);
					if (isFormatSupported(device, testFormat))
					{
						imageFormats[format] = allocate<VulkanImageFormat>(ImageFormatType::COLOR, testComponentCount, componentSize, floatingPoint, device);
					}
				}
			}
		}

		Ref<ImageFormat> VulkanImageFormatTable::getFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint)
		{
			ImageFormatDescription format;
			format.type = type;
			format.componentCount = componentCount;
			format.componentSize = componentSize;
			format.floatingPoint = floatingPoint;
			return imageFormats.at(format);
		}

		// TODO: put duplicate code into a VulkanImage base class

		VulkanImage2D::VulkanImage2D(VkDevice device, VkQueue queue, VulkanSemaphoreChain *semaphoreChain, u32 width, u32 height, VkImage image,
									 Ref<const ImageFormat> format, ImageUsage usage, VkCommandBuffer accessCommandBuffer) : Image2D(usage, format, width, height), device(device), queue(queue), semaphoreChain(semaphoreChain), image(image), accessCommandBuffer(accessCommandBuffer), ownsImage(false)
		{
			memory = VK_NULL_HANDLE;

			if (usage & ImageUsage::ACCESS)
			{
				stagingBuffer = VK_NULL_HANDLE;
				stagingMemory = VK_NULL_HANDLE;
				stagingData = null;
			}
			else
			{
				stagingBuffer = VK_NULL_HANDLE;
				stagingMemory = VK_NULL_HANDLE;
				stagingData = null;
			}
		}

		VulkanImage2D::VulkanImage2D(VkDevice device, VkQueue queue, VulkanSemaphoreChain *semaphoreChain, u32 width, u32 height, u32 levels,
									 Ref<const ImageFormat> format, ImageUsage usage, VulkanMemoryAllocator *allocator, VkCommandBuffer accessCommandBuffer) : Image2D(usage, format, width, height), device(device), queue(queue), semaphoreChain(semaphoreChain), accessCommandBuffer(accessCommandBuffer), ownsImage(true)
		{
			VkFormat vkFormat = CastDown<const VulkanImageFormat>(format)->getFormat();
			auto imageMemory = allocator->createImage2D(width, height, levels, vkFormat, usage);
			image = imageMemory.first;
			memory = imageMemory.second;

			if (usage & ImageUsage::ACCESS)
			{
				u32 size = format->componentSize * format->componentCount * width * height;
				auto stagingBufferMemory = allocator->createBuffer(size, MemoryType::CPU, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 0);

				stagingBuffer = stagingBufferMemory.first;
				stagingMemory = stagingBufferMemory.second;

				assertVkResult(vkMapMemory(device, stagingMemory, 0, size, 0, &stagingData), SOURCE_LOCATION());
			}
			else
			{
				stagingBuffer = VK_NULL_HANDLE;
				stagingMemory = VK_NULL_HANDLE;
				stagingData = null;
			}
		}

		VulkanImage2D::~VulkanImage2D()
		{
			if (ownsImage)
			{
				vkDestroyImage(device, image, null);
				if (memory)
				{
					vkFreeMemory(device, memory, null);
				}
			}

			if (stagingBuffer)
			{
				vkDestroyBuffer(device, stagingBuffer, null);
			}
			if (stagingMemory)
			{
				vkFreeMemory(device, stagingMemory, null);
			}
		}

		void VulkanImage2D::access(Function<void(Image2DData)> accessCallback, ImageLayout targetLayout)
		{
			Image2DData accessor(stagingData, width, height, format, false);
			accessCallback(accessor);

			VkCommandBufferBeginInfo beginInfo;
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = null;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			beginInfo.pInheritanceInfo = null;

			assertVkResult(vkBeginCommandBuffer(accessCommandBuffer, &beginInfo), SOURCE_LOCATION());

			pipelineBarrier(accessCommandBuffer, image, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DESTINATION, 0, 1, format->type);

			VkBufferImageCopy region;
			region.bufferImageHeight = 0;
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.imageExtent = {width, height, 1};
			region.imageOffset = {0, 0, 0};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageSubresource.mipLevel = 0;

			vkCmdCopyBufferToImage(accessCommandBuffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			pipelineBarrier(accessCommandBuffer, image, ImageLayout::TRANSFER_DESTINATION, ImageLayout::SHADER_READ_ONLY, 0, 1, format->type);

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
			for (u32 i = 0; i < waitSemaphores.size(); i++)
			{
				waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			}
			submit.pWaitDstStageMask = waitStages.data();

			VkSemaphore signalSemaphore = semaphoreChain->getSemaphore();
			semaphoreChain->addWaitSemaphore(signalSemaphore);
			submit.signalSemaphoreCount = 1;
			submit.pSignalSemaphores = &signalSemaphore;

			assertVkResult(vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE), SOURCE_LOCATION());

			vkDeviceWaitIdle(device); // TODO: remove
		}

		VulkanImage3D::VulkanImage3D(VkDevice device, VkQueue queue, VulkanSemaphoreChain *semaphoreChain, u32 width, u32 height, u32 length, VkImage image,
									 Ref<const ImageFormat> format, ImageUsage usage, VkCommandBuffer accessCommandBuffer) : Image3D(usage, format, width, height, length), device(device), queue(queue), semaphoreChain(semaphoreChain), image(image), accessCommandBuffer(accessCommandBuffer), ownsImage(false)
		{
			memory = VK_NULL_HANDLE;

			if (usage & ImageUsage::ACCESS)
			{
				stagingBuffer = VK_NULL_HANDLE;
				stagingMemory = VK_NULL_HANDLE;
				stagingData = null;
			}
			else
			{
				stagingBuffer = VK_NULL_HANDLE;
				stagingMemory = VK_NULL_HANDLE;
				stagingData = null;
			}
		}

		VulkanImage3D::VulkanImage3D(VkDevice device, VkQueue queue, VulkanSemaphoreChain *semaphoreChain, u32 width, u32 height, u32 length, u32 levels,
									 Ref<const ImageFormat> format, ImageUsage usage, VulkanMemoryAllocator *allocator, VkCommandBuffer accessCommandBuffer) : Image3D(usage, format, width, height, length), device(device), queue(queue), semaphoreChain(semaphoreChain), accessCommandBuffer(accessCommandBuffer), ownsImage(true)
		{
			VkFormat vkFormat = CastDown<const VulkanImageFormat>(format)->getFormat();
			auto imageMemory = allocator->createImage3D(width, height, length, levels, vkFormat, usage);
			image = imageMemory.first;
			memory = imageMemory.second;

			if (usage & ImageUsage::ACCESS)
			{
				u32 size = format->componentSize * format->componentCount * width * height * length;
				auto stagingBufferMemory = allocator->createBuffer(size, MemoryType::CPU, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 0);

				stagingBuffer = stagingBufferMemory.first;
				stagingMemory = stagingBufferMemory.second;

				assertVkResult(vkMapMemory(device, stagingMemory, 0, size, 0, &stagingData), SOURCE_LOCATION());
			}
			else
			{
				stagingBuffer = VK_NULL_HANDLE;
				stagingMemory = VK_NULL_HANDLE;
				stagingData = null;
			}
		}

		VulkanImage3D::~VulkanImage3D()
		{
			if (ownsImage)
			{
				vkDestroyImage(device, image, null);
				if (memory)
				{
					vkFreeMemory(device, memory, null);
				}
			}

			if (stagingBuffer)
			{
				vkDestroyBuffer(device, stagingBuffer, null);
			}
			if (stagingMemory)
			{
				vkFreeMemory(device, stagingMemory, null);
			}
		}

		void VulkanImage3D::access(Function<void(Image3DData)> accessCallback, ImageLayout targetLayout)
		{
			Image3DData accessor(stagingData, width, height, length, format, false);
			accessCallback(accessor);

			VkCommandBufferBeginInfo beginInfo;
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = null;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			beginInfo.pInheritanceInfo = null;

			assertVkResult(vkBeginCommandBuffer(accessCommandBuffer, &beginInfo), SOURCE_LOCATION());

			pipelineBarrier(accessCommandBuffer, image, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DESTINATION, 0, 1, format->type);

			VkBufferImageCopy region;
			region.bufferImageHeight = 0;
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.imageExtent = {width, height, length};
			region.imageOffset = {0, 0, 0};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageSubresource.mipLevel = 0;

			vkCmdCopyBufferToImage(accessCommandBuffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			pipelineBarrier(accessCommandBuffer, image, ImageLayout::TRANSFER_DESTINATION, ImageLayout::SHADER_READ_ONLY, 0, 1, format->type);

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
			for (u32 i = 0; i < waitSemaphores.size(); i++)
			{
				waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			}
			submit.pWaitDstStageMask = waitStages.data();

			VkSemaphore signalSemaphore = semaphoreChain->getSemaphore();
			semaphoreChain->addWaitSemaphore(signalSemaphore);
			submit.signalSemaphoreCount = 1;
			submit.pSignalSemaphores = &signalSemaphore;

			assertVkResult(vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE), SOURCE_LOCATION());

			vkDeviceWaitIdle(device); // TODO: remove
		}

		static UnorderedMap<ImageFormatType, VkImageAspectFlags> aspectMasks = {{ImageFormatType::COLOR, VK_IMAGE_ASPECT_COLOR_BIT}, {ImageFormatType::DEPTH, VK_IMAGE_ASPECT_DEPTH_BIT}, {ImageFormatType::DEPTH_STENCIL, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT}, {ImageFormatType::STENCIL, VK_IMAGE_ASPECT_STENCIL_BIT}};

		VulkanImageView2D::VulkanImageView2D(VkDevice device, u32 width, u32 height, u32 baseLevel, u32 levelCount, Ref<Image2D> image, Ref<const ImageFormat> format) : ImageView2D(width, height, baseLevel, levelCount, image), device(device), type(format->type)
		{
			VkImageViewCreateInfo createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = null;
			createInfo.flags = 0;
			createInfo.image = CastDown<VulkanImage2D>(image)->getImage();
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = CastDown<const VulkanImageFormat>(format)->getFormat();
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = aspectMasks[format->type];
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.baseMipLevel = baseLevel;
			createInfo.subresourceRange.layerCount = 1;
			createInfo.subresourceRange.levelCount = levelCount;

			assertVkResult(vkCreateImageView(device, &createInfo, null, &view), SOURCE_LOCATION());
		}

		VulkanImageView2D::~VulkanImageView2D()
		{
			vkDestroyImageView(device, view, null);
		}

		VulkanImageView3D::VulkanImageView3D(VkDevice device, u32 width, u32 height, u32 length, u32 baseLevel, u32 levelCount, Ref<Image3D> image,
											 Ref<const ImageFormat> format) : ImageView3D(width, height, length, baseLevel, levelCount, image), device(device), type(format->type)
		{
			VkImageViewCreateInfo createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = null;
			createInfo.flags = 0;
			createInfo.image = CastDown<VulkanImage3D>(image)->getImage();
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
			createInfo.format = CastDown<const VulkanImageFormat>(format)->getFormat();
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = aspectMasks[format->type];
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.baseMipLevel = baseLevel;
			createInfo.subresourceRange.layerCount = 1;
			createInfo.subresourceRange.levelCount = levelCount;

			assertVkResult(vkCreateImageView(device, &createInfo, null, &view), SOURCE_LOCATION());
		}

		VulkanImageView3D::~VulkanImageView3D()
		{
			vkDestroyImageView(device, view, null);
		}

	}

}
