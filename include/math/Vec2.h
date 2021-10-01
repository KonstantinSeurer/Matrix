/*
 * Vec2.h
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#ifndef MATH_VEC2_H_
#define MATH_VEC2_H_

#include "../Base.h"

namespace matrix
{

namespace math
{

struct Vec2
{
	f32 x;
	f32 y;

	Vec2() :
	x(0.0f), y(0.0f)
	{
	}

	Vec2(f32 f) :
	x(f), y(f)
	{
	}

	Vec2(f32 x, f32 y) :
	x(x), y(y)
	{
	}

	void operator+=(Vec2 v);
	void operator-=(Vec2 v);
};

Vec2 operator+(Vec2 a, Vec2 b);
Vec2 operator-(Vec2 a, Vec2 b);
Vec2 operator*(Vec2 a, Vec2 b);
Vec2 operator/(Vec2 a, Vec2 b);

}

}

#endif /* MATH_VEC2_H_ */
