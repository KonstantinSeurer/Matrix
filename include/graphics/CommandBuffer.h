/*
 * CommandBuffer.h
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_COMMANDBUFFER_H_
#define GRAPHICS_COMMANDBUFFER_H_

#include "RenderPass.h"
#include "FrameBuffer.h"
#include "../math/Vec4.h"
#include "../math/Vec3i.h"
#include "Buffer.h"
#include "DescriptorSet.h"

namespace matrix {

namespace graphics {

struct ClearData {
	math::Vec4 color;
	f32 depth;

	ClearData() :
			color(math::Vec4()), depth(0.0f) {
	}

	ClearData(const math::Vec4 &color, f32 depth) :
			color(color), depth(depth) {
	}
};

struct ImageLayoutTransition {
	Ref<Image> image;
	ImageLayout source;
	ImageLayout destination;

	ImageLayoutTransition(Ref<Image> image, ImageLayout source, ImageLayout destination) :
			image(image), source(source), destination(destination) {
	}
};

enum class ComputeSize {
	LOCAL, GLOBAL
};

class CommandBuffer {
public:
	CommandBuffer() {
	}

	virtual ~CommandBuffer() {
	}

	virtual void record(Function<void()> recordCallback) = 0;

	virtual void renderPass(Ref<const RenderPass> renderPass, Ref<const FrameBuffer> frameBuffer,
			const ClearData &clearData, Function<void()> renderPassBody) = 0;

	virtual void graphicsPipeline(Ref<const GraphicsPipeline> pipeline, Function<void()> pipelineBody) = 0;

	virtual void draw(u32 vertexCount, u32 vertexOffset, u32 instanceCount, u32 instanceOffset) = 0;
	virtual void drawIndexed(u32 indexCount, u32 indexOffset, u32 vertexOffset, u32 instanceCount,
			u32 instanceOffset) = 0;

	virtual void uniforms(Function<void(StructAccessor)> uniformCallback) = 0;

	virtual void vertexArray(Ref<const ArrayBuffer> vertexBuffer, Ref<const ArrayBuffer> indexBuffer,
			Function<void()> vertexArrayCallback) = 0;

	virtual void descriptorSet(Ref<const DescriptorSet> set, Function<void()> setCallback) = 0;
	virtual void descriptorSets(const Array<Ref<const DescriptorSet>> &sets, Function<void()> setsCallback) = 0;

	virtual void copyImage2D(Ref<ImageView2D> source, ImageLayout sourceLayout, Ref<ImageView2D> destination,
			ImageLayout destinationLayout) = 0;

	virtual void blitImage2D(Ref<ImageView2D> source, ImageLayout sourceLayout, Ref<ImageView2D> destination,
			ImageLayout destinationLayout, SamplingMode samplingMide) = 0;

	virtual void barrier(const Array<ImageLayoutTransition> &imageLayoutTransitions) = 0;

	virtual void computePipeline(Ref<const ComputePipeline> pipeline, Function<void()> pipelineBody) = 0;

	virtual void compute(math::Vec3u32 size, ComputeSize space) = 0;
};

}

}

#endif /* GRAPHICS_COMMANDBUFFER_H_ */
