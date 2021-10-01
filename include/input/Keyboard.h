/*
 * Keyboard.h
 *
 *  Created on: Aug 1, 2021
 *      Author: konstantin
 */

#ifndef INPUT_KEYBOARD_H_
#define INPUT_KEYBOARD_H_

#include "../Base.h"

namespace matrix
{

namespace input
{

enum class Key
{
	ESCAPE,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	PRINT,
	D1,
	D2,
	D3,
	D4,
	D5,
	D6,
	D7,
	D8,
	D9,
	D0,
	TAB,
	Q,
	W,
	E,
	R,
	T,
	Z,
	U,
	I,
	O,
	P,
	PLUS,
	BACKSPACE,
	ENTER,
	CAPS,
	A,
	S,
	D,
	F,
	G,
	H,
	J,
	K,
	L,
	HASH,
	LEFT_SHIFT,
	LESS,
	Y,
	X,
	C,
	V,
	B,
	N,
	M,
	COLON,
	DOT,
	MINUS,
	RIGHT_SHIFT,
	LEFT_CONTROL,
	HOME,
	LEFT_ALT,
	SPACE,
	RIGHT_ALT,
	RIGHT_CONTROL,
	DELETE,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	N1,
	N2,
	N3,
	N4,
	N5,
	N6,
	N7,
	N8,
	N9,
	N0
};

struct KeyboardState
{
public:
	u64 keyStates[2];
public:
	KeyboardState()
	{
		keyStates[0] = 0;
		keyStates[1] = 0;
	}

	bool isKeyPressed(Key key) const;
	void setKeyPressed(Key key, bool pressed);
};

struct KeyEvent: KeyboardState
{
public:
	Key key;
	char character;
};

}

}

#endif /* INPUT_KEYBOARD_H_ */
