/*
 * Camera.cpp
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#include <scene/Camera.h>

namespace matrix
{

	namespace scene
	{

		PerspectiveFrustum &PerspectiveFrustum::setYFov(f32 yfov)
		{
			this->yfov = yfov;
			projection = math::Mat4::perspective(yfov, aspect, near, far);
			return *this;
		}

		PerspectiveFrustum &PerspectiveFrustum::setAspect(f32 aspect)
		{
			this->aspect = aspect;
			projection = math::Mat4::perspective(yfov, aspect, near, far);
			return *this;
		}

		PerspectiveFrustum &PerspectiveFrustum::setNear(f32 near)
		{
			this->near = near;
			projection = math::Mat4::perspective(yfov, aspect, near, far);
			return *this;
		}

		PerspectiveFrustum &PerspectiveFrustum::setFar(f32 far)
		{
			this->far = far;
			projection = math::Mat4::perspective(yfov, aspect, near, far);
			return *this;
		}

		CameraComponent::CameraComponent() : parent(null), frustum(null)
		{
		}

		CameraComponent::CameraComponent(Ref<Frustum> frustum) : parent(null), frustum(frustum)
		{
		}

		void CameraComponent::init(Entity *parent, Scene *scene)
		{
			this->parent = parent;
		}

		const TransformComponent *CameraComponent::getTransform() const
		{
			return parent->getComponent<TransformComponent>();
		}

	}

}
