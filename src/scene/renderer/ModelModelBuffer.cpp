/*
 * ModelstorageBuffer.cpp
 *
 *  Created on: Aug 14, 2021
 *      Author: konstantin
 */

#include <scene/renderer/ModelModelBuffer.h>

#include <Logging.h>

namespace matrix
{

	using namespace graphics;
	using namespace math;

	namespace scene
	{

		void ModelModelBuffer::init(const ModelType *type, Ref<DeviceInstance> device, const Array<void *> &attributes, u32 vertexCount, const Array<u32> &indexData)
		{
			u32 floatCount = vertexCount * type->getFloatAttributeCount();
			if (floatCount > 0)
			{
				floats = device->createStructBuffer(getSource()->getDescriptorSet("modelBuffer")->getBuffer("floats")->data, {}, BufferLayout::STANDARD,
													StructBufferType::STORAGE, MemoryType::CPU, floatCount);
				floats->access([&](StructAccessor accessor)
							   {
								   ArrayAccessor dataAccessor = accessor.getArray("data");

								   u32 offset = 0;

								   const auto &typeAttributes = type->getAttributes();
								   for (u32 attributeIndex = 0; attributeIndex < typeAttributes.size(); attributeIndex++)
								   {
									   const auto &attribute = typeAttributes[attributeIndex];
									   switch (attribute.first)
									   {
									   case PrimitiveTypeType::FLOAT:
									   {
										   f32 *data = (f32 *)attributes[attributeIndex];
										   for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
										   {
											   dataAccessor.setFloat(type->getFloatAttributeCount() * vertexIndex + offset, data[vertexIndex]);
										   }
										   offset++;
										   break;
									   }
									   case PrimitiveTypeType::VEC2:
									   {
										   Vec2 *data = (Vec2 *)attributes[attributeIndex];
										   for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
										   {
											   u32 baseIndex = type->getFloatAttributeCount() * vertexIndex + offset;
											   dataAccessor.setFloat(baseIndex, data[vertexIndex].x);
											   dataAccessor.setFloat(baseIndex + 1, data[vertexIndex].y);
										   }
										   offset += 2;
										   break;
									   }
									   case PrimitiveTypeType::VEC3:
									   {
										   Vec3 *data = (Vec3 *)attributes[attributeIndex];
										   for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
										   {
											   u32 baseIndex = type->getFloatAttributeCount() * vertexIndex + offset;
											   dataAccessor.setFloat(baseIndex, data[vertexIndex].x);
											   dataAccessor.setFloat(baseIndex + 1, data[vertexIndex].y);
											   dataAccessor.setFloat(baseIndex + 2, data[vertexIndex].z);
										   }
										   offset += 3;
										   break;
									   }
									   case PrimitiveTypeType::VEC4:
									   {
										   Vec4 *data = (Vec4 *)attributes[attributeIndex];
										   for (u32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
										   {
											   u32 baseIndex = type->getFloatAttributeCount() * vertexIndex + offset;
											   dataAccessor.setFloat(baseIndex, data[vertexIndex].x);
											   dataAccessor.setFloat(baseIndex + 1, data[vertexIndex].y);
											   dataAccessor.setFloat(baseIndex + 2, data[vertexIndex].z);
											   dataAccessor.setFloat(baseIndex + 3, data[vertexIndex].w);
										   }
										   offset += 4;
										   break;
									   }
									   default:
										   err("Unsupported vertex attribute type " + toString((u32)attribute.first) + "!");
										   break;
									   }
								   }
							   });
			}

			u32 intCount = vertexCount * type->getIntAttributeCount();
			if (intCount > 0)
			{
				ints = device->createStructBuffer(getSource()->getDescriptorSet("modelBuffer")->getBuffer("ints")->data, {}, BufferLayout::STANDARD,
												  StructBufferType::STORAGE, MemoryType::CPU, intCount);
				// TODO: implement
			}

			if (!indexData.empty())
			{
				indices = device->createStructBuffer(getSource()->getDescriptorSet("modelBuffer")->getBuffer("indices")->data, {}, BufferLayout::STANDARD,
													 StructBufferType::STORAGE, MemoryType::CPU, indexData.size());
				indices->access([&](StructAccessor accessor)
								{
									ArrayAccessor dataAccessor = accessor.getArray("data");
									for (u32 indexIndex = 0; indexIndex < indexData.size(); indexIndex++)
									{
										dataAccessor.setInt(indexIndex, indexData[indexIndex]);
									}
								});
			}
		}

		static Ref<PipelineSource> source = null;

		Ref<PipelineSource> ModelModelBuffer::getSource()
		{
			if (!source)
			{
				source = parsePipeline(RL("file:///shaders/scene/modelBuffer.shader"));
				addShaderInclude("scene.ModelBuffer", source);
			}
			return source;
		}

	}

}
