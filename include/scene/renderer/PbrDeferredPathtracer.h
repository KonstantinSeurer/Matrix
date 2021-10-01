/*
 * ShadelessForwardRenderer.h
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SHADELESSFORWARDRENDERER_H_
#define SCENE_RENDERER_SHADELESSFORWARDRENDERER_H_

#include "Renderer.h"
#include "DeferredRenderer.h"
#include "SkyRenderer.h"

namespace matrix {

namespace scene {

class PbrDeferredPathtracer: public Renderer {
private:
	Ref<DeferredRenderer> gBufferRenderer;
	Ref<graphics::Sampler2D> gBufferSampler;
	Ref<graphics::ImageSampler2D> albedoMetallicSampler;
	Ref<graphics::ImageSampler2D> normalRoughnessSampler;
	Ref<graphics::ImageSampler2D> emissiveOcclusionSampler;
	Ref<graphics::ImageSampler2D> depthSampler;

	Ref<SkyRenderer> skyRenderer;
	Ref<graphics::Sampler2D> skySampler;

	Ref<graphics::DeviceInstance> device;

	Ref<graphics::ImageFormat> resultFormat;
	Ref<graphics::Image2D> result;
	Ref<graphics::ImageView2D> resultView;

	Ref<graphics::DescriptorSet> descriptorSet;

	Ref<graphics::ComputePipeline> pipeline;

	u32 frameIndex = 0;
public:
	PbrDeferredPathtracer(Ref<graphics::DeviceInstance> device, Ref<SkyRenderer> skyRenderer);
	virtual ~PbrDeferredPathtracer();

	virtual void updateDescriptors(u32 bufferIndex);
	virtual void render(Ref<graphics::CommandBuffer> cb, const Scene &scene, ComponentReference camera, u32 bufferIndex);
	virtual void resize(u32 width, u32 height);
	virtual Ref<graphics::ImageView2D> getResult(u32 bufferIndex) const;
};

}

}

#endif /* SCENE_RENDERER_SHADELESSFORWARDRENDERER_H_ */
