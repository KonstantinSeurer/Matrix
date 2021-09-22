/*
 * Mouse.cpp
 *
 *  Created on: Jul 31, 2021
 *      Author: konstantin
 */

#include "Mouse.h"

namespace matrix
{

namespace input
{

bool MouseState::isButtonPressed(MouseButton button) const
{
	return pressedButtons & (1 << (u32) button);
}

void MouseState::setButtonPressed(MouseButton button, bool pressed)
{
	if (pressed)
	{
		pressedButtons |= (1 << (u32) button);
	}
	else
	{
		pressedButtons &= ~(1 << (u32) button);
	}
}

}

}
