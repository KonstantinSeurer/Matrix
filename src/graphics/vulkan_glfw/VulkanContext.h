/*
 * VulkanGraphicsContext.h
 *
 *  Created on: Jun 3, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANCONTEXT_H_
#define GRAPHICS_VULKAN_GLFW_VULKANCONTEXT_H_

#include <vulkan/vulkan.h>
#include <graphics/Context.h>

namespace matrix
{

	namespace graphics
	{

		class VulkanContext : public Context
		{
		private:
			VkInstance instance;
			Array<Ref<Device>> availableDevices;

		public:
			VulkanContext();

			virtual ~VulkanContext();

			virtual const Array<Ref<Device>> &getAvailableDevices();

			VkInstance getInstance() const
			{
				return instance;
			}

			virtual Ref<WindowSurface> createSurface(Ref<Window> window) const;
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANCONTEXT_H_ */
