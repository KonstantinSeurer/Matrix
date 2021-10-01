/*
 * Mat3.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: konstantin
 */

#include <math/Mat3.h>
#include <math/Base.h>

namespace matrix
{

	namespace math
	{

		Mat3 Mat3::rotationX(f32 angle)
		{
			f32 s = sin(angle);
			f32 c = cos(angle);
			return {{1.0f, 0.0f, 0.0f}, {0.0f, c, s}, {0.0f, -s, c}};
		}

		Mat3 Mat3::rotationY(f32 angle)
		{
			f32 s = sin(angle);
			f32 c = cos(angle);
			return {{c, 0.0f, -s}, {0.0f, 1.0f, 0.0f}, {s, 0.0f, c}};
		}

		Mat3 Mat3::rotationZ(f32 angle)
		{
			f32 s = sin(angle);
			f32 c = cos(angle);
			return {{c, s, 0.0f}, {-s, c, 0.0f}, {0.0f, 0.0f, 1.0f}};
		}

	}

}
