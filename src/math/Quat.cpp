/*
 * Quat.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#include <math/Quat.h>

namespace matrix
{

	namespace math
	{

		Quat::Quat(Vec3 axis, f32 rotation) : a(cos(rotation * 0.5f))
		{
			f32 s = sin(rotation * 0.5f);
			b = axis.x * s;
			c = axis.y * s;
			d = axis.z * s;
		}

		Quat::operator Mat3() const
		{
			Mat3 result;
			result.x.x = a * a + b * b - c * c - d * d;
			result.x.y = 2.0 * b * c + 2.0 * a * d;
			result.x.z = 2.0 * b * d - 2.0 * a * c;
			result.y.x = 2.0 * b * c - 2.0 * a * d;
			result.y.y = a * a - b * b + c * c - d * d;
			result.y.z = 2.0 * c * d + 2.0 * a * b;
			result.z.x = 2.0 * b * d + 2.0 * a * c;
			result.z.y = 2.0 * c * d - 2.0 * a * b;
			result.z.z = a * a - b * b - c * c + d * d;
			return result;
		}

		Quat operator*(Quat a, Quat b)
		{
			Quat result;
			result.a = a.a * b.a - a.b * b.b - a.c * b.c - a.d * b.d;
			result.b = a.a * b.b + a.b * b.a + a.c * b.d - a.d * b.c;
			result.c = a.a * b.c - a.b * b.d + a.c * b.a + a.d * b.b;
			result.d = a.a * b.d + a.b * b.c - a.c * b.b + a.d * b.a;
			return result;
		}

		void Quat::operator*=(Quat q)
		{
			*this = *this * q;
		}

		Quat &Quat::normalize()
		{
			f32 invLength = 1.0f / sqrt(a * a + b * b + c * c + d + d);
			a *= invLength;
			b *= invLength;
			c *= invLength;
			d *= invLength;
			return *this;
		}

	}

}
