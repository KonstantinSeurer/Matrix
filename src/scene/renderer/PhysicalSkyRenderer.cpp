/*
 * PhysicalSkyRenderer.cpp
 *
 *  Created on: Sep 3, 2021
 *      Author: konstantin
 */

#include "PhysicalSkyRenderer.h"
#include "SceneLightBuffer.h"

namespace matrix
{

using namespace graphics;

namespace scene
{

PhysicalSkyRenderer::PhysicalSkyRenderer(Ref<graphics::DeviceInstance> device, u32 resolution) :
	SkyRenderer(device, resolution)
{
	SceneLightBuffer::getSource();

	Ref<ComputePipelineSource> source = parseComputePipeline(RL("file:///shaders/scene/renderer/physicalSky.shader"));
	pipeline = device->createComputePipeline(source);

	descriptorSet = device->createDescriptorSet(source->getDescriptorSet("data"));
	descriptorSet->access([&](DescriptorSetAccessor &accessor)
	{
		accessor.set("result", getEnvironmentView().get());
		accessor.set("transmittance", getTransmittanceView().get());
	});
}

void PhysicalSkyRenderer::render(Ref<graphics::CommandBuffer> cb, const Scene &scene, ComponentReference camera, u32 bufferIndex)
{
	auto lightBuffer = scene.getRenderImplementation<SceneLightBuffer>();

	cb->barrier(
		{ ImageLayoutTransition(getEnvironment(), ImageLayout::UNDEFINED, ImageLayout::SHADER_STORAGE), ImageLayoutTransition(getTransmittance(),
			ImageLayout::UNDEFINED, ImageLayout::SHADER_STORAGE) });

	cb->computePipeline(pipeline, [&]()
	{
		cb->descriptorSets(
			{	lightBuffer->getDescriptorSet(bufferIndex), descriptorSet}, [&]()
			{
				cb->compute(math::Vec3u32(getEnvironment()->width, getEnvironment()->height, 1), ComputeSize::GLOBAL);
			});
	});
}

}

}
