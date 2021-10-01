/*
 * Renderer.h
 *
 *  Created on: Jul 8, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_RENDERER_H_
#define SCENE_RENDERER_RENDERER_H_

#include "../Scene.h"
#include "../../graphics/CommandBuffer.h"
#include "../../graphics/Image.h"

namespace matrix {

namespace scene {

class Renderer {
public:
	Renderer() {
	}

	virtual ~Renderer() {
	}

	virtual void updateDescriptors(u32 bufferIndex) = 0;

	virtual void render(Ref<graphics::CommandBuffer> cb, const Scene& scene, ComponentReference camera, u32 bufferIndex) = 0;

	virtual void resize(u32 width, u32 height) = 0;

	virtual Ref<graphics::ImageView2D> getResult(u32 bufferIndex) const = 0;
};

}

}

#endif /* SCENE_RENDERER_RENDERER_H_ */
