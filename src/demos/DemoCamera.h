/*
 * DemoCamera.h
 *
 *  Created on: Sep 18, 2021
 *      Author: konstantin
 */

#ifndef DEMOS_DEMOCAMERA_H_
#define DEMOS_DEMOCAMERA_H_

#include "../scene/Camera.h"

namespace matrix
{

class DemoCamera
{
private:
	Ref<scene::Entity> entity;
	Ref<scene::PerspectiveFrustum> frustum;
	u32 height;
	math::Vec2 rotation;
public:
	DemoCamera(scene::Scene& scene);

	void update(const UpdateData& data);
	void resize(u32 width, u32 height);

	scene::ComponentReference getCameraReference() const
	{
		return *entity->getComponentReference<scene::CameraComponent>();
	}

	scene::TransformComponent* getTransform()
	{
		return entity->getComponent<scene::TransformComponent>();
	}
};

}

#endif /* DEMOS_DEMOCAMERA_H_ */
