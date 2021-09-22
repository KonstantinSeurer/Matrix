/*
 * ModelMap.cpp
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#include "SceneModelMap.h"

#include "../../Logging.h"

namespace matrix {

namespace scene {

void SceneModelMap::render(const Scene &scene) {
	entries.clear();

	for (const ModelComponent &c : scene.getComponents<ModelComponent>(ModelComponent::type())) {
		if (entries.find(c.getModel().get()) == entries.end()) {
			entries[c.getModel().get()] = { };
		}
		entries[c.getModel().get()].push_back(&c);
	}
}

}

}
