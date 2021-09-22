/*
 * Logging.cpp
 *
 *  Created on: Jun 5, 2021
 *      Author: konstantin
 */

#include "Logging.h"
#include <iostream>

namespace matrix {

void out(const String& text) {
	std::cout << text << std::endl;
}

void err(const String& text) {
	std::cerr << text << std::endl;
}

}
