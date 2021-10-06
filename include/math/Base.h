/*
 * Math.h
 *
 *  Created on: Jun 12, 2021
 *      Author: konstantin
 */

#ifndef MATH_BASE_H_
#define MATH_BASE_H_

#include <Base.h>

namespace matrix
{

	namespace math
	{

		template <typename T>
		T min(T a, T b)
		{
			return a < b ? a : b;
		}

		template <typename T>
		T max(T a, T b)
		{
			return a > b ? a : b;
		}

		template <typename T>
		T clamp(T value, T min, T max)
		{
			if (value < min)
			{
				return min;
			}
			if (value > max)
			{
				return max;
			}
			return value;
		}

#define PI 3.141592654

		f32 radians(f32 degrees);

		f32 sin(f32 x);
		f32 cos(f32 x);
		f32 tan(f32 x);

		f32 fract(f32 f);
		f64 fract(f64 f);

	}

}

#endif /* MATH_BASE_H_ */
