/*
 * VulkanCommandBuffer.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#include "VulkanCommandBuffer.h"
#include "VulkanUtil.h"
#include "VulkanRenderPass.h"
#include "VulkanFrameBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"
#include "VulkanPipeline.h"

namespace matrix {

namespace graphics {

VulkanCommandBuffer::VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool) :
	width(0), height(0), currentGraphicsPipeline(null), currentComputePipeline(null), currentPipeline(null) {
	VkCommandBufferAllocateInfo info { };
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = null;
	info.commandPool = commandPool;
	info.commandBufferCount = 1;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	assertVkResult(vkAllocateCommandBuffers(device, &info, &commandBuffer), SOURCE_LOCATION());
}

VulkanCommandBuffer::~VulkanCommandBuffer() {

}

VkPipelineBindPoint VulkanCommandBuffer::getCurrentBindPoint() const {
	if (currentGraphicsPipeline) {
		return VK_PIPELINE_BIND_POINT_GRAPHICS;
	}
	if (currentComputePipeline) {
		return VK_PIPELINE_BIND_POINT_COMPUTE;
	}
	err("There is no pipeline bound!");
	return VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void VulkanCommandBuffer::record(Function<void()> recordCallback) {
	VkCommandBufferBeginInfo info { };
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = null;
	info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	info.pInheritanceInfo = null;

	assertVkResult(vkBeginCommandBuffer(commandBuffer, &info), SOURCE_LOCATION());

	recordCallback();

	assertVkResult(vkEndCommandBuffer(commandBuffer), SOURCE_LOCATION());
}

void VulkanCommandBuffer::renderPass(Ref<const RenderPass> renderPass, Ref<const FrameBuffer> frameBuffer, const ClearData &clearData,
	Function<void()> renderPassBody) {
	VkRenderPassBeginInfo info;
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.pNext = null;
	info.renderPass = CastDown<const VulkanRenderPass>(renderPass)->getRenderPass();
	info.framebuffer = CastDown<const VulkanFrameBuffer>(frameBuffer)->getFrameBuffer();
	info.renderArea.offset = { 0, 0 };
	info.renderArea.extent = { frameBuffer->getWidth(), frameBuffer->getHeight() };

	Array<VkClearValue> clearValues;
	for (const auto &attachment : renderPass->getAttachments()) {
		VkClearValue value;
		if (attachment.loadOperation == LoadOperation::CLEAR) {
			if (attachment.format->type == ImageFormatType::COLOR) {
				value.color.float32[0] = clearData.color.x;
				value.color.float32[1] = clearData.color.y;
				value.color.float32[2] = clearData.color.z;
				value.color.float32[3] = clearData.color.w;
			} else {
				value.depthStencil.depth = clearData.depth;
				value.depthStencil.stencil = 0;
			}
		}
		clearValues.push_back(value);
	}

	info.clearValueCount = clearValues.size();
	info.pClearValues = clearValues.data();

	width = frameBuffer->getWidth();
	height = frameBuffer->getHeight();

	vkCmdBeginRenderPass(commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

	renderPassBody();

	vkCmdEndRenderPass(commandBuffer);
}

void VulkanCommandBuffer::graphicsPipeline(Ref<const GraphicsPipeline> pipeline, Function<void()> pipelineBody) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, CastDown<const VulkanPipeline>(pipeline)->getPipeline());

	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = { width, height };
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	currentGraphicsPipeline = CastDown<const VulkanGraphicsPipeline>(pipeline);
	currentPipeline = CastDown<const VulkanPipeline>(pipeline);

	pipelineBody();

	currentGraphicsPipeline = null;
}

void VulkanCommandBuffer::draw(u32 vertexCount, u32 vertexOffset, u32 instanceCount, u32 instanceOffset) {
	vkCmdDraw(commandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);
}

void VulkanCommandBuffer::drawIndexed(u32 indexCount, u32 indexOffset, u32 vertexOffset, u32 instanceCount, u32 instanceOffset) {
	vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset);
}

void VulkanCommandBuffer::uniforms(Function<void(StructAccessor)> uniformCallback) {
	auto blueprint = currentPipeline->getUniformAccessor();
	void *data = currentPipeline->getUniformData();

	uniformCallback(StructAccessor(blueprint.get(), data));

	vkCmdPushConstants(commandBuffer, currentPipeline->getLayout(), VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT, 0, blueprint->size, data);
}

void VulkanCommandBuffer::vertexArray(Ref<const ArrayBuffer> vertexBuffer, Ref<const ArrayBuffer> indexBuffer, Function<void()> vertexArrayCallback) {
	if (vertexBuffer) {
		VkBuffer vulkanVertexBuffer = CastDown<const VulkanArrayBuffer>(vertexBuffer)->getBuffer();
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vulkanVertexBuffer, &offset);
	}

	if (indexBuffer) {
		VkBuffer vulkanIndexBuffer = CastDown<const VulkanArrayBuffer>(indexBuffer)->getBuffer();
		vkCmdBindIndexBuffer(commandBuffer, vulkanIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	vertexArrayCallback();
}

void VulkanCommandBuffer::descriptorSet(Ref<const DescriptorSet> set, Function<void()> setCallback) {
	VkDescriptorSet vkSet = CastDown<const VulkanDescriptorSet>(set)->getDescriptorSet();
	VkPipelineLayout layout = currentPipeline->getLayout();
	vkCmdBindDescriptorSets(commandBuffer, getCurrentBindPoint(), layout, 0, 1, &vkSet, 0, null);

	setCallback();
}

void VulkanCommandBuffer::descriptorSets(const Array<Ref<const DescriptorSet>> &sets, Function<void()> setsCallback) {
	Array<VkDescriptorSet> vkSets(sets.size());
	for (u32 i = 0; i < sets.size(); i++) {
		vkSets[i] = CastDown<const VulkanDescriptorSet>(sets[i])->getDescriptorSet();
	}
	VkPipelineLayout layout = currentPipeline->getLayout();
	vkCmdBindDescriptorSets(commandBuffer, getCurrentBindPoint(), layout, 0, vkSets.size(), vkSets.data(), 0,
	null);

	setsCallback();
}

void VulkanCommandBuffer::copyImage2D(Ref<ImageView2D> source, ImageLayout sourceLayout, Ref<ImageView2D> destination, ImageLayout destinationLayout) {
	auto vkSourceImage = CastDown<VulkanImageView2D>(source);
	auto vkDestinationImage = CastDown<VulkanImageView2D>(destination);

	VkImageCopy region;
	region.extent = { source->width, source->height, 1 };

	region.srcOffset = { 0, 0, 0 };
	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.baseArrayLayer = 0;
	region.srcSubresource.layerCount = 1;
	region.srcSubresource.mipLevel = source->baseLevel;

	region.dstOffset = { 0, 0, 0 };
	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.baseArrayLayer = 0;
	region.dstSubresource.layerCount = 1;
	region.dstSubresource.mipLevel = destination->baseLevel;

	vkCmdCopyImage(commandBuffer, vkSourceImage->getImage(), getVkImageLayout(sourceLayout, vkSourceImage->getType()), vkDestinationImage->getImage(),
		getVkImageLayout(destinationLayout, vkDestinationImage->getType()), 1, &region);
}

void VulkanCommandBuffer::blitImage2D(Ref<ImageView2D> source, ImageLayout sourceLayout, Ref<ImageView2D> destination, ImageLayout destinationLayout,
	SamplingMode samplingMide) {
	auto vkSourceImage = CastDown<VulkanImageView2D>(source);
	auto vkDestinationImage = CastDown<VulkanImageView2D>(destination);

	VkImageBlit region;

	region.srcOffsets[0] = { 0, 0, 0 };
	region.srcOffsets[1] = { (s32) source->width, (s32) source->height, 1 };

	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.baseArrayLayer = 0;
	region.srcSubresource.layerCount = 1;
	region.srcSubresource.mipLevel = source->baseLevel;

	region.dstOffsets[0] = { 0, 0, 0 };
	region.dstOffsets[1] = { (s32) destination->width, (s32) destination->height, 1 };

	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.baseArrayLayer = 0;
	region.dstSubresource.layerCount = 1;
	region.dstSubresource.mipLevel = destination->baseLevel;

	vkCmdBlitImage(commandBuffer, vkSourceImage->getImage(), getVkImageLayout(sourceLayout, vkSourceImage->getType()), vkDestinationImage->getImage(),
		getVkImageLayout(destinationLayout, vkDestinationImage->getType()), 1, &region, getVkFilter(samplingMide));
}

void VulkanCommandBuffer::barrier(const Array<ImageLayoutTransition> &imageLayoutTransitions) {
	for (ImageLayoutTransition transition : imageLayoutTransitions) { // TODO: call vkCmdPipelineBarrier only one time
		VkImage vkImage = CastDown<VulkanImage2D>(transition.image)->getImage();
		pipelineBarrier(commandBuffer, vkImage, transition.source, transition.destination, 0, 1, transition.image->format->type);
	}
}

void VulkanCommandBuffer::computePipeline(Ref<const ComputePipeline> pipeline, Function<void()> pipelineBody) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, CastDown<const VulkanPipeline>(pipeline)->getPipeline());

	currentComputePipeline = CastDown<const VulkanComputePipeline>(pipeline);
	currentPipeline = CastDown<const VulkanPipeline>(pipeline);

	pipelineBody();

	currentComputePipeline = null;
}

static u32 getWorkGroupCount(u32 globalSize, u32 localSize) {
	if (globalSize % localSize == 0) {
		return globalSize / localSize;
	}
	return globalSize / localSize + 1;
}

void VulkanCommandBuffer::compute(math::Vec3u32 size, ComputeSize space) {
	math::Vec3u32 invocationCount = size;
	if (space == ComputeSize::GLOBAL) {
		math::Vec3u32 localSize = currentComputePipeline->getSource()->localSize;
		invocationCount.x = getWorkGroupCount(size.x, localSize.x);
		invocationCount.y = getWorkGroupCount(size.y, localSize.y);
		invocationCount.z = getWorkGroupCount(size.z, localSize.z);
	}

	vkCmdDispatch(commandBuffer, invocationCount.x, invocationCount.y, invocationCount.z);
}

}

}
