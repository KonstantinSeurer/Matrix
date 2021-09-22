/*
 * DemoCamera.cpp
 *
 *  Created on: Sep 18, 2021
 *      Author: konstantin
 */

#include "DemoCamera.h"

namespace matrix
{

using namespace scene;
using namespace math;
using namespace input;

DemoCamera::DemoCamera(scene::Scene &scene) :
	height(1)
{
	entity = scene.createEntity();

	entity->addComponent<TransformComponent>();

	frustum = CastDown<PerspectiveFrustum>(entity->addComponent<CameraComponent>(allocate<PerspectiveFrustum>())->frustum);
	frustum->setNear(1.0f);
	frustum->setFar(1000.0f);

	scene.add(entity);
}

void DemoCamera::update(const UpdateData &data)
{
	TransformComponent *transform = entity->getComponent<TransformComponent>();

	if (data.mouse.isButtonPressed(MouseButton::LEFT))
	{
		rotation -= data.mouse.movement / (f32) height * frustum->getYFov();
		transform->rotation = Quat(math::Vec3(0.0f, 1.0f, 0.0f), rotation.x) * Quat(math::Vec3(1.0f, 0.0f, 0.0f), rotation.y);
	}

	f32 dMovement = data.delta.seconds() * 3.0f;
	if (data.keyboard.isKeyPressed(Key::W))
	{
		transform->position.x += dMovement * sin(rotation.x);
		transform->position.z += dMovement * cos(rotation.x);
	}
	if (data.keyboard.isKeyPressed(Key::S))
	{
		transform->position.x -= dMovement * sin(rotation.x);
		transform->position.z -= dMovement * cos(rotation.x);
	}
	if (data.keyboard.isKeyPressed(Key::A))
	{
		transform->position.x += dMovement * sin(rotation.x - PI * 0.5f);
		transform->position.z += dMovement * cos(rotation.x - PI * 0.5f);
	}
	if (data.keyboard.isKeyPressed(Key::D))
	{
		transform->position.x -= dMovement * sin(rotation.x - PI * 0.5f);
		transform->position.z -= dMovement * cos(rotation.x - PI * 0.5f);
	}
	if (data.keyboard.isKeyPressed(Key::SPACE))
	{
		transform->position.y += dMovement;
	}
	if (data.keyboard.isKeyPressed(Key::LEFT_SHIFT))
	{
		transform->position.y -= dMovement;
	}
}

void DemoCamera::resize(u32 width, u32 height)
{
	this->height = height;
	frustum->setAspect((f32) width / (f32) height);
}

}
