/*
 * GraphicsDevice.h
 *
 *  Created on: Jun 3, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_DEVICE_H_
#define GRAPHICS_DEVICE_H_

#include <Base.h>
#include "RenderPass.h"
#include "Window.h"
#include "Swapchain.h"
#include "FrameBuffer.h"
#include "CommandBuffer.h"
#include "Buffer.h"
#include "DescriptorSet.h"
#include "Fence.h"
#include "AccelerationStructure.h"

namespace matrix
{

	namespace graphics
	{

		class DeviceInstance
		{
		protected:
			Array<Function<void()>> freeFunctions;
			Ref<Sampler2D> colorSampler = null;

		public:
			DeviceInstance()
			{
			}

			virtual ~DeviceInstance()
			{
				for (auto &freeFunction : freeFunctions)
				{
					freeFunction();
				}
				freeFunctions.clear();
			}

			void addFreeFunction(Function<void()> function)
			{
				freeFunctions.push_back(function);
			}

			virtual Ref<Swapchain> createSwapchain(Ref<WindowSurface> surface) = 0;

			virtual Ref<RenderPass> createRenderpass(const Array<Attachment> &attachments, const Array<Subpass> &subpasses) = 0;

			virtual Ref<FrameBuffer> createFrameBuffer(u32 width, u32 height, Ref<RenderPass> renderPass, const Array<Ref<ImageView2D>> &imageViews) = 0;

			virtual Ref<CommandBuffer> createCommandBuffer() = 0;

			virtual Ref<GraphicsPipeline> createGraphicsPipeline(Ref<GraphicsPipelineSource> source, Ref<RenderPass> renderPass, u32 subpass) = 0;

			virtual Ref<ComputePipeline> createComputePipeline(Ref<ComputePipelineSource> source) = 0;

			virtual Ref<StructBuffer> createStructBuffer(Ref<const StructType> source, const Array<Ref<const StructType>> &structs, BufferLayout layout,
														 StructBufferType type, MemoryType memory, u32 unsizedArrayLength = 0) = 0;

			virtual Ref<ArrayBuffer> createArrayBuffer(Ref<const DataType> elementType, u32 elementCount, ArrayBufferType type, MemoryType memory) = 0;

			Ref<ArrayBuffer> createArrayBuffer(const Array<VariableSource> &vertexInputs, u32 elementCount, ArrayBufferType type, MemoryType memory);

			virtual Ref<DescriptorSet> createDescriptorSet(Ref<const DescriptorSetSource> source) = 0;

			virtual Ref<Image2D> createImage2D(u32 width, u32 height, u32 levels, Ref<const ImageFormat> format, ImageUsage usage) = 0;

			Ref<Image2D> createImage2D(const Image2DData &data, ImageUsage usage);

			virtual Ref<ImageView2D> createImageView2D(Ref<Image2D> image, u32 baseLevel, u32 levelCount) = 0;

			virtual Ref<Sampler2D> createSampler2D(SamplingMode samplingMode, SamplingMode levelSelectionMode, WrapMode xWrappingMode, WrapMode yWrappingMode) = 0;

			virtual Ref<Image3D> createImage3D(u32 width, u32 height, u32 length, u32 levels, Ref<const ImageFormat> format, ImageUsage usage) = 0;

			Ref<Image3D> createImage3D(const Image3DData &data, ImageUsage usage);

			virtual Ref<ImageView3D> createImageView3D(Ref<Image3D> image, u32 baseLevel, u32 levelCount) = 0;

			virtual Ref<Sampler3D> createSampler3D(SamplingMode samplingMode, SamplingMode levelSelectionMode, WrapMode xWrappingMode, WrapMode yWrappingMode, WrapMode zWrappingMode) = 0;

			virtual Ref<ImageFormat> createImageFormat(ImageFormatType type, u8 componentCount, u8 componentSize, bool floatingPoint) = 0;

			Ref<ImageSampler2D> createColorTexture(f32 r, f32 g, f32 b, f32 a, Ref<const ImageFormat> format);

			virtual Ref<Fence> createFence() = 0;

			virtual Ref<BottomLevelAccelerationStructure> createBottomLevelAccelerationStructure() = 0;

			virtual Ref<TopLevelAccelerationStructure> createTopLevelAccelerationStructure(u32 maxInstanceCount) = 0;

			virtual void submit(Ref<CommandBuffer> commandBuffer, Ref<Fence> fence) = 0;

			virtual void wait() = 0;
			virtual void wait(Array<Ref<Fence>> fences) = 0;
		};

		enum class DeviceType
		{
			DISCRETE,
			INTEGRATED,
			VIRTUAL,
			CPU,
			OTHER
		};

		struct DeviceFeatures
		{
			bool swapchain;
			bool raytracing;
		};

		class Device
		{
		public:
			Device()
			{
			}

			virtual ~Device()
			{
			}

			virtual Ref<DeviceInstance> createInstance() = 0;

			virtual const String &getName() const = 0;

			virtual DeviceType getType() const = 0;

			virtual DeviceFeatures getFeatures() const = 0;

			u32 getScore() const;
		};

		Ref<Device> getBestDevice(const Array<Ref<Device>> &devices);
		Ref<Device> getWorstDevice(const Array<Ref<Device>> &devices);

	}

}

#endif /* GRAPHICS_DEVICE_H_ */
