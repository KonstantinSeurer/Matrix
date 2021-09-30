/*
 * Volume.h
 *
 *  Created on: Sep 28, 2021
 *      Author: konstantin
 */

#ifndef SCENE_VOLUME_H_
#define SCENE_VOLUME_H_

#include "Scene.h"
#include "Material.h"

namespace matrix
{

namespace scene
{

class Volume
{
private:
	math::Vec3 min;
	math::Vec3 max;
	Ref<const Material> material;
public:
	Volume(math::Vec3 min, math::Vec3 max, Ref<const Material> material) :
		min(min), max(max), material(material)
	{
	}

	math::Vec3 getMin() const
	{
		return min;
	}

	math::Vec3 getMax() const
	{
		return max;
	}

	Ref<const Material> getMaterial() const
	{
		return material;
	}
};

class VolumeComponent: public Component
{
public:
	VolumeComponent()
	{
	}

	MATRIX_SCENE_COMPONENT(VolumeComponent, null,
			{})
};

const MaterialType* getVolumetricScatteringUniformMaterialType();

}

}

#endif /* SCENE_VOLUME_H_ */
