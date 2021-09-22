/*
 * Material.h
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#ifndef SCENE_MATERIAL_H_
#define SCENE_MATERIAL_H_

#include "../Base.h"
#include "../graphics/Descriptor.h"
#include "../graphics/Pipeline.h"
#include "../graphics/Image.h"
#include "../math/Vec2.h"
#include "../math/Vec3.h"
#include "../math/Vec4.h"
#include "../math/Mat2.h"
#include "../math/Mat3.h"
#include "../math/Mat4.h"

namespace matrix {

namespace scene {

class MaterialType {
private:
	Array<Pair<graphics::DescriptorType, String>> descriptors;
	Array<Pair<graphics::PrimitiveTypeType, String>> attributes;
	UnorderedMap<String, u32> offsets;
	u32 floatAttributeCount = 0;
	u32 intAttributeCount = 0;
	u32 imageSampler2DCount = 0;
public:
	MaterialType() {
	}

	void addAttribute(const Pair<graphics::PrimitiveTypeType, String> &attribute);
	void addDescriptor(const Pair<graphics::DescriptorType, String> &descriptor);

	u32 getFloatAttributeCount() const {
		return floatAttributeCount;
	}

	u32 getIntAttributeCount() const {
		return intAttributeCount;
	}

	u32 getImageSampler2DCount() const {
		return imageSampler2DCount;
	}

	u32 getOffset(const String &name) const {
		return offsets.at(name);
	}
};

class Material {
private:
	const MaterialType *type;
	Array<Ref<graphics::ImageSampler2D>> imageSamplers2D;
	Array<s32> ints;
	Array<f32> floats;
private:
	s32* getIntPointer(const String &name);
	f32* getFloatPointer(const String &name);
public:
	Material(const MaterialType *type);

	Material& setImageSampler2D(const String &name, Ref<graphics::ImageSampler2D> value);

	Material& setFloat(const String &name, f32 value);
	Material& setVec2(const String &name, math::Vec2 value);
	Material& setVec3(const String &name, math::Vec3 value);
	Material& setVec4(const String &name, math::Vec4 value);
	Material& setMat2(const String &name, math::Mat2 value);
	Material& setMat3(const String &name, math::Mat3 value);
	Material& setMat4(const String &name, math::Mat4 value);
	Material& setInt(const String &name, s32 value);

	const MaterialType* getType() const {
		return type;
	}

	s32 getInt(u32 index) const {
		return ints[index];
	}

	f32 getFloat(u32 index) const {
		return floats[index];
	}

	Ref<graphics::ImageSampler2D> getImageSampler2D(u32 index) const {
		return imageSamplers2D[index];
	}
};

}

}

#endif /* SCENE_MATERIAL_H_ */
