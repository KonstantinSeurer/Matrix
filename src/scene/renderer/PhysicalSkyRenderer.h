/*
 * PhysicalSkyRenderer.h
 *
 *  Created on: Sep 3, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_PHYSICALSKYRENDERER_H_
#define SCENE_RENDERER_PHYSICALSKYRENDERER_H_

#include "SkyRenderer.h"

namespace matrix
{

namespace scene
{

class PhysicalSkyRenderer: public SkyRenderer
{
private:
	Ref<graphics::ComputePipeline> pipeline;
	Ref<graphics::DescriptorSet> descriptorSet;
public:
	PhysicalSkyRenderer(Ref<graphics::DeviceInstance> device, u32 resolution);

	virtual void updateDescriptors(u32 bufferIndex)
	{
	}

	virtual void render(Ref<graphics::CommandBuffer> cb, const Scene &scene, ComponentReference camera, u32 bufferIndex);
};

}

}

#endif /* SCENE_RENDERER_PHYSICALSKYRENDERER_H_ */
