/*
 * MaterialBuffer.h
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SCENECAMERABUFFER_H_
#define SCENE_RENDERER_SCENECAMERABUFFER_H_

#include "../Scene.h"
#include "../../graphics/Device.h"

namespace matrix
{

namespace scene
{

class SceneCameraBuffer: public SceneRenderImplementation
{
private:
	struct FrameData
	{
		Ref<graphics::DescriptorSet> descriptorSet;
		Ref<graphics::StructBuffer> buffer;
	};

	Array<FrameData> buffers;
public:
	SceneCameraBuffer();

	virtual ~SceneCameraBuffer();

	virtual void init(Ref<graphics::DeviceInstance> device, u32 bufferCount);
	virtual void updateDescriptors(const Scene &scene, u32 bufferIndex);
	virtual void render(const Scene &scene);

	Ref<graphics::DescriptorSet> getDescriptorSet(u32 bufferIndex) const
	{
		return buffers[bufferIndex].descriptorSet;
	}

	static Ref<graphics::PipelineSource> getSource();

	MATRIX_SCENE_RENDER_IMPLEMENTATION(Scene, SceneCameraBuffer, Array<RenderImplementationType>())
};

}

}

#endif /* SCENE_RENDERER_SCENECAMERABUFFER_H_ */
