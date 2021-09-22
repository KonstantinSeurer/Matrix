/*
 * DemoSelector.cpp
 *
 *  Created on: Sep 18, 2021
 *      Author: konstantin
 */

#include "DemoSelector.h"
#include <string.h>
#include <iostream>

int main(int argc, const char **args)
{
	if (argc > 1)
	{
		const char *demoName = args[1];
		if (strcmp(demoName, "double_pendulum") == 0)
		{
			doublePendulumMain();
		}
		else if (strcmp(demoName, "sponza") == 0)
		{
			sponzaMain();
		}
		else if (strcmp(demoName, "sky") == 0)
		{
			skyMain();
		}
		else
		{
			std::cout << "Unknown demo '" << demoName << "'!" << std::endl;
			return 1;
		}
	}
	else
	{
		std::cout << "Available demos:" << std::endl << "double_pendulum" << std::endl << "sponza" << std::endl;
		;
	}

	return 0;
}
