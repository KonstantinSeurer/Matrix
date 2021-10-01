/*
 * ModelStorageBuffer.h
 *
 *  Created on: Aug 14, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_MODELMODELBUFFER_H_
#define SCENE_RENDERER_MODELMODELBUFFER_H_

#include "../Model.h"
#include "../../graphics/Buffer.h"
#include "../../graphics/Device.h"

namespace matrix {

namespace scene {

class ModelModelBuffer: public ModelRenderImplementation {
private:
	Ref<graphics::StructBuffer> floats = null;
	Ref<graphics::StructBuffer> ints = null;
	Ref<graphics::StructBuffer> indices = null;
public:
	virtual void init(const ModelType *type, Ref<graphics::DeviceInstance> device, const Array<void*> &attributes, u32 vertexCount, const Array<u32> &indices);

	Ref<graphics::StructBuffer> getFloats() const {
		return floats;
	}

	Ref<graphics::StructBuffer> getInts() const {
		return ints;
	}

	Ref<graphics::StructBuffer> getIndices() const {
		return indices;
	}

	MATRIX_SCENE_RENDER_IMPLEMENTATION(Model, ModelModelBuffer, {})

	static Ref<graphics::PipelineSource> getSource();
};

}

}

#endif /* SCENE_RENDERER_MODELMODELBUFFER_H_ */
