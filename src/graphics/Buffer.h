/*
 * Buffer.h
 *
 *  Created on: Jun 23, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_BUFFER_H_
#define GRAPHICS_BUFFER_H_

#include "Memory.h"
#include "Descriptor.h"
#include "../math/Math.h"

namespace matrix {

namespace graphics {

enum class BufferLayout {
	STANDARD, PACKED, PACKED_NO_PADDING
};

class AccessorBlueprint {
public:
	u32 alignment;
	u32 size;

	virtual ~AccessorBlueprint() {
	}
};

class ArrayAccessorBlueprint: public AccessorBlueprint {
public:
	Ref<AccessorBlueprint> elementType;
	u32 length;
};

class StructAccessorBlueprint: public AccessorBlueprint {
public:
	UnorderedMap<String, Pair<u32, Ref<AccessorBlueprint>>> members;
};

class Accessor {
protected:
	const AccessorBlueprint *blueprint;
	void *address;
public:
	Accessor(const AccessorBlueprint *blueprint, void *address) :
			blueprint(blueprint), address(address) {
	}
};

class ArrayAccessor;

class StructAccessor: public Accessor {
public:
	StructAccessor(const StructAccessorBlueprint *blueprint, void *address) :
			Accessor(blueprint, address) {
	}

	void setInt(const String &name, s32 value);
	void setUInt(const String &name, u32 value);
	void setFloat(const String &name, f32 value);
	void setVec2(const String &name, math::Vec2 value);
	void setVec3(const String &name, math::Vec3 value);
	void setVec4(const String &name, math::Vec4 value);
	void setMat2(const String &name, math::Mat2 value);
	void setMat3(const String &name, math::Mat3 value);
	void setMat4(const String &name, math::Mat4 value);

	ArrayAccessor getArray(const String &name);

	StructAccessor getStruct(const String &name);

	void* getMemberAddress(const String &name) const;
};

class ArrayAccessor: public Accessor {
public:
	ArrayAccessor(const ArrayAccessorBlueprint *blueprint, void *address) :
			Accessor(blueprint, address) {
	}

	void setInt(u32 index, s32 value);
	void setFloat(u32 index, f32 value);
	void setVec2(u32 index, math::Vec2 value);
	void setVec3(u32 index, math::Vec3 value);
	void setVec4(u32 index, math::Vec4 value);
	void setMat2(u32 index, math::Mat2 value);
	void setMat3(u32 index, math::Mat3 value);
	void setMat4(u32 index, math::Mat4 value);

	ArrayAccessor getArray(u32 index);

	StructAccessor getStruct(u32 index);

	void* getElementAddress(u32 index) const;
};

enum class StructBufferType {
	UNIFORM = (u32) DescriptorType::UNIFORM_BUFFER, STORAGE = (u32) DescriptorType::STORAGE_BUFFER
};

enum class ArrayBufferType {
	VERTEX, INDEX, ACCELERATION_STRUCTURE_SOURCE
};

class Buffer {
private:
public:
	const MemoryType memoryType;

	Buffer(MemoryType memoryType) :
			memoryType(memoryType) {
	}

	virtual ~Buffer() {
	}
};

class StructBuffer: public Buffer, public Descriptor {
private:
public:
	const StructBufferType type;

	StructBuffer(StructBufferType type, MemoryType memoryType) :
			Buffer(memoryType), Descriptor((DescriptorType) type), type(type) {
	}

	virtual void access(Function<void(StructAccessor)> accessCallback) = 0;
};

class ArrayBuffer: public Buffer {
private:
public:
	const ArrayBufferType type;
	const u32 length;

	ArrayBuffer(ArrayBufferType type, MemoryType memoryType, u32 length) :
			Buffer(memoryType), type(type), length(length) {
	}

	virtual void access(Function<void(ArrayAccessor)> accessCallback) = 0;
};

}

}

#endif /* GRAPHICS_BUFFER_H_ */
