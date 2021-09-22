/*
 * VulkanDescriptorSetLayout.cpp
 *
 *  Created on: Jun 29, 2021
 *      Author: konstantin
 */

#include "VulkanDescriptorSetLayout.h"

namespace matrix
{

namespace graphics
{

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(Ref<const DescriptorSetSource> source, VkDevice device,
		u32 poolSize) :
		device(device), poolSize(poolSize)
{
	Array<VkDescriptorSetLayoutBinding> bindings(source->descriptors.size());
	for (u32 i = 0; i < source->descriptors.size(); i++)
	{
		bindings[i].binding = i;
		bindings[i].descriptorCount = source->descriptors[i]->arrayLength;
		bindings[i].descriptorType = getVkDescriptorType(source->descriptors[i]->type);
		bindings[i].pImmutableSamplers = null;
		bindings[i].stageFlags = VK_SHADER_STAGE_ALL;
	}

	Array<VkDescriptorBindingFlags> bindingFlags(source->descriptors.size());
	for (u32 i = 0; i < source->descriptors.size(); i++)
	{
		VkDescriptorBindingFlags flags = 0;
		if (source->descriptors[i]->arrayLength > 1)
		{
			flags |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
		}
		bindingFlags[i] = flags;

	}

	VkDescriptorSetLayoutBindingFlagsCreateInfo flagsCreateInfo;
	flagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	flagsCreateInfo.pNext = null;
	flagsCreateInfo.pBindingFlags = bindingFlags.data();
	flagsCreateInfo.bindingCount = bindingFlags.size();

	VkDescriptorSetLayoutCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.pNext = &flagsCreateInfo;
	createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	createInfo.bindingCount = bindings.size();
	createInfo.pBindings = bindings.data();

	assertVkResult(vkCreateDescriptorSetLayout(device, &createInfo, null, &layout), SOURCE_LOCATION());

	UnorderedMap<VkDescriptorType, u32> descriptorCounts;
	for (const auto &descriptor : source->descriptors)
	{
		VkDescriptorType type = getVkDescriptorType(descriptor->type);
		if (descriptorCounts.find(type) == descriptorCounts.end())
		{
			descriptorCounts[type] = descriptor->arrayLength;
		}
		else
		{
			descriptorCounts[type] += descriptor->arrayLength;
		}
	}

	for (const auto &count : descriptorCounts)
	{
		VkDescriptorPoolSize size;
		size.type = count.first;
		size.descriptorCount = count.second * poolSize;
		poolSizes.push_back(size);
	}

	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = null;
	poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
			| VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	poolCreateInfo.maxSets = poolSize;
	poolCreateInfo.poolSizeCount = poolSizes.size();
	poolCreateInfo.pPoolSizes = poolSizes.data();
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(device, layout, null);

	for (VkDescriptorPool pool : pools)
	{
		vkDestroyDescriptorPool(device, pool, null);
	}
}

VulkanDescriptorSetAllocation VulkanDescriptorSetLayout::alloc()
{
	VulkanDescriptorSetAllocation alloc;
	bool found = false;
	for (u32 i = 0; i < pools.size(); i++)
	{
		if (poolAllocCounts[i] < poolSize)
		{
			alloc.pool = pools[i];
			alloc.poolIndex = i;
			poolAllocCounts[i]++;
			found = true;
			break;
		}
	}
	if (!found)
	{
		poolAllocCounts.push_back(1);

		assertVkResult(vkCreateDescriptorPool(device, &poolCreateInfo, null, &alloc.pool), SOURCE_LOCATION());
		alloc.poolIndex = pools.size();
		pools.push_back(alloc.pool);
	}

	VkDescriptorSetAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.pNext = null;
	allocateInfo.descriptorPool = alloc.pool;
	allocateInfo.descriptorSetCount = 1;
	allocateInfo.pSetLayouts = &layout;

	assertVkResult(vkAllocateDescriptorSets(device, &allocateInfo, &alloc.descriptorSet), SOURCE_LOCATION());

	return alloc;
}

void VulkanDescriptorSetLayout::free(VulkanDescriptorSetAllocation alloc)
{
	poolAllocCounts[alloc.poolIndex]--;
	assertVkResult(vkFreeDescriptorSets(device, alloc.pool, 1, &alloc.descriptorSet), SOURCE_LOCATION());
}

VulkanDescriptorSetLayoutCache::VulkanDescriptorSetLayoutCache(VkDevice device) :
		device(device)
{
}

VulkanDescriptorSetLayout* VulkanDescriptorSetLayoutCache::get(Ref<const DescriptorSetSource> source)
{
	if (data.find(source) == data.end())
	{
		Ref<VulkanDescriptorSetLayout> layout = allocate<VulkanDescriptorSetLayout>(source, device, 10);
		data[source] = layout;
		return layout.get();
	}
	return data[source].get();
}

}

}
