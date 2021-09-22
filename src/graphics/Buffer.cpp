/*
 * Accessor.cpp
 *
 *  Created on: Jun 18, 2021
 *      Author: konstantin
 */

#include "Buffer.h"
#include "../Logging.h"

namespace matrix
{

namespace graphics
{

void StructAccessor::setInt(const String &name, s32 value)
{
	*((s32*) getMemberAddress(name)) = value;
}

void StructAccessor::setUInt(const String &name, u32 value)
{
	*((u32*) getMemberAddress(name)) = value;
}

void StructAccessor::setFloat(const String &name, f32 value)
{
	*((f32*) getMemberAddress(name)) = value;
}

void StructAccessor::setVec2(const String &name, math::Vec2 value)
{
	f32 *ptr = (f32*) getMemberAddress(name);

	ptr[0] = value.x;
	ptr[1] = value.y;
}

void StructAccessor::setVec3(const String &name, math::Vec3 value)
{
	f32 *ptr = (f32*) getMemberAddress(name);

	ptr[0] = value.x;
	ptr[1] = value.y;
	ptr[2] = value.z;
}

void StructAccessor::setVec4(const String &name, math::Vec4 value)
{
	f32 *ptr = (f32*) getMemberAddress(name);

	ptr[0] = value.x;
	ptr[1] = value.y;
	ptr[2] = value.z;
	ptr[3] = value.w;
}

void StructAccessor::setMat2(const String &name, math::Mat2 value)
{
	f32 *ptr = (f32*) getMemberAddress(name);

	ptr[0] = value.x.x;
	ptr[1] = value.x.y;

	ptr[2] = value.y.x;
	ptr[3] = value.y.y;
}

void StructAccessor::setMat3(const String &name, math::Mat3 value)
{
	f32 *ptr = (f32*) getMemberAddress(name);

	ptr[0] = value.x.x;
	ptr[1] = value.x.y;
	ptr[2] = value.x.z;

	ptr[3] = value.y.x;
	ptr[4] = value.y.y;
	ptr[5] = value.y.z;

	ptr[6] = value.z.x;
	ptr[7] = value.z.y;
	ptr[8] = value.z.z;
}

void StructAccessor::setMat4(const String &name, math::Mat4 value)
{
	f32 *ptr = (f32*) getMemberAddress(name);

	ptr[0] = value.x.x;
	ptr[1] = value.x.y;
	ptr[2] = value.x.z;
	ptr[3] = value.x.w;

	ptr[4] = value.y.x;
	ptr[5] = value.y.y;
	ptr[6] = value.y.z;
	ptr[7] = value.y.w;

	ptr[8] = value.z.x;
	ptr[9] = value.z.y;
	ptr[10] = value.z.z;
	ptr[11] = value.z.w;

	ptr[12] = value.w.x;
	ptr[13] = value.w.y;
	ptr[14] = value.w.z;
	ptr[15] = value.w.w;
}

ArrayAccessor StructAccessor::getArray(const String &name)
{
	auto &member = ((StructAccessorBlueprint*) blueprint)->members[name];
	return ArrayAccessor((ArrayAccessorBlueprint*) member.second.get(), address + member.first);
}

StructAccessor StructAccessor::getStruct(const String &name)
{
	auto &member = ((StructAccessorBlueprint*) blueprint)->members[name];
	return StructAccessor((StructAccessorBlueprint*) member.second.get(), address + member.first);
}

void* StructAccessor::getMemberAddress(const String &name) const
{
	return address + ((StructAccessorBlueprint*) blueprint)->members[name].first;
}

void ArrayAccessor::setInt(u32 index, s32 value)
{
	*((s32*) getElementAddress(index)) = value;
}

void ArrayAccessor::setFloat(u32 index, f32 value)
{
	*((f32*) getElementAddress(index)) = value;
}

void ArrayAccessor::setVec2(u32 index, math::Vec2 value)
{
	f32 *ptr = (f32*) getElementAddress(index);

	ptr[0] = value.x;
	ptr[1] = value.y;
}

void ArrayAccessor::setVec3(u32 index, math::Vec3 value)
{
	f32 *ptr = (f32*) getElementAddress(index);

	ptr[0] = value.x;
	ptr[1] = value.y;
	ptr[2] = value.z;
}

void ArrayAccessor::setVec4(u32 index, math::Vec4 value)
{
	f32 *ptr = (f32*) getElementAddress(index);

	ptr[0] = value.x;
	ptr[1] = value.y;
	ptr[2] = value.z;
	ptr[3] = value.w;
}

void ArrayAccessor::setMat2(u32 index, math::Mat2 value)
{
	f32 *ptr = (f32*) getElementAddress(index);

	ptr[0] = value.x.x;
	ptr[1] = value.x.y;

	ptr[2] = value.y.x;
	ptr[3] = value.y.y;
}

void ArrayAccessor::setMat3(u32 index, math::Mat3 value)
{
	f32 *ptr = (f32*) getElementAddress(index);

	ptr[0] = value.x.x;
	ptr[1] = value.x.y;
	ptr[2] = value.x.z;

	ptr[3] = value.y.x;
	ptr[4] = value.y.y;
	ptr[5] = value.y.z;

	ptr[6] = value.z.x;
	ptr[7] = value.z.y;
	ptr[8] = value.z.z;
}

void ArrayAccessor::setMat4(u32 index, math::Mat4 value)
{
	f32 *ptr = (f32*) getElementAddress(index);

	ptr[0] = value.x.x;
	ptr[1] = value.x.y;
	ptr[2] = value.x.z;
	ptr[3] = value.x.w;

	ptr[4] = value.y.x;
	ptr[5] = value.y.y;
	ptr[6] = value.y.z;
	ptr[7] = value.y.w;

	ptr[8] = value.z.x;
	ptr[9] = value.z.y;
	ptr[0] = value.z.z;
	ptr[11] = value.z.w;

	ptr[12] = value.w.x;
	ptr[13] = value.w.y;
	ptr[14] = value.w.z;
	ptr[15] = value.w.w;
}

ArrayAccessor ArrayAccessor::getArray(u32 index)
{
	ArrayAccessorBlueprint *elementType = (ArrayAccessorBlueprint*) ((ArrayAccessorBlueprint*) blueprint)->elementType.get();
	return ArrayAccessor(elementType, address + index * elementType->size);
}

StructAccessor ArrayAccessor::getStruct(u32 index)
{
	StructAccessorBlueprint *elementType = (StructAccessorBlueprint*) ((ArrayAccessorBlueprint*) blueprint)->elementType.get();
	return StructAccessor(elementType, address + index * elementType->size);
}

void* ArrayAccessor::getElementAddress(u32 index) const
{
	return address + index * ((ArrayAccessorBlueprint*) blueprint)->elementType->size;
}

}

}
