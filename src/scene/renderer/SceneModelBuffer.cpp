/*
 * MaterialBuffer.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#include <scene/renderer/SceneModelBuffer.h>

namespace matrix
{

	using namespace graphics;

	namespace scene
	{

		SceneModelBuffer::SceneModelBuffer()
		{
		}

		SceneModelBuffer::~SceneModelBuffer()
		{
		}

		void SceneModelBuffer::init(Ref<DeviceInstance> device, u32 bufferCount)
		{
			Ref<PipelineSource> source = ModelModelBuffer::getSource();

			auto setSource = source->getDescriptorSet("modelBuffer");

			for (u32 i = 0; i < bufferCount; i++)
			{
				FrameData data;
				data.typeTable = device->createStructBuffer(setSource->getBuffer("typeTable")->data, source->structs, BufferLayout::STANDARD, StructBufferType::STORAGE,
															MemoryType::CPU);

				data.descriptorSet = device->createDescriptorSet(setSource);
				data.descriptorSet->access([&data](DescriptorSetAccessor &accessor)
										   { accessor.set("typeTable", data.typeTable.get()); });

				buffers.push_back(data);
			}
		}

		void SceneModelBuffer::updateDescriptors(const Scene &scene, u32 bufferIndex)
		{
			FrameData buffer = buffers[bufferIndex];

			const auto &modelMap = scene.getRenderImplementation<SceneModelMap>();

			buffer.typeTable->access([&](StructAccessor accessor)
									 {
										 ArrayAccessor typesAccessor = accessor.getArray("types");
										 u32 modelIndex = 0;
										 for (const auto &entry : modelMap->getEntries())
										 {
											 StructAccessor typeAccessor = typesAccessor.getStruct(modelIndex++);
											 auto type = entry.first->getType();
											 typeAccessor.setInt("floatAttributeCount", type->getFloatAttributeCount());
											 typeAccessor.setInt("intAttributeCount", type->getIntAttributeCount());
										 }
									 });

			buffer.descriptorSet->access([&](DescriptorSetAccessor &accessor)
										 {
											 u32 modelIndex = 0;
											 for (const auto &entry : modelMap->getEntries())
											 {
												 auto buffer = entry.first->getRenderImplementation<ModelModelBuffer>();
												 if (buffer->getFloats())
												 {
													 accessor.set("floats", modelIndex, buffer->getFloats().get());
												 }
												 if (buffer->getInts())
												 {
													 accessor.set("ints", modelIndex, buffer->getInts().get());
												 }
												 if (buffer->getIndices())
												 {
													 accessor.set("indices", modelIndex, buffer->getIndices().get());
												 }
												 modelIndex++;
											 }
										 });
		}

	}

}
