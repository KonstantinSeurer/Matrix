/*
 * ModelMap.h
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SCENEMODELMAP_H_
#define SCENE_RENDERER_SCENEMODELMAP_H_

#include "../Scene.h"
#include "../Model.h"

namespace matrix {

namespace scene {

class SceneModelMap: public SceneRenderImplementation {
private:
	OrderedMap<Model*, Array<const ModelComponent*>> entries;
public:
	SceneModelMap() {
	}

	virtual void render(const Scene &scene);

	const OrderedMap<Model*, Array<const ModelComponent*>>& getEntries() const {
		return entries;
	}

	MATRIX_SCENE_RENDER_IMPLEMENTATION(Scene, SceneModelMap, { });
};

}

}

#endif /* SCENE_RENDERER_SCENEMODELMAP_H_ */
