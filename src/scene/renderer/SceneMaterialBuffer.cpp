/*
 * MaterialBuffer.cpp
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#include <scene/renderer/SceneMaterialBuffer.h>

namespace matrix
{

	namespace scene
	{

		using namespace graphics;

		SceneMaterialBuffer::SceneMaterialBuffer()
		{
		}

		SceneMaterialBuffer::~SceneMaterialBuffer()
		{
		}

		void SceneMaterialBuffer::init(Ref<graphics::DeviceInstance> device, u32 bufferCount)
		{
			Ref<PipelineSource> source = getSource();

			auto setSource = source->getDescriptorSet("materialBuffer");

			for (u32 i = 0; i < bufferCount; i++)
			{
				FrameData data;
				data.typeTable = device->createStructBuffer(setSource->getBuffer("typeTable")->data, source->structs, BufferLayout::STANDARD, StructBufferType::STORAGE,
															MemoryType::CPU);
				data.attributes = device->createStructBuffer(setSource->getBuffer("attributes")->data, source->structs, BufferLayout::STANDARD,
															 StructBufferType::STORAGE, MemoryType::CPU);

				data.descriptorSet = device->createDescriptorSet(setSource);
				data.descriptorSet->access([&data](DescriptorSetAccessor &accessor)
										   {
											   accessor.set("typeTable", data.typeTable.get());
											   accessor.set("attributes", data.attributes.get());
										   });

				buffers.push_back(data);
			}
		}

		void SceneMaterialBuffer::updateDescriptors(const Scene &scene, u32 bufferIndex)
		{
			FrameData buffer = buffers[bufferIndex];

			buffer.typeTable->access([this](StructAccessor accessor)
									 {
										 ArrayAccessor types = accessor.getArray("types");
										 u32 i = 0;
										 u32 floatAttributesOffset = 0;
										 u32 intAttributesOffset = 0;
										 u32 imageSamplers2DOffset = 0;
										 u32 imageSamplers3DOffset = 0;

										 for (auto entry : materials)
										 {
											 StructAccessor type = types.getStruct(i);

											 u32 floatAttributeCount = entry.type->getFloatAttributeCount();
											 u32 intAttributeCount = entry.type->getFloatAttributeCount();
											 u32 imageSampler2DCount = entry.type->getImageSampler2DCount();
											 u32 imageSampler3DCount = entry.type->getImageSampler3DCount();

											 type.setInt("floatAttributesOffset", floatAttributesOffset);
											 type.setInt("intAttributesOffset", intAttributesOffset);
											 type.setInt("imageSamplers2DOffset", imageSamplers2DOffset);
											 type.setInt("imageSamplers3DOffset", imageSamplers3DOffset);

											 type.setInt("floatAttributeCount", floatAttributeCount);
											 type.setInt("intAttributeCount", intAttributeCount);
											 type.setInt("imageSampler2DCount", imageSampler2DCount);
											 type.setInt("imageSampler3DCount", imageSampler3DCount);

											 floatAttributesOffset += floatAttributeCount * entry.materials.size();
											 intAttributesOffset += intAttributeCount * entry.materials.size();
											 imageSamplers2DOffset += imageSampler2DCount * entry.materials.size();
											 imageSamplers3DOffset += imageSampler3DCount * entry.materials.size();
											 i++;
										 }
									 });

			buffer.attributes->access([this](StructAccessor accessor)
									  {
										  ArrayAccessor ints = accessor.getArray("ints");
										  ArrayAccessor floats = accessor.getArray("floats");

										  u32 floatAttributesOffset = 0;
										  u32 intAttributesOffset = 0;

										  for (const auto &entry : materials)
										  {
											  for (const auto &material : entry.materials)
											  {
												  for (u32 i = 0; i < entry.type->getIntAttributeCount(); i++)
												  {
													  ints.setInt(intAttributesOffset++, material->getInt(i));
												  }
												  for (u32 i = 0; i < entry.type->getFloatAttributeCount(); i++)
												  {
													  floats.setFloat(floatAttributesOffset++, material->getFloat(i));
												  }
											  }
										  }
									  });

			buffer.descriptorSet->access([this](DescriptorSetAccessor &accessor)
										 {
											 u32 index2D = 0;
											 u32 index3D = 0;
											 for (const auto &entry : materials)
											 {
												 for (const auto &material : entry.materials)
												 {
													 for (u32 i = 0; i < entry.type->getImageSampler2DCount(); i++)
													 {
														 accessor.set("imageSamplers2D", index2D++, material->getImageSampler2D(i).get());
													 }
													 for (u32 i = 0; i < entry.type->getImageSampler2DCount(); i++)
													 {
														 accessor.set("imageSamplers3D", index3D++, material->getImageSampler3D(i).get());
													 }
												 }
											 }
										 });
		}

		void SceneMaterialBuffer::render(const Scene &scene)
		{
			materials.clear();

			auto modelMap = scene.getRenderImplementation<SceneModelMap>();

			for (auto entry : modelMap->getEntries())
			{
				const Material *material = entry.first->getMaterial().get();

				s32 entryIndex = -1;
				for (u32 i = 0; i < materials.size(); i++)
				{
					if (material->getType() == materials[i].type)
					{
						entryIndex = i;
					}
				}

				if (entryIndex < 0)
				{
					entryIndex = materials.size();
					materials.push_back(MaterialBufferEntry(material->getType()));
				}

				auto &materialsEntry = materials[entryIndex];
				auto &materialArray = materialsEntry.materials;
				if (find(materialArray.begin(), materialArray.end(), material) == materialArray.end())
				{
					materialArray.push_back(material);
				}
			}
		}

		u32 SceneMaterialBuffer::getMaterialTypeIndex(const MaterialType *type)
		{
			for (u32 i = 0; i < materials.size(); i++)
			{
				if (type == materials[i].type)
				{
					return i;
				}
			}
			return 0;
		}

		u32 SceneMaterialBuffer::getMaterialIndex(u32 typeIndex, const Material *material)
		{
			const auto &materialArray = materials[typeIndex].materials;
			for (u32 i = 0; i < materialArray.size(); i++)
			{
				if (material == materialArray[i])
				{
					return i;
				}
			}
			return 0;
		}

		u32 SceneMaterialBuffer::getMaterialIndex(const Material *material)
		{
			return getMaterialIndex(getMaterialTypeIndex(material->getType()), material);
		}

		static Ref<PipelineSource> source;

		Ref<PipelineSource> SceneMaterialBuffer::getSource()
		{
			if (!source)
			{
				source = parsePipeline(RL("file:///shaders/scene/materialBuffer.shader"));
				addShaderInclude("scene.MaterialBuffer", source);
			}
			return source;
		}

	}

}
