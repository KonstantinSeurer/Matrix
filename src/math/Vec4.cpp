/*
 * Vec4.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#include <math/Vec4.h>

namespace matrix
{

	namespace math
	{

		Vec4 operator*(Vec4 a, Vec4 b)
		{
			return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
		}

		Vec4 operator*(Vec4 a, f32 b)
		{
			return {a.x * b, a.y * b, a.z * b, a.w * b};
		}

		Vec4 operator+(Vec4 a, Vec4 b)
		{
			return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
		}

		void Vec4::operator*=(f32 f)
		{
			x *= f;
			y *= f;
			z *= f;
			w *= f;
		}

	}

}
