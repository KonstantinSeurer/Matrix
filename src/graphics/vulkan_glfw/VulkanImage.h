/*
 * VulkanImage.h
 *
 *  Created on: Jun 12, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANIMAGE_H_
#define GRAPHICS_VULKAN_GLFW_VULKANIMAGE_H_

#include "../Image.h"
#include <vulkan/vulkan.h>
#include "VulkanMemoryAllocator.h"
#include "VulkanSemaphoreChain.h"

namespace matrix
{

namespace graphics
{

class VulkanSampler2D: public Sampler2D
{
private:
	VkDevice device;
	VkSampler sampler;
public:
	VulkanSampler2D(VkDevice device, SamplingMode samplingMode, SamplingMode levelSelectionMode, WrapMode xWrappingMode,
			WrapMode yWrappingMode);
	virtual ~VulkanSampler2D();

	VkSampler getSampler() const
	{
		return sampler;
	}
};

class VulkanImageFormat: public ImageFormat
{
private:
	VkFormat format;
public:
	VulkanImageFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint, VkFormat format);

	VulkanImageFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint,
			VkPhysicalDevice physicalDevice);

	VkFormat getFormat() const
	{
		return format;
	}
};

struct ImageFormatDescription
{
public:
	ImageFormatType type;
	u8 componentCount;
	u8 componentSize;
	bool floatingPoint;
public:
	ImageFormatDescription() :
			type(ImageFormatType::COLOR), componentCount(0), componentSize(0), floatingPoint(false)
	{
	}

	ImageFormatDescription(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint) :
			type(type), componentCount(componentCount), componentSize(componentSize), floatingPoint(floatingPoint)
	{
	}

	bool operator==(ImageFormatDescription desc) const
	{
		return type == desc.type && componentCount == desc.componentCount && componentSize == desc.componentSize
				&& floatingPoint == desc.floatingPoint;
	}
};

class ImageFormatDescriptionHash
{
public:
	size_t operator()(ImageFormatDescription desc) const
	{
		return desc.componentCount | (desc.componentSize << 8);
	}
};

class VulkanImageFormatTable
{
private:
	UnorderedMap<ImageFormatDescription, Ref<ImageFormat>, ImageFormatDescriptionHash> imageFormats;
private:
	void addFormats(VkPhysicalDevice device, u8 componentSize, bool floatingPoint);
public:
	VulkanImageFormatTable(VkPhysicalDevice device);

	Ref<ImageFormat> getFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint);
};

class VulkanImage2D: public Image2D
{
	VkDevice device;
	VkQueue queue;
	VulkanSemaphoreChain *semaphoreChain;

	VkImage image;
	VkDeviceMemory memory;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	void *stagingData;

	VkCommandBuffer accessCommandBuffer;

	bool ownsImage;
private:
public:
	VulkanImage2D(VkDevice device, VkQueue queue, VulkanSemaphoreChain *semaphoreChain, u32 width, u32 height,
			VkImage image, Ref<const ImageFormat> format, ImageUsage usage, VkCommandBuffer accessCommandBuffer);
	VulkanImage2D(VkDevice device, VkQueue queue, VulkanSemaphoreChain *semaphoreChain, u32 width, u32 height,
			u32 levels, Ref<const ImageFormat> format, ImageUsage usage, VulkanMemoryAllocator *allocator,
			VkCommandBuffer accessCommandBuffer);
	virtual ~VulkanImage2D();

	virtual void access(Function<void(Image2DAccessor)> accessCallback, ImageLayout targetLayout);

	VkImage getImage() const
	{
		return image;
	}
};

class VulkanImageView2D: public ImageView2D
{
private:
	VkDevice device;
	VkImageView view;
	ImageFormatType type;
public:
	VulkanImageView2D(VkDevice device, u32 width, u32 height, u32 baseLevel, u32 levelCount, Ref<Image2D> image,
			Ref<const ImageFormat> format);
	virtual ~VulkanImageView2D();

	VkImage getImage() const
	{
		return CastDown<VulkanImage2D>(image)->getImage();
	}

	VkImageView getView() const
	{
		return view;
	}

	ImageFormatType getType() const {
		return type;
	}
};

}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANIMAGE_H_ */
