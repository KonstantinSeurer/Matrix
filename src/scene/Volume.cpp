/*
 * Volume.cpp
 *
 *  Created on: Sep 28, 2021
 *      Author: konstantin
 */

#include <scene/Volume.h>
#include <graphics/Descriptor.h>

namespace matrix
{

	using namespace graphics;

	namespace scene
	{

		static MaterialType *materialType = null;

		const MaterialType *getVolumetricScatteringUniformMaterialType()
		{
			if (!materialType)
			{
				materialType = new MaterialType();
				materialType->addDescriptor({DescriptorType::IMAGE_SAMPLER_3D, "density"});
				materialType->addAttribute({PrimitiveTypeType::VEC3, "albedo"});
				materialType->addAttribute({PrimitiveTypeType::VEC3, "emissive"});
				materialType->addAttribute({PrimitiveTypeType::FLOAT, "absorbtion"});
			}
			return materialType;
		}

	}

}
