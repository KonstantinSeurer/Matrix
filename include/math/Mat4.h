/*
 * Mat4.h
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#ifndef MATH_MAT4_H_
#define MATH_MAT4_H_

#include "Vec4.h"
#include "Mat3.h"
#include "Base.h"

namespace matrix {

namespace math {

struct Mat4 {
	Vec4 x;
	Vec4 y;
	Vec4 z;
	Vec4 w;

	Mat4() :
			x(1.0f, 0.0f, 0.0f, 0.0f), y(0.0f, 1.0f, 0.0f, 0.0f), z(0.0f, 0.0f, 1.0f, 0.0f), w(0.0f, 0.0f, 0.0f, 1.0f) {
	}

	Mat4(const Mat3 &mat) :
			x(mat.x, 0.0f), y(mat.y, 0.0f), z(mat.z, 0.0f), w(0.0f, 0.0f, 0.0f, 1.0f) {
	}

	Mat4(Vec4 x, Vec4 y, Vec4 z, Vec4 w) :
			x(x), y(y), z(z), w(w) {
	}

	f32 determinant() const;

	static Mat4 perspective(f32 yfov, f32 aspect, f32 near, f32 far);

	static Mat4 translation(Vec3 translation);

	static Mat4 inverse(const Mat4& mat);
};

Vec4 operator*(Vec4 a, const Mat4 &b);
Mat4 operator*(const Mat4 &a, const Mat4 &b);

}

}

#endif /* MATH_MAT4_H_ */
