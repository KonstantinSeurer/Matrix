/*
 * Material.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#include <scene/Material.h>

namespace matrix
{

	using namespace graphics;

	namespace scene
	{

		static UnorderedMap<PrimitiveTypeType, u32> floatAttributeSizes = {{PrimitiveTypeType::FLOAT, 1}, {PrimitiveTypeType::VEC2, 2}, {PrimitiveTypeType::VEC3, 3}, {PrimitiveTypeType::VEC4, 4}, {PrimitiveTypeType::MAT2, 4}, {PrimitiveTypeType::MAT3, 9}, {PrimitiveTypeType::MAT4, 16}, {PrimitiveTypeType::INT, 0}};

		static UnorderedMap<PrimitiveTypeType, u32> intAttributeSizes = {{PrimitiveTypeType::FLOAT, 0}, {PrimitiveTypeType::VEC2, 0}, {PrimitiveTypeType::VEC3, 0}, {PrimitiveTypeType::VEC4, 0}, {PrimitiveTypeType::MAT2, 0}, {PrimitiveTypeType::MAT3, 0}, {PrimitiveTypeType::MAT4, 0}, {PrimitiveTypeType::INT, 1}};

		void MaterialType::addAttribute(const Pair<PrimitiveTypeType, String> &attribute)
		{
			attributes.push_back(attribute);

			u32 floatSize = floatAttributeSizes[attribute.first];
			if (floatSize)
			{
				offsets[attribute.second] = floatAttributeCount;
			}

			u32 intSize = intAttributeSizes[attribute.first];
			if (intSize)
			{
				offsets[attribute.second] = intAttributeCount;
			}

			floatAttributeCount += floatSize;
			intAttributeCount += intSize;
		}

		void MaterialType::addDescriptor(const Pair<DescriptorType, String> &descriptor)
		{
			descriptors.push_back(descriptor);
			switch (descriptor.first)
			{
			case DescriptorType::IMAGE_SAMPLER_2D:
				offsets[descriptor.second] = imageSampler2DCount;
				imageSampler2DCount++;
				break;
			case DescriptorType::IMAGE_SAMPLER_3D:
				offsets[descriptor.second] = imageSampler3DCount;
				imageSampler3DCount++;
				break;
			default:
				break;
			}
		}

		Material::Material(const MaterialType *type) : type(type)
		{
			imageSamplers2D.resize(type->getImageSampler2DCount());
			imageSamplers3D.resize(type->getImageSampler3DCount());
			ints.resize(type->getIntAttributeCount());
			floats.resize(type->getFloatAttributeCount());
		}

		s32 *Material::getIntPointer(const String &name)
		{
			return &ints.data()[type->getOffset(name)];
		}

		f32 *Material::getFloatPointer(const String &name)
		{
			return &floats.data()[type->getOffset(name)];
		}

		Material &Material::setImageSampler2D(const String &name, Ref<ImageSampler2D> value)
		{
			imageSamplers2D[type->getOffset(name)] = value;
			return *this;
		}

		Material &Material::setImageSampler3D(const String &name, Ref<graphics::ImageSampler3D> value)
		{
			imageSamplers3D[type->getOffset(name)] = value;
			return *this;
		}

		Material &Material::setFloat(const String &name, f32 value)
		{
			f32 *data = getFloatPointer(name);

			data[0] = value;

			return *this;
		}

		Material &Material::setVec2(const String &name, math::Vec2 value)
		{
			f32 *data = getFloatPointer(name);

			data[0] = value.x;
			data[1] = value.y;

			return *this;
		}

		Material &Material::setVec3(const String &name, math::Vec3 value)
		{
			f32 *data = getFloatPointer(name);

			data[0] = value.x;
			data[1] = value.y;
			data[2] = value.z;

			return *this;
		}

		Material &Material::setVec4(const String &name, math::Vec4 value)
		{
			f32 *data = getFloatPointer(name);

			data[0] = value.x;
			data[1] = value.y;
			data[2] = value.z;
			data[3] = value.w;

			return *this;
		}

		Material &Material::setMat2(const String &name, math::Mat2 value)
		{
			f32 *data = getFloatPointer(name);

			data[0] = value.x.x;
			data[1] = value.x.y;

			data[2] = value.y.x;
			data[3] = value.y.y;

			return *this;
		}

		Material &Material::setMat3(const String &name, math::Mat3 value)
		{
			f32 *data = getFloatPointer(name);

			data[0] = value.x.x;
			data[1] = value.x.y;
			data[2] = value.x.z;

			data[3] = value.y.x;
			data[4] = value.y.y;
			data[5] = value.y.z;

			data[6] = value.z.x;
			data[7] = value.z.y;
			data[8] = value.z.z;

			return *this;
		}

		Material &Material::setMat4(const String &name, math::Mat4 value)
		{
			f32 *data = getFloatPointer(name);

			data[0] = value.x.x;
			data[1] = value.x.y;
			data[2] = value.x.z;
			data[3] = value.x.w;

			data[4] = value.y.x;
			data[5] = value.y.y;
			data[6] = value.y.z;
			data[7] = value.y.w;

			data[8] = value.z.x;
			data[9] = value.z.y;
			data[10] = value.z.z;
			data[11] = value.z.w;

			data[12] = value.w.x;
			data[13] = value.w.y;
			data[14] = value.w.z;
			data[15] = value.w.w;

			return *this;
		}

		Material &Material::setInt(const String &name, s32 value)
		{
			s32 *data = getIntPointer(name);

			data[0] = value;

			return *this;
		}

	}

}
