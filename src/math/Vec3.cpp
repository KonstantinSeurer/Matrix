/*
 * Vec3.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#include <math/Vec3.h>

namespace matrix
{

	namespace math
	{

		Vec3::operator String() const
		{
			return "(" + toString(x) + " " + toString(y) + " " + toString(z) + ")";
		}

		f32 Vec3::lengthSquared() const
		{
			return x * x + y * y + z * z;
		}

		f32 Vec3::length() const
		{
			return sqrtf(lengthSquared());
		}

		void Vec3::operator*=(Vec3 v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
		}

		void Vec3::operator*=(f32 f)
		{
			x *= f;
			y *= f;
			z *= f;
		}

		void Vec3::operator/=(Vec3 v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
		}

		void Vec3::operator/=(f32 f)
		{
			f32 invF = 1.0f / f;
			x *= invF;
			y *= invF;
			z *= invF;
		}

		void Vec3::operator+=(Vec3 v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}

		void Vec3::operator+=(f32 f)
		{
			x += f;
			y += f;
			z += f;
		}

		void Vec3::operator-=(Vec3 v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		void Vec3::operator-=(f32 f)
		{
			x -= f;
			y -= f;
			z -= f;
		}

		Vec3 Vec3::normalize(Vec3 v)
		{
			return v / v.length();
		}

		f32 Vec3::dot(Vec3 a, Vec3 b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		Vec3 operator*(Vec3 a, Vec3 b)
		{
			return {a.x * b.x, a.y * b.y, a.z * b.z};
		}

		Vec3 operator*(Vec3 a, f32 b)
		{
			return {a.x * b, a.y * b, a.z * b};
		}

		Vec3 operator/(Vec3 a, Vec3 b)
		{
			return {a.x / b.x, a.y / b.y, a.z / b.z};
		}

		Vec3 operator/(Vec3 a, f32 b)
		{
			f32 invB = 1.0f / b;
			return {a.x * invB, a.y * invB, a.z * invB};
		}

		Vec3 operator+(Vec3 a, Vec3 b)
		{
			return {a.x + b.x, a.y + b.y, a.z + b.z};
		}

		Vec3 operator+(Vec3 a, f32 b)
		{
			return {a.x + b, a.y + b, a.z + b};
		}

		Vec3 operator-(Vec3 a, Vec3 b)
		{
			return {a.x - b.x, a.y - b.y, a.z - b.z};
		}

		Vec3 operator-(Vec3 a, f32 b)
		{
			return {a.x - b, a.y - b, a.z - b};
		}

	}

}
