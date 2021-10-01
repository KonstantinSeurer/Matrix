/*
 * VulkansemaphoreChain.h
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANSEMAPHORECHAIN_H_
#define GRAPHICS_VULKAN_GLFW_VULKANSEMAPHORECHAIN_H_

#include <vulkan/vulkan.h>

#include <Base.h>

namespace matrix
{

	namespace graphics
	{

		class VulkanSemaphoreChain
		{
		private:
			VkDevice device;
			Array<VkSemaphore> semaphoreCache;
			Array<VkSemaphore> waitSemaphores;

		public:
			VulkanSemaphoreChain(VkDevice device, u32 size);
			~VulkanSemaphoreChain();

			VkSemaphore getSemaphore();
			const Array<VkSemaphore> pollWaitSemaphores();
			void addWaitSemaphore(VkSemaphore semaphore);
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANSEMAPHORECHAIN_H_ */
