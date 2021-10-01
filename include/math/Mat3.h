/*
 * Mat3.h
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#ifndef MATH_MAT3_H_
#define MATH_MAT3_H_

#include "Vec3.h"

namespace matrix {

namespace math {

struct Mat3 {
	Vec3 x;
	Vec3 y;
	Vec3 z;

	Mat3() :
			x(1.0f, 0.0f, 0.0f), y(0.0f, 1.0f, 0.0f), z(0.0f, 0.0f, 1.0f) {
	}

	Mat3(Vec3 x, Vec3 y, Vec3 z) :
			x(x), y(y), z(z) {
	}

	static Mat3 rotationX(f32 angle);
	static Mat3 rotationY(f32 angle);
	static Mat3 rotationZ(f32 angle);
};

}

}

#endif /* MATH_MAT3_H_ */
