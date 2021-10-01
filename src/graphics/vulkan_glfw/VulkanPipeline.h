/*
 * VulkanPipeline.h
 *
 *  Created on: Jun 15, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANPIPELINE_H_
#define GRAPHICS_VULKAN_GLFW_VULKANPIPELINE_H_

#include <graphics/Pipeline.h>
#include "VulkanRenderPass.h"
#include "VulkanUtil.h"
#include "VulkanDescriptorSetLayout.h"

namespace matrix
{

	namespace graphics
	{

		class VulkanPipeline
		{
		protected:
			VkDevice device;
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout layout;
			Ref<const StructAccessorBlueprint> uniformAccessor;
			void *uniformData;

		public:
			VulkanPipeline(VkDevice device, Ref<const PipelineSource> source, VulkanDescriptorSetLayoutCache *setLayoutCache);

			virtual ~VulkanPipeline();

			VkPipeline getPipeline() const
			{
				return pipeline;
			}

			VkPipelineLayout getLayout() const
			{
				return layout;
			}

			Ref<const StructAccessorBlueprint> getUniformAccessor() const
			{
				return uniformAccessor;
			}

			void *getUniformData() const
			{
				return uniformData;
			}
		};

		class VulkanGraphicsPipeline : public GraphicsPipeline, public VulkanPipeline
		{
		public:
			VulkanGraphicsPipeline(VkDevice device, Ref<GraphicsPipelineSource> source, Ref<RenderPass> renderPass, u32 subpass,
								   VulkanDescriptorSetLayoutCache *setLayoutCache);
		};

		class VulkanComputePipeline : public ComputePipeline, public VulkanPipeline
		{
		public:
			VulkanComputePipeline(VkDevice device, Ref<ComputePipelineSource> source, VulkanDescriptorSetLayoutCache *setLayoutCache);
		};

	}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANPIPELINE_H_ */
