/*
 * Window.h
 *
 *  Created on: Jun 2, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_WINDOW_H_
#define GRAPHICS_WINDOW_H_

#include "../Base.h"
#include "../input/Mouse.h"
#include "../input/Keyboard.h"

namespace matrix {

namespace graphics {

enum class WindowType {
	WINDOWED, RESIZABLE_WINDOWED, FULLSCREEN
};

class WindowSurface {
public:
	WindowSurface() {
	}

	virtual ~WindowSurface() {
	}
};

class Window {
public:
	Window() {
	}

	virtual ~Window() {
	}

	virtual u32 getWidth() const = 0;
	virtual u32 getHeight() const = 0;

	virtual void setWidth(u32 width) = 0;
	virtual void setHeight(u32 height) = 0;

	virtual const String& getTitile() const = 0;
	virtual void setTitle(const String &title) = 0;

	virtual bool isCloseRequested() const = 0;
	virtual void update() = 0;

	virtual input::MouseState getMouseState() const = 0;
	virtual input::KeyboardState getKeyboardState() const = 0;
};

Ref<Window> createWindow(u32 width, u32 height, const String& title, WindowType type);

}

}

#endif /* GRAPHICS_WINDOW_H_ */
