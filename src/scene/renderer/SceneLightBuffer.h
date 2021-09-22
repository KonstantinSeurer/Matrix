/*
 * MaterialBuffer.h
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SCENELIGHTBUFFER_H_
#define SCENE_RENDERER_SCENELIGHTBUFFER_H_

#include "../Material.h"
#include "../Scene.h"
#include "../../graphics/Device.h"

namespace matrix {

namespace scene {

class SceneLightBuffer: public SceneRenderImplementation {
private:
	struct FrameData {
		Ref<graphics::DescriptorSet> descriptorSet;
		Ref<graphics::StructBuffer> buffer;
	};

	Array<FrameData> buffers;
public:
	SceneLightBuffer();

	virtual ~SceneLightBuffer();

	virtual void init(Ref<graphics::DeviceInstance> device, u32 bufferCount);
	virtual void updateDescriptors(const Scene &scene, u32 bufferIndex);

	Ref<graphics::DescriptorSet> getDescriptorSet(u32 bufferIndex) const {
		return buffers[bufferIndex].descriptorSet;
	}

	static Ref<graphics::PipelineSource> getSource();

	MATRIX_SCENE_RENDER_IMPLEMENTATION(Scene, SceneLightBuffer, { })
};

}

}

#endif /* SCENE_RENDERER_SCENELIGHTBUFFER_H_ */
