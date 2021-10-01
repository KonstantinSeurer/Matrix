/*
 * Vec2.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#include <math/Vec2.h>

namespace matrix
{

	namespace math
	{

		void Vec2::operator+=(Vec2 v)
		{
			*this = *this + v;
		}

		void Vec2::operator-=(Vec2 v)
		{
			*this = *this - v;
		}

		Vec2 operator+(Vec2 a, Vec2 b)
		{
			return {a.x + b.x, a.y + b.y};
		}

		Vec2 operator-(Vec2 a, Vec2 b)
		{
			return {a.x - b.x, a.y - b.y};
		}

		Vec2 operator*(Vec2 a, Vec2 b)
		{
			return {a.x * b.x, a.y * b.y};
		}

		Vec2 operator/(Vec2 a, Vec2 b)
		{
			return {a.x / b.x, a.y / b.y};
		}

	}

}
