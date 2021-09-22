/*
 * Base.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#include "Base.h"

#include <cmath>

namespace matrix {

namespace math {

f32 radians(f32 degrees) {
	return degrees / 180.0f * PI;
}

f32 sin(f32 x) {
	return sinf(x);
}

f32 cos(f32 x) {
	return cosf(x);
}

f32 tan(f32 x) {
	return tanf(x);
}

f32 fract(f32 f) {
	return f - (s32) f;
}

f64 fract(f64 f) {
	return f - (s64) f;
}

}

}
