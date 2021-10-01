/*
 * SkyRenderer.h
 *
 *  Created on: Sep 3, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SKYRENDERER_H_
#define SCENE_RENDERER_SKYRENDERER_H_

#include "../../Base.h"
#include "../../graphics/Device.h"
#include "../Scene.h"

namespace matrix
{

namespace scene
{

class SkyRenderer
{
private:
	Ref<graphics::Image2D> environment;
	Ref<graphics::ImageView2D> environmentView;
	Ref<graphics::Image2D> transmittance;
	Ref<graphics::ImageView2D> transmittanceView;
public:
	SkyRenderer(Ref<graphics::DeviceInstance> device, u32 resolution);

	virtual ~SkyRenderer()
	{
	}

	Ref<graphics::Image2D> getEnvironment() const
	{
		return environment;
	}

	Ref<graphics::ImageView2D> getEnvironmentView() const
	{
		return environmentView;
	}

	Ref<graphics::Image2D> getTransmittance() const
	{
		return transmittance;
	}

	Ref<graphics::ImageView2D> getTransmittanceView() const
	{
		return transmittanceView;
	}

	virtual void updateDescriptors(u32 bufferIndex) = 0;

	virtual void render(Ref<graphics::CommandBuffer> cb, const Scene &scene, ComponentReference camera, u32 bufferIndex) = 0;
};

}

}

#endif /* SCENE_RENDERER_SKYRENDERER_H_ */
