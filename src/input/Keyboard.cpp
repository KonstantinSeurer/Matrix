/*
 * Keyboard.cpp
 *
 *  Created on: Aug 1, 2021
 *      Author: konstantin
 */

#include "Keyboard.h"

namespace matrix
{

namespace input
{

static u32 getBlockIndex(Key key)
{
	return (u32) key / 64;
}

static u32 getBitIndex(Key key)
{
	return (u32) key % 64;
}

bool KeyboardState::isKeyPressed(Key key) const
{
	return keyStates[getBlockIndex(key)] & (1 << getBitIndex(key));
}

void KeyboardState::setKeyPressed(Key key, bool pressed)
{
	if (pressed)
	{
		keyStates[getBlockIndex(key)] |= (1 << getBitIndex(key));
	}
	else
	{
		keyStates[getBlockIndex(key)] &= ~(1 << getBitIndex(key));
	}
}

}

}
