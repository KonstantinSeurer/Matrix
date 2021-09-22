/*
 * MaterialBuffer.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#include "SceneLightBuffer.h"
#include "../Light.h"

namespace matrix {

namespace scene {

using namespace graphics;

SceneLightBuffer::SceneLightBuffer() {
}

SceneLightBuffer::~SceneLightBuffer() {
}

void SceneLightBuffer::init(Ref<DeviceInstance> device, u32 bufferCount) {
	Ref<PipelineSource> source = getSource();

	auto setSource = source->getDescriptorSet("lightBuffer");

	for (u32 i = 0; i < bufferCount; i++) {
		FrameData data;
		data.buffer = device->createStructBuffer(setSource->getBuffer("lights")->data, source->structs, BufferLayout::STANDARD, StructBufferType::STORAGE,
			MemoryType::CPU);

		data.descriptorSet = device->createDescriptorSet(setSource);
		data.descriptorSet->access([&data](DescriptorSetAccessor &accessor) {
			accessor.set("lights", data.buffer.get());
		});

		buffers.push_back(data);
	}
}

void SceneLightBuffer::updateDescriptors(const Scene &scene, u32 bufferIndex) {
	FrameData buffer = buffers[bufferIndex];

	const auto &directionalLights = scene.getComponents<DirectionalLightComponent>();

	buffer.buffer->access([&](StructAccessor accessor) {
		ArrayAccessor directionalLightsAccessor = accessor.getArray("directionalLights");
		for (u32 lightIndex = 0; lightIndex < directionalLights.size() && lightIndex < 10; lightIndex++) {
			StructAccessor directionalLightAccessor = directionalLightsAccessor.getStruct(lightIndex);
			directionalLightAccessor.setVec3("color", directionalLights[lightIndex].color);
			directionalLightAccessor.setVec3("direction", directionalLights[lightIndex].direction);
		}
		accessor.setInt("directionalLightCount", directionalLights.size());
	});
}

static Ref<PipelineSource> source;

Ref<PipelineSource> SceneLightBuffer::getSource() {
	if (!source) {
		source = parsePipeline(RL("file:///shaders/scene/lightBuffer.shader"));
		addShaderInclude("scene.LightBuffer", source);
	}
	return source;
}

}

}
