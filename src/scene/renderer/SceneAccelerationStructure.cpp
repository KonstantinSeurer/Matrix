/*
 * SceneAccelerationStructure.cpp
 *
 *  Created on: Aug 14, 2021
 *      Author: konstantin
 */

#include "SceneAccelerationStructure.h"

namespace matrix {

using namespace graphics;

namespace scene {

#define MAX_INSTANCE_COUNT 1000

void SceneAccelerationStructure::init(Ref<graphics::DeviceInstance> device, u32 bufferCount) {
	auto descriptorSetSource = getSource()->getDescriptorSet("sceneAccelerationStructure");

	for (u32 bufferIndex = 0; bufferIndex < bufferCount; bufferIndex++) {
		FrameData data;
		data.accelerationStructure = device->createTopLevelAccelerationStructure(MAX_INSTANCE_COUNT);
		data.descriptorSet = device->createDescriptorSet(descriptorSetSource);
		buffers.push_back(data);
	}
}

void SceneAccelerationStructure::updateDescriptors(const Scene &scene, u32 bufferIndex) {
	auto modelMap = scene.getRenderImplementation<SceneModelMap>();

	buffers[bufferIndex].accelerationStructure->access([&](Array<AccelerationStructureInstance> &instances) {
		u32 modelIndex = 0;
		for (const auto &entry : modelMap->getEntries()) {
			auto accelerationStructure = entry.first->getRenderImplementation<ModelAccelerationStructure>()->getAccelerationStructure();
			for (auto component : entry.second) {
				AccelerationStructureInstance instance;
				instance.accelerationStructure = accelerationStructure;
				instance.index = modelIndex;
				if (component->getTransform()) {
					instance.transform = component->getTransform()->getGlobalTransform();
				}
				instances.push_back(instance);
			}
			modelIndex++;
		}
	});

	buffers[bufferIndex].descriptorSet->access([&](DescriptorSetAccessor &accessor) {
		accessor.set("accelerationStructure", buffers[bufferIndex].accelerationStructure.get());
	});
}

static Ref<PipelineSource> source;

Ref<PipelineSource> SceneAccelerationStructure::getSource() {
	if (!source) {
		source = parsePipeline(RL("file:///shaders/scene/accelerationStructure.shader"));
		addShaderInclude("scene.AccelerationStructure", source);
	}
	return source;
}

}

}
