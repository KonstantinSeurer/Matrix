/*
 * RandomNumberGenerator.h
 *
 *  Created on: Jul 24, 2021
 *      Author: konstantin
 */

#ifndef MATH_RANDOM_RANDOMNUMBERGENERATOR_H_
#define MATH_RANDOM_RANDOMNUMBERGENERATOR_H_

#include "../Base.h"

namespace matrix {

namespace math {

class RandomNumberGenerator32 {
public:
	RandomNumberGenerator32() {
	}

	virtual ~RandomNumberGenerator32() {
	}

	virtual u32 nextU32() = 0;
	virtual s32 nextS32() = 0;

	virtual f32 nextF32() = 0;
};

}

}

#endif /* MATH_RANDOM_RANDOMNUMBERGENERATOR_H_ */
