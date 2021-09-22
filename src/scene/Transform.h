/*
 * Transform.h
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#ifndef SCENE_TRANSFORM_H_
#define SCENE_TRANSFORM_H_

#include "Scene.h"
#include "../math/Vec3.h"
#include "../math/Mat4.h"
#include "../math/Quat.h"

namespace matrix {

namespace scene {

class TransformComponent;

void updateTransformComponents(Array<TransformComponent> &components, const UpdateData& data);

class TransformComponent: public Component {
public:
	math::Mat4 localTransform;
	TransformComponent *parent;
	math::Vec3 position;
	math::Vec3 scale;
	math::Quat rotation;

	TransformComponent() :
			parent(null), position(0.0f), scale(1.0f) {
	}

	TransformComponent(math::Vec3 position) :
			parent(null), position(position), scale(1.0f) {
	}

	TransformComponent(math::Vec3 position, math::Vec3 scale) :
			parent(null), position(position), scale(scale) {
	}

	TransformComponent(math::Vec3 position, math::Vec3 scale, math::Quat rotation) :
			parent(null), position(position), scale(scale), rotation(rotation) {
	}

	math::Mat4 getGlobalTransform() const;

	MATRIX_SCENE_COMPONENT(TransformComponent, updateTransformComponents, {})
};

}

}

#endif /* SCENE_TRANSFORM_H_ */
