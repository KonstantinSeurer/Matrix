/*
 * Image.cpp
 *
 *  Created on: Jun 29, 2021
 *      Author: konstantin
 */

#include "Image.h"

namespace matrix
{

namespace graphics
{

void* Image2DAccessor::getPixelAddress(u32 x, u32 y) const
{
	return data + (x + y * width) * pixelSize;
}

void Image2DAccessor::setUnorm8(u32 x, u32 y, u8 r)
{
	u8 *address = (u8*) getPixelAddress(x, y);

	address[0] = r;
}

void Image2DAccessor::setUnorm8(u32 x, u32 y, u8 r, u8 g)
{
	u8 *address = (u8*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
}

void Image2DAccessor::setUnorm8(u32 x, u32 y, u8 r, u8 g, u8 b)
{
	u8 *address = (u8*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
	address[2] = b;
}

void Image2DAccessor::setUnorm8(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a)
{
	u8 *address = (u8*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
	address[2] = b;
	address[3] = a;
}

ImageUsage ImageUsage::NONE = 0;
ImageUsage ImageUsage::PRESENT = 1;
ImageUsage ImageUsage::SAMPLED = 2;
ImageUsage ImageUsage::ACCESS = 4;
ImageUsage ImageUsage::TRANSFER_SOURCE = 8;
ImageUsage ImageUsage::TRANSFER_DESTINATION = 16;
ImageUsage ImageUsage::COLOR_ATTACHMENT = 32;
ImageUsage ImageUsage::DEPTH_ATTACHMENT = 64;
ImageUsage ImageUsage::STENCIL_ATTACHMENT = 128;
ImageUsage ImageUsage::STORAGE = 256;
ImageUsage ImageUsage::ALL = 0xFFFFFFFF;

Image2DData::Image2DData(u32 width, u32 height, Ref<const ImageFormat> format, void *data) :
		width(width), height(height), format(format), data(data), pixelSize(
				format->componentSize * format->componentCount)
{
}

Image2DData::~Image2DData()
{
	delete data;
}

void* Image2DData::getPixelAddress(u32 x, u32 y) const
{
	return data + (x + y * width) * pixelSize;
}

void Image2DData::getUnorm8(u32 x, u32 y, u8 &r) const
{
	u8 *address = (u8*) getPixelAddress(x, y);

	r = address[0];
}

void Image2DData::getUnorm8(u32 x, u32 y, u8 &r, u8 &g) const
{
	u8 *address = (u8*) getPixelAddress(x, y);

	r = address[0];
	g = address[1];
}

void Image2DData::getUnorm8(u32 x, u32 y, u8 &r, u8 &g, u8 &b) const
{
	u8 *address = (u8*) getPixelAddress(x, y);

	r = address[0];
	g = address[1];
	b = address[2];
}

void Image2DData::getUnorm8(u32 x, u32 y, u8 &r, u8 &g, u8 &b, u8 &a) const
{
	u8 *address = (u8*) getPixelAddress(x, y);

	r = address[0];
	g = address[1];
	b = address[2];
	a = address[3];
}


Image3DData::Image3DData(u32 width, u32 height, u32 length, Ref<const ImageFormat> format, void *data) :
		width(width), height(height), length(length), format(format), data(data), pixelSize(
				format->componentSize * format->componentCount)
{
}

Image3DData::~Image3DData()
{
	delete data;
}

void* Image3DData::getPixelAddress(u32 x, u32 y, u32 z) const
{
	return data + (x + y * width + z * width * height) * pixelSize;
}

void Image3DData::getUnorm8(u32 x, u32 y, u32 z, u8 &r) const
{
	u8 *address = (u8*) getPixelAddress(x, y, z);

	r = address[0];
}

void Image3DData::getUnorm8(u32 x, u32 y, u32 z, u8 &r, u8 &g) const
{
	u8 *address = (u8*) getPixelAddress(x, y, z);

	r = address[0];
	g = address[1];
}

void Image3DData::getUnorm8(u32 x, u32 y, u32 z, u8 &r, u8 &g, u8 &b) const
{
	u8 *address = (u8*) getPixelAddress(x, y, z);

	r = address[0];
	g = address[1];
	b = address[2];
}

void Image3DData::getUnorm8(u32 x, u32 y, u32 z, u8 &r, u8 &g, u8 &b, u8 &a) const
{
	u8 *address = (u8*) getPixelAddress(x, y, z);

	r = address[0];
	g = address[1];
	b = address[2];
	a = address[3];
}

}

}
