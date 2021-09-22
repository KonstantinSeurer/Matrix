/*
 * Mouse.h
 *
 *  Created on: Jul 31, 2021
 *      Author: konstantin
 */

#ifndef INPUT_MOUSE_H_
#define INPUT_MOUSE_H_

#include "../math/Vec2.h"

namespace matrix
{

namespace input
{

enum class MouseButton
{
	LEFT, RIGHT, CENTER
};

struct MouseState
{
public:
	math::Vec2 position;
	u32 pressedButtons = 0;
public:
	bool isButtonPressed(MouseButton button) const;
	void setButtonPressed(MouseButton button, bool pressed);
};

struct MouseMoveEvent : public MouseState
{
public:
	math::Vec2 movement;
};

struct MouseButtonEvent : MouseState
{
public:
	MouseButton button;
	bool pressed;
};

}

}

#endif /* INPUT_MOUSE_H_ */
