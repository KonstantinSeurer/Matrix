/*
 * XOrShiftRandomNumberGenerator.h
 *
 *  Created on: Jul 24, 2021
 *      Author: konstantin
 */

#ifndef MATH_RANDOM_XORSHIFTRANDOMNUMBERGENERATOR_H_
#define MATH_RANDOM_XORSHIFTRANDOMNUMBERGENERATOR_H_

#include "RandomNumberGenerator.h"

namespace matrix {

namespace math {

class XOrShiftRandomNumberGenerator32: public RandomNumberGenerator32 {
private:
	u32 seed;
public:
	XOrShiftRandomNumberGenerator32(u32 seed) :
			seed(seed) {
	}

	virtual u32 nextU32();
	virtual s32 nextS32();

	virtual f32 nextF32();
};

}

}

#endif /* MATH_RANDOM_XORSHIFTRANDOMNUMBERGENERATOR_H_ */
