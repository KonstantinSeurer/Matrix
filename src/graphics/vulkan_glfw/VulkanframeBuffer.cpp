/*
 * VulkanframeBuffer.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#include "VulkanFrameBuffer.h"
#include "VulkanUtil.h"
#include "VulkanImage.h"

namespace matrix {

namespace graphics {

VulkanFrameBuffer::VulkanFrameBuffer(VkDevice device, u32 width, u32 height, Ref<RenderPass> renderPass,
		const Array<Ref<ImageView2D>> &attachments) :
		device(device) {
	this->width = width;
	this->height = height;

	Array<VkImageView> imageViews(attachments.size());
	for (u32 i = 0; i < attachments.size(); i++) {
		imageViews[i] = CastDown<VulkanImageView2D>(attachments[i])->getView();
	}

	VkFramebufferCreateInfo createInfo { };
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = null;
	createInfo.flags = 0;
	createInfo.renderPass = CastDown<VulkanRenderPass>(renderPass)->getRenderPass();
	createInfo.attachmentCount = imageViews.size();
	createInfo.pAttachments = imageViews.data();
	createInfo.width = width;
	createInfo.height = height;
	createInfo.layers = 1;

	assertVkResult(vkCreateFramebuffer(device, &createInfo, null, &frameBuffer), SOURCE_LOCATION());
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
	vkDestroyFramebuffer(device, frameBuffer, null);
}

}

}
