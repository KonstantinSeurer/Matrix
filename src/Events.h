/*
 * Events.h
 *
 *  Created on: Jul 31, 2021
 *      Author: konstantin
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include "Time.h"
#include "input/Mouse.h"
#include "input/Keyboard.h"

namespace matrix
{

namespace graphics
{

class Window;

}

struct UpdateData
{
	Time time;
	Time delta;
	input::MouseMoveEvent mouse;
	input::KeyboardState keyboard;
};

}

#endif /* EVENTS_H_ */
