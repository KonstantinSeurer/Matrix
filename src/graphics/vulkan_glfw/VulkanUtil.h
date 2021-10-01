/*
 * VulkanUtil.h
 *
 *  Created on: Jun 3, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANUTIL_H_
#define GRAPHICS_VULKAN_GLFW_VULKANUTIL_H_

#include "vulkan/vulkan.h"
#include <iostream>

#include <Base.h>
#include <Logging.h>
#include <graphics/Image.h>
#include <graphics/Pipeline.h>
#include <graphics/Buffer.h>

namespace matrix
{

	namespace graphics
	{

		void assertVkResult(VkResult result, const SourceLocation &location);

		VkFormat getVkFormat(VkPhysicalDevice physicalDevice, ImageFormat format);

		VkFormat getVkFormat(PrimitiveTypeType type);

		VkDescriptorType getVkDescriptorType(DescriptorType type);

		VkShaderModule compileShader(VkDevice device, const PipelineSource &source, const ShaderSource &shader, ShaderType stage);

		VkShaderStageFlagBits getShaderStage(ShaderType type);

		u32 getPrimitiveTypeSize(PrimitiveTypeType type);

		Ref<StructAccessorBlueprint> createStructAccessorBlueprint(Ref<const StructType> s, const Array<Ref<const StructType>> &structs, BufferLayout layout, u32 unsizedArrayLength = 0);

		Ref<AccessorBlueprint> createArrayBufferElementBlueprint(Ref<const DataType> type);

		VkImageLayout getVkImageLayout(ImageLayout layout, ImageFormatType imageType);

		void pipelineBarrier(VkCommandBuffer cmd, VkImage image, ImageLayout oldLayout, ImageLayout newLayout, u32 baseLevel, u32 levelCount,
							 ImageFormatType imageType);

		VkFilter getVkFilter(SamplingMode samplingMode);

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANUTIL_H_ */
