/*
 * GraphicsContext.h
 *
 *  Created on: Jun 3, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_CONTEXT_H_
#define GRAPHICS_CONTEXT_H_

#include "../Base.h"
#include "Device.h"
#include "Window.h"

namespace matrix {

namespace graphics {

class Context {
public:
	Context() {
	}

	virtual ~Context() {
	}

	virtual const Array<Ref<Device>>& getAvailableDevices() = 0;

	virtual Ref<WindowSurface> createSurface(Ref<Window> window) const = 0;
};

Ref<Context> createContext();

}

}

#endif /* GRAPHICS_CONTEXT_H_ */
