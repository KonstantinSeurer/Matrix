/*
 * Transform.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#include <scene/Transform.h>

namespace matrix
{

	namespace scene
	{

		math::Mat4 TransformComponent::getGlobalTransform() const
		{
			if (parent)
			{
				return parent->getGlobalTransform() * localTransform;
			}
			return localTransform;
		}

		void updateTransformComponents(Array<TransformComponent> &components, const UpdateData &data)
		{
			for (u32 i = 0; i < components.size(); i++)
			{
				TransformComponent &component = components[i];

				component.localTransform = math::Mat4(component.rotation);

				component.localTransform.x *= component.scale.x;
				component.localTransform.y *= component.scale.y;
				component.localTransform.z *= component.scale.z;

				component.localTransform.w.x = component.position.x;
				component.localTransform.w.y = component.position.y;
				component.localTransform.w.z = component.position.z;
			}
		}

	}

}
