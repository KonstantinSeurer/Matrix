/*
 * Light.h
 *
 *  Created on: Aug 19, 2021
 *      Author: konstantin
 */

#ifndef SCENE_LIGHT_H_
#define SCENE_LIGHT_H_

#include "Scene.h"
#include "Transform.h"
#include "../math/Vec3.h"
#include "../graphics/Buffer.h"
#include "../graphics/Pipeline.h"

namespace matrix {

namespace scene {

class LightComponent: public Component {
private:
	Entity *parent;
public:
	math::Vec3 color;

	LightComponent() :
		parent(null) {
	}

	LightComponent(math::Vec3 color) :
		parent(null), color(color) {
	}

	virtual ~LightComponent() {
	}

	virtual void init(Entity *parent, Scene *scene) {
		this->parent = parent;
	}

	const TransformComponent* getTransform() const {
		return parent->getComponent<TransformComponent>();
	}

	MATRIX_SCENE_COMPONENT(LightComponent, null,
		{	TransformComponent::type()})
};

class DirectionalLightComponent: public LightComponent {
public:
	math::Vec3 direction;

	DirectionalLightComponent() :
		LightComponent(), direction() {
	}

	DirectionalLightComponent(math::Vec3 direction) :
		LightComponent(), direction(direction) {
	}

	DirectionalLightComponent(math::Vec3 color, math::Vec3 direction) :
		LightComponent(color), direction(direction) {
	}

	MATRIX_SCENE_COMPONENT(DirectionalLightComponent, null,
			{	TransformComponent::type()})
};

}

}

#endif /* SCENE_LIGHT_H_ */
