/*
 * VulkanFrameBuffer.h
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANFRAMEBUFFER_H_
#define GRAPHICS_VULKAN_GLFW_VULKANFRAMEBUFFER_H_

#include <graphics/FrameBuffer.h>
#include "VulkanRenderPass.h"

namespace matrix
{

	namespace graphics
	{

		class VulkanFrameBuffer : public FrameBuffer
		{
		private:
			VkDevice device;
			VkFramebuffer frameBuffer;

		public:
			VulkanFrameBuffer(VkDevice device, u32 width, u32 height, Ref<RenderPass> renderPass,
							  const Array<Ref<ImageView2D>> &attachments);

			virtual ~VulkanFrameBuffer();

			VkFramebuffer getFrameBuffer() const
			{
				return frameBuffer;
			}
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANFRAMEBUFFER_H_ */
