/*
 * VulkanPipeline.cpp
 *
 *  Created on: Jun 15, 2021
 *      Author: konstantin
 */

#include "VulkanPipeline.h"

namespace matrix {

namespace graphics {

VulkanPipeline::VulkanPipeline(VkDevice device, Ref<const PipelineSource> source, VulkanDescriptorSetLayoutCache *setLayoutCache) :
	device(device) {

	Ref<StructType> uniforms = allocate<StructType>("uniforms", source->uniforms);
	uniformAccessor = createStructAccessorBlueprint(uniforms, source->structs, BufferLayout::STANDARD);
	uniformData = memAlloc(uniformAccessor->size);

	VkPipelineLayoutCreateInfo layoutCreateInfo;
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = null;
	layoutCreateInfo.flags = 0;

	VkPushConstantRange pushConstants;
	if (uniformAccessor->size > 0) {
		pushConstants.offset = 0;
		pushConstants.size = uniformAccessor->size;
		pushConstants.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;

		layoutCreateInfo.pushConstantRangeCount = 1;
		layoutCreateInfo.pPushConstantRanges = &pushConstants;
	} else {
		layoutCreateInfo.pushConstantRangeCount = 0;
		layoutCreateInfo.pPushConstantRanges = null;
	}

	Array<VkDescriptorSetLayout> setLayouts;
	for (Ref<const DescriptorSetSource> set : source->descriptorSets) {
		setLayouts.push_back(setLayoutCache->get(set)->getLayout());
	}

	layoutCreateInfo.setLayoutCount = setLayouts.size();
	layoutCreateInfo.pSetLayouts = setLayouts.data();

	assertVkResult(vkCreatePipelineLayout(device, &layoutCreateInfo, null, &layout), SOURCE_LOCATION());
}

VulkanPipeline::~VulkanPipeline() {
	vkDestroyPipeline(device, pipeline, null);
	vkDestroyPipelineLayout(device, layout, null);
	memFree(uniformData);
}

static UnorderedMap<Topology, VkPrimitiveTopology> topologies = { { Topology::LINE_LIST, VK_PRIMITIVE_TOPOLOGY_LINE_LIST }, { Topology::LINE_STRIP,
	VK_PRIMITIVE_TOPOLOGY_LINE_STRIP }, { Topology::PATCH_LIST, VK_PRIMITIVE_TOPOLOGY_PATCH_LIST }, { Topology::POINT_LIST, VK_PRIMITIVE_TOPOLOGY_POINT_LIST },
	{ Topology::TRIANGLE_LIST, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }, { Topology::TRIANGLE_STRIP, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP } };

static UnorderedMap<DrawMode, VkPolygonMode> drawModes = { { DrawMode::EDGES, VK_POLYGON_MODE_LINE }, { DrawMode::FILL, VK_POLYGON_MODE_FILL }, {
	DrawMode::VERTICES, VK_POLYGON_MODE_POINT } };

static UnorderedMap<CompareOperation, VkCompareOp> compareOperations = { { CompareOperation::ALWAYS, VK_COMPARE_OP_ALWAYS }, { CompareOperation::EQUAL,
	VK_COMPARE_OP_EQUAL }, { CompareOperation::GREATER, VK_COMPARE_OP_GREATER }, { CompareOperation::GREATER_OR_EQUAL, VK_COMPARE_OP_GREATER_OR_EQUAL }, {
	CompareOperation::LESS, VK_COMPARE_OP_LESS }, { CompareOperation::LESS_OR_EQUAL, VK_COMPARE_OP_LESS_OR_EQUAL }, { CompareOperation::NEVER,
	VK_COMPARE_OP_NEVER }, { CompareOperation::NOT_EQUAL, VK_COMPARE_OP_NOT_EQUAL } };

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device, Ref<GraphicsPipelineSource> source, Ref<RenderPass> renderPass, u32 subpass,
	VulkanDescriptorSetLayoutCache *setLayoutCache) :
	GraphicsPipeline(source), VulkanPipeline(device, source, setLayoutCache) {

	VkPipelineVertexInputStateCreateInfo vertexInput;
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.pNext = null;
	vertexInput.flags = 0;

	const auto &vertexInputs = source->shaders[ShaderType::VERTEX].inputs;
	Ref<StructType> vertexInputSource = allocate<StructType>("vertexShader.inputs", vertexInputs);
	Ref<StructAccessorBlueprint> vertexBlueprint = CastDown<StructAccessorBlueprint>(createArrayBufferElementBlueprint(vertexInputSource));

	Array<VkVertexInputAttributeDescription> vertexAttributes(vertexInputs.size());
	for (u32 i = 0; i < vertexInputs.size(); i++) {
		auto &attribute = vertexAttributes[i];
		attribute.binding = 0;
		attribute.location = i;
		attribute.offset = vertexBlueprint->members[vertexInputs[i].name].first;
		attribute.format = getVkFormat(CastDown<const PrimitiveType>(vertexInputs[i].type)->primitiveType);
	}

	vertexInput.vertexAttributeDescriptionCount = vertexAttributes.size();
	vertexInput.pVertexAttributeDescriptions = vertexAttributes.data();

	VkVertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = vertexBlueprint->size;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &bindingDescription;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.pNext = null;
	inputAssembly.flags = 0;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	inputAssembly.topology = topologies[source->topology];

	VkPipelineTessellationStateCreateInfo tesselation;
	tesselation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tesselation.pNext = null;
	tesselation.flags = 0;
	tesselation.patchControlPoints = source->patchSize;

	VkPipelineViewportStateCreateInfo viewport;
	viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport.pNext = null;
	viewport.flags = 0;

	VkViewport vp;
	vp.x = 0.0f;
	vp.y = 0.0f;
	vp.width = 1.0f;
	vp.height = 1.0f;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;
	viewport.viewportCount = 1;
	viewport.pViewports = &vp;

	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = { 1, 1 };
	viewport.scissorCount = 1;
	viewport.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterization;
	rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization.pNext = null;
	rasterization.flags = 0;

	VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
	if (source->cullBack) {
		cullMode |= VK_CULL_MODE_BACK_BIT;
	}
	if (source->cullFront) {
		cullMode |= VK_CULL_MODE_FRONT_BIT;
	}
	rasterization.cullMode = cullMode;
	rasterization.frontFace = source->frontFace == FrontFace::CLOCKWISE ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;

	rasterization.depthBiasClamp = 0.0f;
	rasterization.depthBiasConstantFactor = 0.0f;
	rasterization.depthBiasSlopeFactor = 0.0f;
	rasterization.depthBiasEnable = VK_FALSE;
	rasterization.depthClampEnable = source->depthClamp;

	rasterization.lineWidth = 1.0f;
	rasterization.polygonMode = drawModes[source->drawMode];
	rasterization.rasterizerDiscardEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisample;
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.pNext = null;
	multisample.flags = 0;

	multisample.alphaToCoverageEnable = VK_FALSE;
	multisample.alphaToOneEnable = VK_FALSE;

	multisample.minSampleShading = 1.0f;
	multisample.sampleShadingEnable = VK_FALSE;

	multisample.pSampleMask = null;
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil { };
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.pNext = null;
	depthStencil.flags = 0;

	depthStencil.depthTestEnable = source->depthTest;
	depthStencil.depthWriteEnable = source->depthWrite;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.depthCompareOp = compareOperations[source->depthCompareOperation];
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;

	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlend { };
	colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlend.pNext = null;
	colorBlend.flags = 0;

	Array<VkPipelineColorBlendAttachmentState> attachments;
	for (const auto &attachment : renderPass->getSubpasses()[subpass].colorAttachments) {
		VkPipelineColorBlendAttachmentState state { };
		state.blendEnable = source->blend;
		state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		attachments.push_back(state);
	}

	colorBlend.attachmentCount = attachments.size();
	colorBlend.pAttachments = attachments.data();

	colorBlend.logicOpEnable = VK_FALSE;

	VkPipelineDynamicStateCreateInfo dynamic;
	dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic.pNext = null;
	dynamic.flags = 0;

	Array<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	dynamic.dynamicStateCount = dynamicStates.size();
	dynamic.pDynamicStates = dynamicStates.data();

	VkGraphicsPipelineCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.pNext = null;
	createInfo.flags = 0;

	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = 0;

	createInfo.subpass = subpass;
	createInfo.renderPass = CastDown<VulkanRenderPass>(renderPass)->getRenderPass();

	createInfo.layout = layout;

	Array<VkPipelineShaderStageCreateInfo> stages;
	for (const auto &shader : source->shaders) {
		VkPipelineShaderStageCreateInfo stage;
		stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage.pNext = null;
		stage.flags = 0;
		stage.stage = getShaderStage(shader.first);
		stage.pSpecializationInfo = null;
		stage.pName = "main";
		stage.module = compileShader(device, *source, shader.second, shader.first);

		stages.push_back(stage);
	}

	createInfo.stageCount = stages.size();
	createInfo.pStages = stages.data();

	createInfo.pVertexInputState = &vertexInput;
	createInfo.pInputAssemblyState = &inputAssembly;
	createInfo.pTessellationState = &tesselation;
	createInfo.pViewportState = &viewport;
	createInfo.pRasterizationState = &rasterization;
	createInfo.pMultisampleState = &multisample;
	createInfo.pDepthStencilState = &depthStencil;
	createInfo.pColorBlendState = &colorBlend;
	createInfo.pDynamicState = &dynamic;

	assertVkResult(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, null, &pipeline), SOURCE_LOCATION());

	for (const auto &shader : stages) {
		vkDestroyShaderModule(device, shader.module, null);
	}
}

VulkanComputePipeline::VulkanComputePipeline(VkDevice device, Ref<ComputePipelineSource> source, VulkanDescriptorSetLayoutCache *setLayoutCache) :
	ComputePipeline(source), VulkanPipeline(device, source, setLayoutCache) {

	VkComputePipelineCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = null;
	createInfo.flags = 0;

	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = 0;

	createInfo.layout = layout;

	createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage.pNext = null;
	createInfo.stage.flags = 0;
	createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	createInfo.stage.pSpecializationInfo = null;
	createInfo.stage.pName = "main";
	createInfo.stage.module = compileShader(device, *source, source->shaderSource, ShaderType::COMPUTE);

	assertVkResult(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &createInfo, null, &pipeline), SOURCE_LOCATION());

	vkDestroyShaderModule(device, createInfo.stage.module, null);
}

}

}
