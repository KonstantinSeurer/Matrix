/*
 * VulkanBuffer.h
 *
 *  Created on: Jun 23, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANBUFFER_H_

#include <graphics/Buffer.h>
#include <graphics/Pipeline.h>
#include "VulkanUtil.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanSemaphoreChain.h"

namespace matrix
{

	namespace graphics
	{

		class VulkanStructBuffer : public StructBuffer
		{
		protected:
			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;

		public:
			VulkanStructBuffer(StructBufferType type, MemoryType memoryType) : StructBuffer(type, memoryType)
			{
			}

			virtual ~VulkanStructBuffer()
			{
			}

			VkBuffer getBuffer() const
			{
				return buffer;
			}

			virtual VkDeviceSize getSize() const = 0;
		};

		class VulkanCPUStructBuffer : public VulkanStructBuffer
		{
		private:
			VkDevice device;
			Ref<StructAccessorBlueprint> blueprint;
			void *data;

		public:
			VulkanCPUStructBuffer(Ref<const StructType> source, const Array<Ref<const StructType>> &structs, BufferLayout layout, StructBufferType type,
								  u32 unsizedArrayLength, VkDevice device, VulkanMemoryAllocator *allocator);

			virtual ~VulkanCPUStructBuffer();

			virtual void access(Function<void(StructAccessor)> accessCallback);

			virtual VkDeviceSize getSize() const
			{
				return blueprint->size;
			}
		};

		class VulkanArrayBuffer : public ArrayBuffer
		{
		protected:
			VkDevice device;
			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkDeviceAddress address = 0;

		public:
			VulkanArrayBuffer(ArrayBufferType type, MemoryType memoryType, u32 length, VkDevice device) : ArrayBuffer(type, memoryType, length), device(device)
			{
			}

			virtual ~VulkanArrayBuffer()
			{
			}

			VkBuffer getBuffer() const
			{
				return buffer;
			}

			VkDeviceAddress getAddress() const
			{
				return address;
			}
		};

		class VulkanCPUArrayBuffer : public VulkanArrayBuffer
		{
		private:
			VkDevice device;
			Ref<ArrayAccessorBlueprint> blueprint;
			void *data;

		public:
			VulkanCPUArrayBuffer(Ref<const DataType> elementType, u32 elementCount, ArrayBufferType type, VkDevice device, VulkanMemoryAllocator *allocator);

			virtual ~VulkanCPUArrayBuffer();

			virtual void access(Function<void(ArrayAccessor)> accessCallback);
		};

		class VulkanGPUArrayBuffer : public VulkanArrayBuffer
		{
		private:
			VkDevice device;
			VkQueue queue;
			VulkanSemaphoreChain *semaphoreChain;
			VkCommandBuffer accessCommandBuffer;

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingMemory;

			Ref<ArrayAccessorBlueprint> blueprint;
			void *data;

		public:
			VulkanGPUArrayBuffer(Ref<const DataType> elementType, u32 elementCount, ArrayBufferType type, VkDevice device, VkQueue queue,
								 VulkanMemoryAllocator *allocator, VulkanSemaphoreChain *semaphoreChain, VkCommandBuffer accessCommandBuffer);

			virtual ~VulkanGPUArrayBuffer();

			virtual void access(Function<void(ArrayAccessor)> accessCallback);
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANBUFFER_H_ */
