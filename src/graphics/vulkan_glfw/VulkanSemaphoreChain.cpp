/*
 * VulkanSemaphoreChain.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#include "VulkanSemaphoreChain.h"
#include "VulkanUtil.h"

namespace matrix
{

namespace graphics
{

VulkanSemaphoreChain::VulkanSemaphoreChain(VkDevice device, u32 size) :
		device(device)
{
	VkSemaphoreCreateInfo info { };
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	semaphoreCache.resize(size);
	for (u32 i = 0; i < size; i++)
	{
		VkSemaphore semaphore;
		assertVkResult(vkCreateSemaphore(device, &info, null, &semaphore), SOURCE_LOCATION());
		semaphoreCache[i] = semaphore;
	}
}

VulkanSemaphoreChain::~VulkanSemaphoreChain()
{
	for (u32 i = 0; i < semaphoreCache.size(); i++)
	{
		vkDestroySemaphore(device, semaphoreCache[i], null);
	}
}

VkSemaphore VulkanSemaphoreChain::getSemaphore()
{
	if (semaphoreCache.empty())
	{
		err("Semaphore chain cache is empty!");
		return VK_NULL_HANDLE;
	}

	VkSemaphore result = semaphoreCache[semaphoreCache.size() - 1];
	semaphoreCache.resize(semaphoreCache.size() - 1);
	return result;
}

const Array<VkSemaphore> VulkanSemaphoreChain::pollWaitSemaphores()
{
	Array<VkSemaphore> result = waitSemaphores;
	waitSemaphores.clear();
	semaphoreCache.insert(semaphoreCache.begin(), result.begin(), result.end());
	return result;
}

void VulkanSemaphoreChain::addWaitSemaphore(VkSemaphore semaphore)
{
	waitSemaphores.push_back(semaphore);
}

}

}
