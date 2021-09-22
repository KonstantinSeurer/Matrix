/*
 * VulkanCommandBuffer.h
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANCOMMANDBUFFER_H_
#define GRAPHICS_VULKAN_GLFW_VULKANCOMMANDBUFFER_H_

#include "../CommandBuffer.h"
#include <vulkan/vulkan.h>

#include "VulkanPipeline.h"

namespace matrix {

namespace graphics {

class VulkanCommandBuffer: public CommandBuffer {
private:
	VkCommandBuffer commandBuffer;
	u32 width;
	u32 height;

	Ref<const VulkanGraphicsPipeline> currentGraphicsPipeline;
	Ref<const VulkanComputePipeline> currentComputePipeline;
	Ref<const VulkanPipeline> currentPipeline;
private:
	VkPipelineBindPoint getCurrentBindPoint() const;
public:
	VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool);

	virtual ~VulkanCommandBuffer();

	virtual void record(Function<void()> recordCallback);

	virtual void renderPass(Ref<const RenderPass> renderPass, Ref<const FrameBuffer> frameBuffer, const ClearData &clearData, Function<void()> renderPassBody);

	virtual void graphicsPipeline(Ref<const GraphicsPipeline> pipeline, Function<void()> pipelineBody);

	virtual void draw(u32 vertexCount, u32 vertexOffset, u32 instanceCount, u32 instanceOffset);
	virtual void drawIndexed(u32 indexCount, u32 indexOffset, u32 vertexOffset, u32 instanceCount, u32 instanceOffset);

	virtual void uniforms(Function<void(StructAccessor)> uniformCallback);

	virtual void vertexArray(Ref<const ArrayBuffer> vertexBuffer, Ref<const ArrayBuffer> indexBuffer, Function<void()> vertexArrayCallback);

	virtual void descriptorSet(Ref<const DescriptorSet> set, Function<void()> setCallback);
	virtual void descriptorSets(const Array<Ref<const DescriptorSet>> &sets, Function<void()> setsCallback);

	virtual void copyImage2D(Ref<ImageView2D> source, ImageLayout sourceLayout, Ref<ImageView2D> destination, ImageLayout destinationLayout);

	virtual void blitImage2D(Ref<ImageView2D> source, ImageLayout sourceLayout, Ref<ImageView2D> destination, ImageLayout destinationLayout,
		SamplingMode samplingMide);

	virtual void barrier(const Array<ImageLayoutTransition> &imageLayoutTransitions);

	virtual void computePipeline(Ref<const ComputePipeline> pipeline, Function<void()> pipelineBody);

	virtual void compute(math::Vec3u32 size, ComputeSize space);

	VkCommandBuffer getCommandBuffer() const {
		return commandBuffer;
	}

	const VkCommandBuffer* getCommandBufferPointer() const {
		return &commandBuffer;
	}
};

}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANCOMMANDBUFFER_H_ */
