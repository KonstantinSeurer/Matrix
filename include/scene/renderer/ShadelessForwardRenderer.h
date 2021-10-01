/*
 * ShadelessForwardRenderer.h
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SHADELESSFORWARDRENDERER_H_
#define SCENE_RENDERER_SHADELESSFORWARDRENDERER_H_

#include "Renderer.h"
#include "../../graphics/Device.h"

namespace matrix {

namespace scene {

class ShadelessForwardRenderer: public Renderer {
private:
	Ref<graphics::DeviceInstance> device;
	Ref<graphics::ImageFormat> colorFormat;
	Ref<graphics::ImageFormat> depthFormat;
	Ref<graphics::Image2D> colorAttachment;
	Ref<graphics::ImageView2D> colorAttachmentView;
	Ref<graphics::Image2D> depthAttachment;
	Ref<graphics::ImageView2D> depthAttachmentView;
	Ref<graphics::RenderPass> renderPass;
	Ref<graphics::FrameBuffer> frameBuffer;
	Ref<graphics::GraphicsPipeline> pipeline;
public:
	ShadelessForwardRenderer(Ref<graphics::DeviceInstance> device);
	virtual ~ShadelessForwardRenderer();

	virtual void updateDescriptors(u32 bufferIndex);
	virtual void render(Ref<graphics::CommandBuffer> cb, const Scene &scene, ComponentReference camera, u32 bufferIndex);
	virtual void resize(u32 width, u32 height);
	virtual Ref<graphics::ImageView2D> getResult(u32 bufferIndex) const;
};

}

}

#endif /* SCENE_RENDERER_SHADELESSFORWARDRENDERER_H_ */
