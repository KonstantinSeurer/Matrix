/*
 * GraphicsDevice.cpp
 *
 *  Created on: Jun 5, 2021
 *      Author: konstantin
 */

#include "Device.h"
#include "../Logging.h"

namespace matrix
{

namespace graphics
{

u32 Device::getScore() const
{
	u32 score = 0;

	switch (getType())
	{
		case DeviceType::DISCRETE:
			score += 3;
			break;
		case DeviceType::INTEGRATED:
			score += 2;
			break;
		case DeviceType::VIRTUAL:
			score += 1;
			break;
		case DeviceType::CPU:
			score += 0;
			break;
		default:
			break;
	}

	if (getFeatures().raytracing)
	{
		score += 10;
	}

	return score;
}

static Ref<Device> selectDevice(const Array<Ref<Device>> &devices, s32 scoreFactor)
{
	if (devices.empty())
	{
		return null;
	}

	Ref<Device> maxDevice = devices[0];
	s32 maxScore = maxDevice->getScore();
	for (u32 i = 1; i < devices.size(); i++)
	{
		Ref<Device> device = devices[i];
		s32 score = device->getScore() * scoreFactor;

		if (score > maxScore)
		{
			maxScore = score;
			maxDevice = device;
		}
	}

	return maxDevice;
}

Ref<Device> getBestDevice(const Array<Ref<Device>> &devices)
{
	return selectDevice(devices, 1);
}

Ref<Device> getWorstDevice(const Array<Ref<Device>> &devices)
{
	return selectDevice(devices, -1);
}

Ref<ArrayBuffer> DeviceInstance::createArrayBuffer(const Array<VariableSource> &vertexInputs, u32 elementCount, ArrayBufferType type, MemoryType memory)
{
	Ref<StructType> input = allocate<StructType>("vertexShader.inputs", vertexInputs);
	return createArrayBuffer(input, elementCount, type, memory);
}

// TODO: format independent copy

Ref<Image2D> DeviceInstance::createImage2D(const Image2DData &data, ImageUsage usage)
{
	Ref<Image2D> image = createImage2D(data.width, data.height, 1, data.format, usage | ImageUsage::ACCESS);

	image->access([&data](Image2DData accessor)
	{
		MATRIX_ASSERT(accessor.getDataSize() == data.getDataSize())
		memcpy(accessor.getData(), data.getData(), data.getDataSize());
	}, ImageLayout::SHADER_READ_ONLY);

	return image;
}

Ref<Image3D> DeviceInstance::createImage3D(const Image3DData &data, ImageUsage usage)
{
	Ref<Image3D> image = createImage3D(data.width, data.height, data.length, 1, data.format, usage | ImageUsage::ACCESS);

	image->access([&data](Image3DData accessor)
	{
		MATRIX_ASSERT(accessor.getDataSize() == data.getDataSize())
		memcpy(accessor.getData(), data.getData(), data.getDataSize());
	}, ImageLayout::SHADER_READ_ONLY);

	return image;
}

Ref<ImageSampler2D> DeviceInstance::createColorTexture(f32 r, f32 g, f32 b, f32 a, Ref<const ImageFormat> format)
{
	if (!colorSampler)
	{
		colorSampler = createSampler2D(SamplingMode::NEAREST, SamplingMode::NEAREST, WrapMode::CLAMP, WrapMode::CLAMP);
	}

	Ref<Image2D> image = createImage2D(1, 1, 1, format, ImageUsage::SAMPLED | ImageUsage::ACCESS);
	image->access([&](Image2DData accessor)
	{
		if (!format->floatingPoint)
		{
			if (format->componentSize == 1)
			{
				accessor.setUnorm8(0, 0, (u8) (r * 255.0f), (u8) (g * 255.0f), (u8) (b * 255.0f), (u8) (a * 255.0f));
			}
		}
	}, ImageLayout::SHADER_READ_ONLY);

	Ref<ImageView2D> imageView = createImageView2D(image, 0, 1);

	return allocate<ImageSampler2D>(imageView, colorSampler);
}

}

}
