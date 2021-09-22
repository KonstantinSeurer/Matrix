/*
 * XOrShiftRandomNumberGenerator.cpp
 *
 *  Created on: Jul 24, 2021
 *      Author: konstantin
 */

#include "XOrShiftRandomNumberGenerator.h"

namespace matrix {

namespace math {

u32 XOrShiftRandomNumberGenerator32::nextU32() {
	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 5;
	return seed;
}

s32 XOrShiftRandomNumberGenerator32::nextS32() {
	u32 random = nextU32();
	return *((s32*) &random);
}

f32 XOrShiftRandomNumberGenerator32::nextF32() {
	u32 bits = 0b00111111100000000000000000000000 | (nextU32() >> 9);
	return *((f32*) &bits) - 1.0f;
}

}

}
