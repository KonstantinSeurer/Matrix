/*
 * VulkanRenderpass.cpp
 *
 *  Created on: Jun 12, 2021
 *      Author: konstantin
 */

#include "VulkanUtil.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"

namespace matrix {

namespace graphics {

static UnorderedMap<LoadOperation, VkAttachmentLoadOp> loadOperations = {
		{ LoadOperation::CLEAR, VK_ATTACHMENT_LOAD_OP_CLEAR }, { LoadOperation::DONT_CARE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE }, { LoadOperation::LOAD, VK_ATTACHMENT_LOAD_OP_LOAD } };

static UnorderedMap<StoreOperation, VkAttachmentStoreOp> storeOperations = { { StoreOperation::DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE }, { StoreOperation::STORE, VK_ATTACHMENT_STORE_OP_STORE } };

static UnorderedMap<ImageLayout, VkImageLayout> imageLayouts = { { ImageLayout::COLOR_ATTACHMENT,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }, { ImageLayout::DEPTH_ATTACHMENT,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }, { ImageLayout::DEPTH_STENCIL_ATTACHMENT,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }, { ImageLayout::PRESENT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }, {
		ImageLayout::SHADER_READ_ONLY, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }, { ImageLayout::SHADER_STORAGE,
		VK_IMAGE_LAYOUT_GENERAL },
		{ ImageLayout::STENCIL_ATTACHMENT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }, {
				ImageLayout::TRANSFER_DESTINATION, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL }, {
				ImageLayout::TRANSFER_SOURCE, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL }, { ImageLayout::UNDEFINED,
				VK_IMAGE_LAYOUT_UNDEFINED } };

static VkPipelineBindPoint getVkPipelineBindPoint(PipelineType type) {
	switch (type) {
	case PipelineType::GRAPHICS:
		return VK_PIPELINE_BIND_POINT_GRAPHICS;
	case PipelineType::COMPUTE:
		return VK_PIPELINE_BIND_POINT_COMPUTE;
	}

	err("Unsupported pipeline type " + toString((u32) type) + "!");
	return VK_PIPELINE_BIND_POINT_GRAPHICS;
}

VulkanRenderPass::VulkanRenderPass(VkDevice device, const Array<Attachment> &attachments,
		const Array<Subpass> &subpasses) :
		device(device) {
	this->attachments = attachments;
	this->subpasses = subpasses;

	Array<VkAttachmentDescription> attachmentDescriptions(attachments.size());
	for (u32 i = 0; i < attachments.size(); i++) {
		const auto &attachment = attachments[i];

		VkAttachmentDescription description { };
		description.format = CastDown<const VulkanImageFormat>(attachment.format)->getFormat();
		description.samples = (VkSampleCountFlagBits) attachment.sampleCount;
		description.loadOp = loadOperations[attachment.loadOperation];
		description.storeOp = storeOperations[attachment.storeOperation];
		description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		description.initialLayout = imageLayouts[attachment.beginLayout];
		description.finalLayout = imageLayouts[attachment.endLayout];

		attachmentDescriptions[i] = description;
	}

	Array<VkSubpassDescription> subpassDescriptions(subpasses.size());
	Array<Array<VkAttachmentReference>> attachmentReferences(subpasses.size());
	Array<VkAttachmentReference> depthAttachmentReferences;
	for (u32 i = 0; i < subpasses.size(); i++) {
		const auto &subpass = subpasses[i];

		attachmentReferences[i].resize(subpass.colorAttachments.size());
		for (u32 j = 0; j < subpass.colorAttachments.size(); j++) {
			const auto &reference = subpass.colorAttachments[j];

			attachmentReferences[i][j].attachment = reference.index;
			attachmentReferences[i][j].layout = imageLayouts[reference.layout];
		}

		VkSubpassDescription description { };
		description.pipelineBindPoint = getVkPipelineBindPoint(subpass.pipelineType);
		description.colorAttachmentCount = attachmentReferences[i].size();
		description.pColorAttachments = attachmentReferences[i].data();
		if (subpass.depthStencilAttachment) {
			auto &depthAttachment = depthAttachmentReferences.emplace_back();
			depthAttachment.attachment = subpass.depthStencilAttachment->index;
			depthAttachment.layout = imageLayouts[subpass.depthStencilAttachment->layout];
			description.pDepthStencilAttachment = &depthAttachmentReferences[depthAttachmentReferences.size() - 1];
		}
		subpassDescriptions[i] = description;
	}

	VkRenderPassCreateInfo createInfo { };
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.pNext = null;
	createInfo.flags = 0;
	createInfo.attachmentCount = attachmentDescriptions.size();
	createInfo.pAttachments = attachmentDescriptions.data();
	createInfo.subpassCount = subpassDescriptions.size();
	createInfo.pSubpasses = subpassDescriptions.data();

	assertVkResult(vkCreateRenderPass(device, &createInfo, null, &renderPass), SOURCE_LOCATION());
}

VulkanRenderPass::~VulkanRenderPass() {
	vkDestroyRenderPass(device, renderPass, null);

}

}

}
