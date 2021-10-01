/*
 * SceneAccelerationStructure.h
 *
 *  Created on: Aug 14, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SCENEACCELERATIONSTRUCTURE_H_
#define SCENE_RENDERER_SCENEACCELERATIONSTRUCTURE_H_

#include "../Scene.h"
#include "../../graphics/AccelerationStructure.h"
#include "ModelAccelerationStructure.h"
#include "SceneModelMap.h"

namespace matrix {

namespace scene {

class SceneAccelerationStructure: public SceneRenderImplementation {
private:
	struct FrameData {
		Ref<graphics::TopLevelAccelerationStructure> accelerationStructure;
		Ref<graphics::DescriptorSet> descriptorSet;
	};

	Array<FrameData> buffers;
public:
	SceneAccelerationStructure() {
	}

	virtual void init(Ref<graphics::DeviceInstance> device, u32 bufferCount);
	virtual void updateDescriptors(const Scene &scene, u32 bufferIndex);

	Ref<graphics::TopLevelAccelerationStructure> getAccelerationStructure(u32 bufferIndex) const {
		return buffers[bufferIndex].accelerationStructure;
	}

	Ref<graphics::DescriptorSet> getDescriptorSet(u32 bufferIndex) const {
		return buffers[bufferIndex].descriptorSet;
	}

#define SCENEACCELERATIONSTRUCTURE_DEPENDENCIES {ModelAccelerationStructure::type(), SceneModelMap::type()}
	MATRIX_SCENE_RENDER_IMPLEMENTATION(Scene, SceneAccelerationStructure, SCENEACCELERATIONSTRUCTURE_DEPENDENCIES)

	static Ref<graphics::PipelineSource> getSource();
};

}

}

#endif /* SCENE_RENDERER_SCENEACCELERATIONSTRUCTURE_H_ */
