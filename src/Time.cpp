/*
 * Time.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#include <Time.h>

namespace matrix
{

	Time operator-(Time a, Time b)
	{
		return Time(a.value - b.value);
	}

	Time operator+(Time a, Time b)
	{
		return Time(a.value + b.value);
	}

}
