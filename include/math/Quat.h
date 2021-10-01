/*
 * Quat.h
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#ifndef MATH_QUAT_H_
#define MATH_QUAT_H_

#include "Base.h"
#include "Vec3.h"
#include "Mat3.h"

namespace matrix {

namespace math {

struct Quat {
	f32 a;
	f32 b;
	f32 c;
	f32 d;

	Quat() :
			a(1.0f), b(0.0f), c(0.0f), d(0.0f) {
	}

	Quat(f32 a, f32 b, f32 c, f32 d) :
			a(a), b(b), c(c), d(d) {
	}

	Quat(Vec3 axis, f32 rotation);

	operator Mat3() const;

	void operator *=(Quat q);

	Quat& normalize();
};

Quat operator *(Quat a, Quat b);

}

}

#endif /* MATH_QUAT_H_ */
