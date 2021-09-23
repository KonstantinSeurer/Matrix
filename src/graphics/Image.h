/*
 * Image.h
 *
 *  Created on: Jun 12, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_IMAGE_H_
#define GRAPHICS_IMAGE_H_

#include "../Base.h"
#include "Descriptor.h"
#include "Memory.h"

namespace matrix {

namespace graphics {

enum class ImageFormatType : u8 {
	COLOR, DEPTH, STENCIL, DEPTH_STENCIL
};

class ImageFormat {
public:
	ImageFormatType type;
	u8 componentCount;
	u8 componentSize;
	bool floatingPoint;

	virtual ~ImageFormat() {
	}
};

class Image2DAccessor {
private:
	void *data;
	u32 width;
	u32 pixelSize;
private:
	void* getPixelAddress(u32 x, u32 y) const;
public:
	Image2DAccessor(void *data, u32 width, u32 pixelSize) :
		data(data), width(width), pixelSize(pixelSize) {
	}

	void setUnorm8(u32 x, u32 y, u8 r);
	void setUnorm8(u32 x, u32 y, u8 r, u8 g);
	void setUnorm8(u32 x, u32 y, u8 r, u8 g, u8 b);
	void setUnorm8(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a);
};

enum class ImageLayout {
	SHADER_READ_ONLY,
	SHADER_STORAGE,
	TRANSFER_SOURCE,
	TRANSFER_DESTINATION,
	PRESENT,
	COLOR_ATTACHMENT,
	DEPTH_ATTACHMENT,
	STENCIL_ATTACHMENT,
	DEPTH_STENCIL_ATTACHMENT,
	UNDEFINED
};

class ImageUsage {
private:
	u32 bits;
public:
	static ImageUsage NONE;
	static ImageUsage PRESENT;
	static ImageUsage SAMPLED;
	static ImageUsage ACCESS;
	static ImageUsage TRANSFER_SOURCE;
	static ImageUsage TRANSFER_DESTINATION;
	static ImageUsage COLOR_ATTACHMENT;
	static ImageUsage DEPTH_ATTACHMENT;
	static ImageUsage STENCIL_ATTACHMENT;
	static ImageUsage STORAGE;
	static ImageUsage ALL;

	ImageUsage() :
		bits(NONE) {
	}

	ImageUsage(u32 bits) :
		bits(bits) {
	}

	ImageUsage(const ImageUsage &usage) = default;

	operator bool() const {
		return bits != 0;
	}

	ImageUsage operator|(ImageUsage usage) {
		return bits | usage.bits;
	}

	ImageUsage operator&(ImageUsage usage) {
		return bits & usage.bits;
	}

};

enum class SamplingMode {
	NEAREST, LINEAR
};

enum class WrapMode {
	CLAMP, REPEAT, MIRRORED_REPEAT
};

class Sampler2D {
private:
public:
	const SamplingMode samplingMode;
	const SamplingMode levelSelectionMode;
	const WrapMode xWrappingMode;
	const WrapMode yWrappingMode;

	Sampler2D(SamplingMode samplingMode, SamplingMode levelSelectionMode, WrapMode xWrappingMode, WrapMode yWrappingMode) :
		samplingMode(samplingMode), levelSelectionMode(levelSelectionMode), xWrappingMode(xWrappingMode), yWrappingMode(yWrappingMode) {
	}

	virtual ~Sampler2D() {
	}
};

class Image {
public:
	const ImageUsage usage;
	const Ref<const ImageFormat> format;

	Image(ImageUsage usage, Ref<const ImageFormat> format) :
		usage(usage), format(format) {
	}

	virtual ~Image() {
	}
};

class Image2D: public Image {
public:
	const u32 width;
	const u32 height;

	Image2D(ImageUsage usage, Ref<const ImageFormat> format, u32 width, u32 height) :
		Image(usage, format), width(width), height(height) {
	}

	virtual ~Image2D() {
	}

	virtual void access(Function<void(Image2DAccessor)> accessCallback, ImageLayout targetLayout) = 0;
};

class ImageView2D: public Descriptor {
public:
	const u32 width;
	const u32 height;
	const u32 baseLevel;
	const u32 levelCount;
	const Ref<Image2D> image;

	ImageView2D(u32 width, u32 height, u32 baseLevel, u32 levelCount, Ref<Image2D> image) :
		Descriptor(DescriptorType::IMAGE_2D), width(width), height(height), baseLevel(baseLevel), levelCount(levelCount), image(image) {
	}

	virtual ~ImageView2D() {
	}
};

struct ImageSampler2D: public Descriptor {
	Ref<ImageView2D> image;
	Ref<Sampler2D> sampler;

	ImageSampler2D() :
		Descriptor(DescriptorType::IMAGE_SAMPLER_2D), image(null), sampler(
		null) {
	}

	ImageSampler2D(Ref<ImageView2D> image, Ref<Sampler2D> sampler) :
		Descriptor(DescriptorType::IMAGE_SAMPLER_2D), image(image), sampler(sampler) {
	}
};

class Image2DData {
private:
	void *data;
	const u32 pixelSize;
private:
	void* getPixelAddress(u32 x, u32 y) const;
public:
	const u32 width;
	const u32 height;
	const Ref<const ImageFormat> format;

	Image2DData(u32 width, u32 height, Ref<const ImageFormat> format, void *data);

	~Image2DData();

	void getUnorm8(u32 x, u32 y, u8 &r) const;
	void getUnorm8(u32 x, u32 y, u8 &r, u8 &g) const;
	void getUnorm8(u32 x, u32 y, u8 &r, u8 &g, u8 &b) const;
	void getUnorm8(u32 x, u32 y, u8 &r, u8 &g, u8 &b, u8 &a) const;
};

class Image3DData {
private:
	void *data;
	const u32 pixelSize;
private:
	void* getPixelAddress(u32 x, u32 y, u32 z) const;
public:
	const u32 width;
	const u32 height;
	const u32 length;
	const Ref<const ImageFormat> format;

	Image3DData(u32 width, u32 height, u32 length, Ref<const ImageFormat> format, void *data);

	~Image3DData();

	void getUnorm8(u32 x, u32 y, u32 z, u8 &r) const;
	void getUnorm8(u32 x, u32 y, u32 z, u8 &r, u8 &g) const;
	void getUnorm8(u32 x, u32 y, u32 z, u8 &r, u8 &g, u8 &b) const;
	void getUnorm8(u32 x, u32 y, u32 z, u8 &r, u8 &g, u8 &b, u8 &a) const;
};

}

}

#endif /* GRAPHICS_IMAGE_H_ */
