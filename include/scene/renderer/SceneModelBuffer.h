/*
 * MaterialBuffer.h
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SCENEMODELBUFFER_H_
#define SCENE_RENDERER_SCENEMODELBUFFER_H_

#include "ModelModelBuffer.h"
#include "SceneModelMap.h"

namespace matrix {

namespace scene {

class SceneModelBuffer: public SceneRenderImplementation {
private:
	struct FrameData {
		Ref<graphics::DescriptorSet> descriptorSet;
		Ref<graphics::StructBuffer> typeTable;
	};

	Array<FrameData> buffers;
public:
	SceneModelBuffer();

	virtual ~SceneModelBuffer();

	virtual void init(Ref<graphics::DeviceInstance> device, u32 bufferCount);
	virtual void updateDescriptors(const Scene &scene, u32 bufferIndex);

	Ref<graphics::DescriptorSet> getDescriptorSet(u32 bufferIndex) const {
		return buffers[bufferIndex].descriptorSet;
	}

	MATRIX_SCENE_RENDER_IMPLEMENTATION(Scene, SceneModelBuffer, { SceneModelMap::type() })
};

}

}

#endif /* SCENE_RENDERER_SCENEMODELBUFFER_H_ */
