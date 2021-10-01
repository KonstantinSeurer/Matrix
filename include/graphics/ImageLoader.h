/*
 * ImageLoader.h
 *
 *  Created on: Jul 24, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_IMAGELOADER_H_
#define GRAPHICS_IMAGELOADER_H_

#include "Device.h"
#include "Image.h"
#include "../RL.h"

namespace matrix
{

namespace graphics
{

Ref<Image2DData> loadImage(Ref<DeviceInstance> device, const RL &rl);
Ref<Image2DData> loadImage(Ref<DeviceInstance> device, u8 *data, u32 size);

}

}

#endif /* GRAPHICS_IMAGELOADER_H_ */
