/*
 * Camera.h
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#ifndef SCENE_CAMERA_H_
#define SCENE_CAMERA_H_

#include "Scene.h"
#include "Transform.h"
#include "../math/Mat4.h"

namespace matrix
{

namespace scene
{

class Frustum
{
public:
	Frustum()
	{
	}

	virtual ~Frustum()
	{
	}

	virtual const math::Mat4& getProjection() const = 0;
};

class PerspectiveFrustum: public Frustum
{
private:
	math::Mat4 projection;
	f32 yfov;
	f32 aspect;
	f32 near;
	f32 far;

public:
	PerspectiveFrustum()
	{
		setYFov(math::radians(50.0f));
		setAspect(1.0f);
		setNear(0.5f);
		setFar(100.0f);
	}

	PerspectiveFrustum& setYFov(f32 yfov);

	f32 getYFov() const
	{
		return yfov;
	}

	PerspectiveFrustum& setAspect(f32 aspect);

	f32 getAspect() const
	{
		return aspect;
	}

	PerspectiveFrustum& setNear(f32 near);

	f32 getNear() const
	{
		return near;
	}

	PerspectiveFrustum& setFar(f32 far);

	f32 getFar() const
	{
		return far;
	}

	virtual const math::Mat4& getProjection() const override
	{
		return projection;
	}
};

class CameraComponent: public Component
{
private:
	Entity *parent;
public:
	Ref<Frustum> frustum;

	CameraComponent();

	CameraComponent(Ref<Frustum> frustum);

	virtual void init(Entity *parent, Scene *scene);

	const TransformComponent* getTransform() const;

	MATRIX_SCENE_COMPONENT(CameraComponent, null,
	{	TransformComponent::type()})
};

}

}

#endif /* SCENE_CAMERA_H_ */
