/*
 * ModelAccelerationStructure.h
 *
 *  Created on: Aug 14, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_MODELACCELERATIONSTRUCTURE_H_
#define SCENE_RENDERER_MODELACCELERATIONSTRUCTURE_H_

#include "../Model.h"
#include "../../graphics/AccelerationStructure.h"
#include "../../graphics/Device.h"

namespace matrix {

namespace scene {

class ModelAccelerationStructure: public ModelRenderImplementation {
private:
	Ref<graphics::BottomLevelAccelerationStructure> accelerationStructure;
public:
	virtual void init(const ModelType *type, Ref<graphics::DeviceInstance> device, const Array<void*> &attributes, u32 vertexCount, const Array<u32> &indices);

	Ref<graphics::BottomLevelAccelerationStructure> getAccelerationStructure() const {
		return accelerationStructure;
	}

	MATRIX_SCENE_RENDER_IMPLEMENTATION(Model, ModelAccelerationStructure, { })
};

}

}

#endif /* SCENE_RENDERER_MODELACCELERATIONSTRUCTURE_H_ */
