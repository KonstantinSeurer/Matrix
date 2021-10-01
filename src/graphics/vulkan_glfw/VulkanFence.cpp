/*
 * VulkanFence.cpp
 *
 *  Created on: Jul 28, 2021
 *      Author: konstantin
 */

#include "VulkanFence.h"
#include <Time.h>

namespace matrix
{

	namespace graphics
	{

		VulkanFence::VulkanFence(VkDevice device) : device(device)
		{
			VkFenceCreateInfo info;
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.pNext = null;
			info.flags = 0;

			assertVkResult(vkCreateFence(device, &info, null, &fence), SOURCE_LOCATION());
		}

		VulkanFence::~VulkanFence()
		{
			vkDestroyFence(device, fence, null);
		}

	}

}
