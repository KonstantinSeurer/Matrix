/*
 * VulkanSwapchain.cpp
 *
 *  Created on: Jun 7, 2021
 *      Author: konstantin
 */

#include "VulkanSwapchain.h"
#include "VulkanUtil.h"
#include "VulkanImage.h"
#include <math/Base.h>
#include <cstdint>
#include <vulkan/vk_enum_string_helper.h>

namespace matrix
{

	namespace graphics
	{

		static VkSurfaceFormatKHR getSurfaceFormat(const Array<VkSurfaceFormatKHR> &availableFormats)
		{
			for (const auto &availableFormat : availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					return availableFormat;
				}
			}

			return availableFormats[0];
		}

		static VkPresentModeKHR getPresentMode(const Array<VkPresentModeKHR> &availablePresentModes)
		{
			for (const auto &availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return availablePresentMode;
				}
			}

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VulkanSwapchain::VulkanSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkSurfaceKHR surface,
										 GLFWwindow *window, u32 queueFamilyIndex, Ref<VulkanSemaphoreChain> semaphoreChain) : device(device), queue(queue), currentImageIndex(0), semaphoreChain(semaphoreChain)
		{
			VkBool32 supportsSurface;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &supportsSurface);
			if (supportsSurface == VK_FALSE)
			{
				err("Physical device doesn't support surface!");
			}

			VkSurfaceCapabilitiesKHR capabilities;
			assertVkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities),
						   SOURCE_LOCATION());

			u32 formatCount;
			assertVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, null),
						   SOURCE_LOCATION());
			Array<VkSurfaceFormatKHR> formats(formatCount);
			assertVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data()),
						   SOURCE_LOCATION());

			u32 presentModeCount;
			assertVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, null),
						   SOURCE_LOCATION());
			Array<VkPresentModeKHR> presentModes(presentModeCount);
			assertVkResult(
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()),
				SOURCE_LOCATION());

			VkSurfaceFormatKHR format = getSurfaceFormat(formats);
			VkPresentModeKHR presentMode = getPresentMode(presentModes);

			VkExtent2D extent = capabilities.currentExtent;
			if (extent.width == UINT32_MAX)
			{
				s32 width, height;
				glfwGetWindowSize(window, &width, &height);

				extent.width = math::clamp((u32)width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				extent.height = math::clamp((u32)height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			}

			u32 maxImageCount = capabilities.maxImageCount != 0 ? capabilities.maxImageCount : UINT32_MAX;
			u32 imageCount = math::min(capabilities.minImageCount + 1, maxImageCount);

			VkSwapchainCreateInfoKHR createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.pNext = null;
			createInfo.flags = 0;
			createInfo.surface = surface;

			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = format.format;
			createInfo.imageColorSpace = format.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = null;

			createInfo.preTransform = capabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = presentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			assertVkResult(vkCreateSwapchainKHR(device, &createInfo, null, &swapchain), SOURCE_LOCATION());

			vkGetSwapchainImagesKHR(device, swapchain, &imageCount, null);
			Array<VkImage> vulkanImages(imageCount);
			vkGetSwapchainImagesKHR(device, swapchain, &imageCount, vulkanImages.data());
			auto imageFormat = allocate<VulkanImageFormat>(ImageFormatType::COLOR, 4, 1, false, format.format);
			for (VkImage image : vulkanImages)
			{
				images.push_back(
					allocate<VulkanImage2D>(device, VK_NULL_HANDLE, null, extent.width, extent.height, image, imageFormat,
											ImageUsage::ALL, VK_NULL_HANDLE));
			}

			out("Create Swapchain:");
			out(String("\tFormat: ") + string_VkFormat(format.format) + " " + string_VkColorSpaceKHR(format.colorSpace));
			out(String("\tPresentMode: ") + string_VkPresentModeKHR(presentMode));
			out("\tWidth: " + toString(extent.width) + " Height: " + toString(extent.height));
			out("\tImageCount: " + toString(imageCount));
		}

		const Array<Ref<Image2D>> &VulkanSwapchain::getImages() const
		{
			return images;
		}

		u32 VulkanSwapchain::pollImage()
		{
			VkSemaphore signalSemaphore = semaphoreChain->getSemaphore();
			semaphoreChain->addWaitSemaphore(signalSemaphore);
			VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, signalSemaphore, VK_NULL_HANDLE,
													&currentImageIndex);
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				out("resize");
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				assertVkResult(result, SOURCE_LOCATION());
			}
			return currentImageIndex;
		}

		u32 VulkanSwapchain::getCurrentImageIndex() const
		{
			return currentImageIndex;
		}

		void VulkanSwapchain::present()
		{
			VkPresentInfoKHR info{};
			info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			info.pNext = null;

			Array<VkSemaphore> waitSemaphores = semaphoreChain->pollWaitSemaphores();
			info.waitSemaphoreCount = waitSemaphores.size();
			info.pWaitSemaphores = waitSemaphores.data();

			info.swapchainCount = 1;
			info.pSwapchains = &swapchain;
			info.pImageIndices = &currentImageIndex;

			info.pResults = null;

			assertVkResult(vkQueuePresentKHR(queue, &info), SOURCE_LOCATION());
		}

		VulkanSwapchain::~VulkanSwapchain()
		{
			vkDestroySwapchainKHR(device, swapchain, null);
		}

	}

}
