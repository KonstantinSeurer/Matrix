/*
 * ModelVertexArray.h
 *
 *  Created on: Aug 8, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_MODELVERTEXARRAY_H_
#define SCENE_RENDERER_MODELVERTEXARRAY_H_

#include "../Model.h"
#include "../../graphics/Buffer.h"
#include "../../graphics/Device.h"

namespace matrix {

namespace scene {

class ModelVertexArray: public ModelRenderImplementation {
private:
	Ref<graphics::ArrayBuffer> vertexBuffer;
	Ref<graphics::ArrayBuffer> indexBuffer;
public:
	virtual void init(const ModelType *type, Ref<graphics::DeviceInstance> device, const Array<void*> &attributes, u32 vertexCount, const Array<u32> &indices);

	Ref<const graphics::ArrayBuffer> getVertexBuffer() const {
		return vertexBuffer;
	}

	Ref<const graphics::ArrayBuffer> getIndexBuffer() const {
		return indexBuffer;
	}

	MATRIX_SCENE_RENDER_IMPLEMENTATION(Model, ModelVertexArray, { })
};

}

}

#endif /* SCENE_RENDERER_MODELVERTEXARRAY_H_ */
