/*
 * Vec3.h
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#ifndef MATH_VEC3_H_
#define MATH_VEC3_H_

#include "../Base.h"

namespace matrix
{

namespace math
{

struct Vec3
{
	f32 x;
	f32 y;
	f32 z;

	Vec3() :
		x(0.0f), y(0.0f), z(0.0f)
	{
	}

	Vec3(f32 f) :
		x(f), y(f), z(f)
	{
	}

	Vec3(f32 x, f32 y, f32 z) :
		x(x), y(y), z(z)
	{
	}

	operator String() const;

	f32 lengthSquared() const;
	f32 length() const;

	void operator*=(Vec3 v);
	void operator*=(f32 f);

	void operator/=(Vec3 v);
	void operator/=(f32 f);

	void operator+=(Vec3 v);
	void operator+=(f32 f);

	void operator-=(Vec3 v);
	void operator-=(f32 f);

	static Vec3 normalize(Vec3 v);

	static f32 dot(Vec3 a, Vec3 b);
};

Vec3 operator*(Vec3 a, Vec3 b);
Vec3 operator*(Vec3 a, f32 b);

Vec3 operator/(Vec3 a, Vec3 b);
Vec3 operator/(Vec3 a, f32 b);

Vec3 operator+(Vec3 a, Vec3 b);
Vec3 operator+(Vec3 a, f32 b);

Vec3 operator-(Vec3 a, Vec3 b);
Vec3 operator-(Vec3 a, f32 b);

}

}

#endif /* MATH_VEC3_H_ */
