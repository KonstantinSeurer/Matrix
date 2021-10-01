/*
 * VulkanSwapchain.h
 *
 *  Created on: Jun 7, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANSWAPCHAIN_H_
#define GRAPHICS_VULKAN_GLFW_VULKANSWAPCHAIN_H_

#include <graphics/Swapchain.h>
#include <Base.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "VulkanSemaphoreChain.h"

namespace matrix
{

	namespace graphics
	{

		class VulkanSwapchain : public Swapchain
		{
		private:
			VkSwapchainKHR swapchain;
			VkDevice device;
			VkQueue queue;
			Array<Ref<Image2D>> images;
			u32 currentImageIndex;
			Ref<VulkanSemaphoreChain> semaphoreChain;

		public:
			VulkanSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkSurfaceKHR surface, GLFWwindow *window,
							u32 queueFamilyIndex, Ref<VulkanSemaphoreChain> semaphoreChain);

			virtual ~VulkanSwapchain();

			virtual const Array<Ref<Image2D>> &getImages() const;

			virtual u32 pollImage();
			virtual u32 getCurrentImageIndex() const;

			virtual void present();
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANSWAPCHAIN_H_ */
