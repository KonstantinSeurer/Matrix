/*
 * CameraBuffer.cpp
 *
 *  Created on: Jul 30, 2021
 *      Author: konstantin
 */

#include "SceneCameraBuffer.h"

#include "../Camera.h"

namespace matrix
{

using namespace graphics;

namespace scene
{

SceneCameraBuffer::SceneCameraBuffer()
{
}

SceneCameraBuffer::~SceneCameraBuffer()
{
}

void SceneCameraBuffer::init(Ref<graphics::DeviceInstance> device, u32 bufferCount)
{
	buffers.resize(bufferCount);
	for (u32 bufferIndex = 0; bufferIndex < bufferCount; bufferIndex++)
	{
		FrameData data;

		auto setSource = getSource()->getDescriptorSet("cameraBuffer");
		data.descriptorSet = device->createDescriptorSet(setSource);

		auto bufferSource = setSource->getBuffer("cameras")->data;
		data.buffer = device->createStructBuffer(bufferSource, getSource()->structs, BufferLayout::STANDARD, StructBufferType::STORAGE, MemoryType::CPU);

		data.descriptorSet->access([&data](DescriptorSetAccessor &accessor)
		{
			accessor.set("cameras", data.buffer.get());
		});

		buffers[bufferIndex] = data;
	}
}

void SceneCameraBuffer::updateDescriptors(const Scene &scene, u32 bufferIndex)
{
	buffers[bufferIndex].buffer->access([&scene](StructAccessor accessor)
	{
		ArrayAccessor transforms = accessor.getArray("transforms");

		const auto& cameras = scene.getComponents<CameraComponent>();
		for (u32 cameraIndex = 0; cameraIndex < cameras.size(); cameraIndex++)
		{
			StructAccessor transform = transforms.getStruct(cameraIndex);
			const auto& camera = cameras[cameraIndex];

			math::Mat4 invView = camera.getTransform()->getGlobalTransform();
			transform.setMat4("invView", invView);

			math::Mat4 view = math::Mat4::inverse(invView);
			transform.setMat4("view", view);

			const math::Mat4& projection = camera.frustum->getProjection();
			transform.setMat4("projection", projection);

			math::Mat4 invProjection = math::Mat4::inverse(projection);
			transform.setMat4("invProjection", invProjection);

			math::Mat4 viewProjection = view * projection;
			transform.setMat4("viewProjection", viewProjection);
			transform.setMat4("invViewProjection", math::Mat4::inverse(viewProjection));
		}
	});
}

void SceneCameraBuffer::render(const Scene &scene)
{
}

static Ref<PipelineSource> source;

Ref<PipelineSource> SceneCameraBuffer::getSource()
{
	if (!source)
	{
		source = parsePipeline(RL("file:///shaders/scene/cameraBuffer.shader"));
		addShaderInclude("scene.CameraBuffer", source);
	}
	return source;
}

}

}
