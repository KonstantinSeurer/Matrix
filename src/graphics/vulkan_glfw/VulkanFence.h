/*
 * VulkanFence.h
 *
 *  Created on: Jul 28, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANFENCE_H_
#define GRAPHICS_VULKAN_GLFW_VULKANFENCE_H_

#include <graphics/Fence.h>
#include "VulkanUtil.h"

namespace matrix
{

	namespace graphics
	{

		class VulkanFence : public Fence
		{
		private:
			VkDevice device;
			VkFence fence;

		public:
			VulkanFence(VkDevice device);
			virtual ~VulkanFence();

			VkFence getFence() const
			{
				return fence;
			}
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANFENCE_H_ */
