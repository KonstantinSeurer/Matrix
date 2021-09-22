/*
 * Vec4.h
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#ifndef MATH_VEC4_H_
#define MATH_VEC4_H_

#include "../Base.h"
#include "Vec3.h"

namespace matrix {

namespace math {

struct Vec4 {
	f32 x;
	f32 y;
	f32 z;
	f32 w;

	Vec4() :
			x(0.0f), y(0.0f), z(0.0f), w(0.0f) {
	}

	Vec4(f32 f) :
			x(f), y(f), z(f), w(f) {
	}

	Vec4(f32 x, f32 y, f32 z, f32 w) :
			x(x), y(y), z(z), w(w) {
	}

	Vec4(Vec3 xyz, f32 w) :
			x(xyz.x), y(xyz.y), z(xyz.z), w(w) {
	}

	void operator*=(f32 f);
};

Vec4 operator*(Vec4 a, Vec4 b);
Vec4 operator*(Vec4 a, f32 b);

Vec4 operator+(Vec4 a, Vec4 b);

}

}

#endif /* MATH_VEC4_H_ */
