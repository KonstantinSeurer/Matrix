/*
 * ImageLoader.cpp
 *
 *  Created on: Jul 24, 2021
 *      Author: konstantin
 */

#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace matrix {

namespace graphics {

Ref<Image2DData> loadImage(Ref<DeviceInstance> device, const RL& rl) {
	Array<u8> data;
	rl.read(data);
	return loadImage(device, data.data(), data.size());
}

Ref<Image2DData> loadImage(Ref<DeviceInstance> device, u8* data, u32 size) {
	s32 width;
	s32 height;
	s32 componentCount;
	Ref<ImageFormat> format;

	stbi_info_from_memory(data, size, &width, &height, &componentCount);

	format = device->createImageFormat(ImageFormatType::COLOR, componentCount, 1, false);

	void* pixelData = stbi_load_from_memory(data, size, &width, &height, &componentCount, format->componentCount);

	return allocate<Image2DData>(pixelData, width, height, format, true);
}

}

}
