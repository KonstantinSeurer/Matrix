/*
 * ShadelessForwardRenderer.h
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_DEFERREDRENDERER_H_
#define SCENE_RENDERER_DEFERREDRENDERER_H_

#include "Renderer.h"
#include "../../graphics/Device.h"

namespace matrix {

namespace scene {

class DeferredRenderer {
private:
	Ref<graphics::DeviceInstance> device;

	Ref<graphics::ImageFormat> colorFormat;
	Ref<graphics::ImageFormat> depthFormat;

	Ref<graphics::Image2D> albedoMetallicAttachment;
	Ref<graphics::ImageView2D> albedoMetallicAttachmentView;

	Ref<graphics::Image2D> normalRoughnessAttachment;
	Ref<graphics::ImageView2D> normalRoughnessAttachmentView;

	Ref<graphics::Image2D> emissiveOcclusionAttachment;
	Ref<graphics::ImageView2D> emissiveOcclusionAttachmentView;

	Ref<graphics::Image2D> depthAttachment;
	Ref<graphics::ImageView2D> depthAttachmentView;

	Ref<graphics::RenderPass> renderPass;
	Ref<graphics::FrameBuffer> frameBuffer;
	Ref<graphics::GraphicsPipeline> pipeline;
public:
	DeferredRenderer(Ref<graphics::DeviceInstance> device);

	void render(Ref<graphics::CommandBuffer> cb, const Scene &scene, ComponentReference camera, u32 bufferIndex);
	void resize(u32 width, u32 height);

	Ref<graphics::Image2D> getAlbedoMetallic() const {
		return albedoMetallicAttachment;
	}

	Ref<graphics::ImageView2D> getAlbedoMetallicView() const {
		return albedoMetallicAttachmentView;
	}

	Ref<graphics::Image2D> getNormalRoughness() const {
		return normalRoughnessAttachment;
	}

	Ref<graphics::ImageView2D> getNormalRoughnessView() const {
		return normalRoughnessAttachmentView;
	}

	Ref<graphics::Image2D> getEmissiveOcclusion() const {
		return emissiveOcclusionAttachment;
	}

	Ref<graphics::ImageView2D> getEmissiveOcclusionView() const {
		return emissiveOcclusionAttachmentView;
	}

	Ref<graphics::Image2D> getDepth() const {
		return depthAttachment;
	}

	Ref<graphics::ImageView2D> getDepthView() const {
		return depthAttachmentView;
	}
};

}

}

#endif /* SCENE_RENDERER_DEFERREDRENDERER_H_ */
