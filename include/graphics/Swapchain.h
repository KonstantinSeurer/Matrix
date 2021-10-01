/*
 * Swapchain.h
 *
 *  Created on: Jun 7, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_SWAPCHAIN_H_
#define GRAPHICS_SWAPCHAIN_H_

#include "Image.h"
#include "../Base.h"

namespace matrix {

namespace graphics {

class Swapchain {
public:
	Swapchain() {
	}

	virtual ~Swapchain() {
	}

	virtual const Array<Ref<Image2D>>& getImages() const = 0;

	virtual u32 pollImage() = 0;
	virtual u32 getCurrentImageIndex() const = 0;

	virtual void present() = 0;
};

}

}

#endif /* GRAPHICS_SWAPCHAIN_H_ */
