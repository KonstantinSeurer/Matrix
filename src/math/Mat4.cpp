/*
 * Mat4.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#include <math/Mat4.h>
#include <math/Base.h>

namespace matrix
{

	namespace math
	{

		static Mat4 flipZ = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};

		Mat4 Mat4::perspective(f32 yfov, f32 aspect, f32 near, f32 far)
		{
			f32 focalLength = 1.0f / tan(yfov * 0.5f);

			Mat4 result;
			result.x.x = focalLength / aspect;
			result.y.y = focalLength;
			result.z.z = far / (near - far);
			result.z.w = -1.0f;
			result.w.z = near * result.z.z;
			result.w.w = 0.0f;
			return flipZ * result;
		}

		Mat4 Mat4::translation(Vec3 translation)
		{
			Mat4 result;
			result.w.x = translation.x;
			result.w.y = translation.y;
			result.w.z = translation.z;
			return result;
		}

		f32 Mat4::determinant() const
		{
			return x.x * ((y.y * z.z * w.w + y.z * z.w * w.y + y.w * z.y * w.z) - y.w * z.z * w.y - y.y * z.w * w.z - y.z * z.y * w.w) - x.y * ((y.x * z.z * w.w + y.z * z.w * w.x + y.w * z.x * w.z) - y.w * z.z * w.x - y.x * z.w * w.z - y.z * z.x * w.w) + x.z * ((y.x * z.y * w.w + y.y * z.w * w.x + y.w * z.x * w.y) - y.w * z.y * w.x - y.x * z.w * w.y - y.y * z.x * w.w) - x.w * ((y.x * z.y * w.z + y.y * z.z * w.x + y.z * z.x * w.y) - y.z * z.y * w.x - y.x * z.z * w.y - y.y * z.x * w.z);
		}

		f32 determinant3x3(f32 t00, f32 t01, f32 t02, f32 t10, f32 t11, f32 t12, f32 t20, f32 t21, f32 t22)
		{
			return t00 * (t11 * t22 - t12 * t21) + t01 * (t12 * t20 - t10 * t22) + t02 * (t10 * t21 - t11 * t20);
		}

		Mat4 Mat4::inverse(const Mat4 &src)
		{
			f32 determinant = src.determinant();

			if (determinant != 0.0f)
			{
				f32 invDeterminant = 1.0f / determinant;
				Mat4 result;

				result.x.x = determinant3x3(src.y.y, src.y.z, src.y.w, src.z.y, src.z.z, src.z.w, src.w.y, src.w.z, src.w.w) * invDeterminant;
				result.y.x = -determinant3x3(src.y.x, src.y.z, src.y.w, src.z.x, src.z.z, src.z.w, src.w.x, src.w.z, src.w.w) * invDeterminant;
				result.z.x = determinant3x3(src.y.x, src.y.y, src.y.w, src.z.x, src.z.y, src.z.w, src.w.x, src.w.y, src.w.w) * invDeterminant;
				result.w.x = -determinant3x3(src.y.x, src.y.y, src.y.z, src.z.x, src.z.y, src.z.z, src.w.x, src.w.y, src.w.z) * invDeterminant;

				result.x.y = -determinant3x3(src.x.y, src.x.z, src.x.w, src.z.y, src.z.z, src.z.w, src.w.y, src.w.z, src.w.w) * invDeterminant;
				result.y.y = determinant3x3(src.x.x, src.x.z, src.x.w, src.z.x, src.z.z, src.z.w, src.w.x, src.w.z, src.w.w) * invDeterminant;
				result.z.y = -determinant3x3(src.x.x, src.x.y, src.x.w, src.z.x, src.z.y, src.z.w, src.w.x, src.w.y, src.w.w) * invDeterminant;
				result.w.y = determinant3x3(src.x.x, src.x.y, src.x.z, src.z.x, src.z.y, src.z.z, src.w.x, src.w.y, src.w.z) * invDeterminant;

				result.x.z = determinant3x3(src.x.y, src.x.z, src.x.w, src.y.y, src.y.z, src.y.w, src.w.y, src.w.z, src.w.w) * invDeterminant;
				result.y.z = -determinant3x3(src.x.x, src.x.z, src.x.w, src.y.x, src.y.z, src.y.w, src.w.x, src.w.z, src.w.w) * invDeterminant;
				result.z.z = determinant3x3(src.x.x, src.x.y, src.x.w, src.y.x, src.y.y, src.y.w, src.w.x, src.w.y, src.w.w) * invDeterminant;
				result.w.z = -determinant3x3(src.x.x, src.x.y, src.x.z, src.y.x, src.y.y, src.y.z, src.w.x, src.w.y, src.w.z) * invDeterminant;

				result.x.w = -determinant3x3(src.x.y, src.x.z, src.x.w, src.y.y, src.y.z, src.y.w, src.z.y, src.z.z, src.z.w) * invDeterminant;
				result.y.w = determinant3x3(src.x.x, src.x.z, src.x.w, src.y.x, src.y.z, src.y.w, src.z.x, src.z.z, src.z.w) * invDeterminant;
				result.z.w = -determinant3x3(src.x.x, src.x.y, src.x.w, src.y.x, src.y.y, src.y.w, src.z.x, src.z.y, src.z.w) * invDeterminant;
				result.w.w = determinant3x3(src.x.x, src.x.y, src.x.z, src.y.x, src.y.y, src.y.z, src.z.x, src.z.y, src.z.z) * invDeterminant;

				return result;
			}
			return Mat4();
		}

		Vec4 operator*(Vec4 a, const Mat4 &b)
		{
			return b.x * a.x + b.y * a.y + b.z * a.z + b.w * a.w;
		}

		Mat4 operator*(const Mat4 &a, const Mat4 &b)
		{
			return {a.x * b, a.y * b, a.z * b, a.w * b};
		}

	}

}
