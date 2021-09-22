/*
 * Vec3i.h
 *
 *  Created on: Aug 8, 2021
 *      Author: konstantin
 */

#ifndef MATH_VEC3I_H_
#define MATH_VEC3I_H_

#include "../Base.h"

namespace matrix {

namespace math {

template<typename T> struct Vec3i {
public:
	T x;
	T y;
	T z;
public:
	Vec3i() :
		x(0), y(0), z(0) {
	}

	Vec3i(T i) :
		x(i), y(i), z(i) {
	}

	Vec3i(T x, T y, T z) :
		x(x), y(y), z(z) {
	}
};

using Vec3s32 = Vec3i<s32>;
using Vec3u32 = Vec3i<u32>;

using Vec3s64 = Vec3i<s64>;
using Vec3u64 = Vec3i<u64>;

}

}

#endif /* MATH_VEC3I_H_ */
