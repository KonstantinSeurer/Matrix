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

Image2DData::~Image2DData()
{
	if (transferOwnership)
	{
		delete data;
	}
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

void Image2DData::setUnorm8(u32 x, u32 y, u8 r)
{
	u8 *address = (u8*) getPixelAddress(x, y);

	address[0] = r;
}

void Image2DData::setUnorm8(u32 x, u32 y, u8 r, u8 g)
{
	u8 *address = (u8*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
}

void Image2DData::setUnorm8(u32 x, u32 y, u8 r, u8 g, u8 b)
{
	u8 *address = (u8*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
	address[2] = b;
}

void Image2DData::setUnorm8(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a)
{
	u8 *address = (u8*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
	address[2] = b;
	address[3] = a;
}

void Image2DData::setFloat32(u32 x, u32 y, f32 r)
{
	f32 *address = (f32*) getPixelAddress(x, y);

	address[0] = r;
}

void Image2DData::setFloat32(u32 x, u32 y, f32 r, f32 g)
{
	f32 *address = (f32*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
}

void Image2DData::setFloat32(u32 x, u32 y, f32 r, f32 g, f32 b)
{
	f32 *address = (f32*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
	address[2] = b;
}

void Image2DData::setFloat32(u32 x, u32 y, f32 r, f32 g, f32 b, f32 a)
{
	f32 *address = (f32*) getPixelAddress(x, y);

	address[0] = r;
	address[1] = g;
	address[2] = b;
	address[3] = a;
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

void Image3DData::setUnorm8(u32 x, u32 y, u32 z, u8 r)
{
	u8 *address = (u8*) getPixelAddress(x, y, z);

	address[0] = r;
}

void Image3DData::setUnorm8(u32 x, u32 y, u32 z, u8 r, u8 g)
{
	u8 *address = (u8*) getPixelAddress(x, y, z);

	address[0] = r;
	address[1] = g;
}

void Image3DData::setUnorm8(u32 x, u32 y, u32 z, u8 r, u8 g, u8 b)
{
	u8 *address = (u8*) getPixelAddress(x, y, z);

	address[0] = r;
	address[1] = g;
	address[2] = b;
}

void Image3DData::setUnorm8(u32 x, u32 y, u32 z, u8 r, u8 g, u8 b, u8 a)
{
	u8 *address = (u8*) getPixelAddress(x, y, z);

	address[0] = r;
	address[1] = g;
	address[2] = b;
	address[3] = a;
}

void Image3DData::setFloat32(u32 x, u32 y, u32 z, f32 r)
{
	f32 *address = (f32*) getPixelAddress(x, y, z);

	address[0] = r;
}

void Image3DData::setFloat32(u32 x, u32 y, u32 z, f32 r, f32 g)
{
	f32 *address = (f32*) getPixelAddress(x, y, z);

	address[0] = r;
	address[1] = g;
}

void Image3DData::setFloat32(u32 x, u32 y, u32 z, f32 r, f32 g, f32 b)
{
	f32 *address = (f32*) getPixelAddress(x, y, z);

	address[0] = r;
	address[1] = g;
	address[2] = b;
}

void Image3DData::setFloat32(u32 x, u32 y, u32 z, f32 r, f32 g, f32 b, f32 a)
{
	f32 *address = (f32*) getPixelAddress(x, y, z);

	address[0] = r;
	address[1] = g;
	address[2] = b;
	address[3] = a;
}

}

}
