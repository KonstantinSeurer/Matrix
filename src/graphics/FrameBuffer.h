/*
 * FrameBuffer.h
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_FRAMEBUFFER_H_
#define GRAPHICS_FRAMEBUFFER_H_

#include "../Base.h"

namespace matrix {

namespace graphics {

class FrameBuffer {
protected:
	u32 width;
	u32 height;
public:
	virtual ~FrameBuffer() {
	}

	u32 getWidth() const {
		return width;
	}

	u32 getHeight() const {
		return height;
	}
};

}

}

#endif /* GRAPHICS_FRAMEBUFFER_H_ */
