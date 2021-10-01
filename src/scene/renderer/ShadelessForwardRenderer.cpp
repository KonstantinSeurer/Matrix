/*
 * ShadelessForwardRenderer.cpp
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#include <scene/renderer/ShadelessForwardRenderer.h>
#include <scene/renderer/ModelVertexArray.h>
#include <Logging.h>
#include <scene/GLTF.h>
#include <scene/renderer/SceneCameraBuffer.h>
#include <scene/renderer/SceneMaterialBuffer.h>
#include <scene/renderer/SceneModelMap.h>

namespace matrix
{

	namespace scene
	{

		using namespace graphics;

		ShadelessForwardRenderer::ShadelessForwardRenderer(Ref<DeviceInstance> device) : device(device)
		{
			colorFormat = device->createImageFormat(ImageFormatType::COLOR, 4, 1, false);
			depthFormat = device->createImageFormat(ImageFormatType::DEPTH, 1, 3, false);

			Array<Attachment> attachments(2);
			attachments[0].beginLayout = ImageLayout::UNDEFINED;
			attachments[0].endLayout = ImageLayout::TRANSFER_SOURCE;
			attachments[0].format = colorFormat;
			attachments[0].loadOperation = LoadOperation::CLEAR;
			attachments[0].storeOperation = StoreOperation::STORE;
			attachments[0].sampleCount = 1;

			attachments[1].beginLayout = ImageLayout::UNDEFINED;
			attachments[1].endLayout = ImageLayout::DEPTH_ATTACHMENT;
			attachments[1].format = depthFormat;
			attachments[1].loadOperation = LoadOperation::CLEAR;
			attachments[1].storeOperation = StoreOperation::STORE;
			attachments[1].sampleCount = 1;

			Array<Subpass> subpasses(1);
			subpasses[0].colorAttachments.push_back(AttachmentReference(0, ImageLayout::COLOR_ATTACHMENT, AttachmentType::COLOR));
			subpasses[0].depthStencilAttachment = AttachmentReference(1, ImageLayout::DEPTH_ATTACHMENT, AttachmentType::DEPTH);
			subpasses[0].pipelineType = PipelineType::GRAPHICS;

			renderPass = device->createRenderpass(attachments, subpasses);

			registerGLTFShaderInclude();
			SceneCameraBuffer::getSource();

			Ref<GraphicsPipelineSource> source = parseGraphicsPipeline(RL("file:///shaders/scene/renderer/shadelessForward.shader"));
			source->cullBack = true;
			source->depthTest = true;
			source->depthWrite = true;
			source->depthCompareOperation = CompareOperation::LESS_OR_EQUAL;
			pipeline = device->createGraphicsPipeline(source, renderPass, 0);
		}

		ShadelessForwardRenderer::~ShadelessForwardRenderer()
		{
		}

		void ShadelessForwardRenderer::updateDescriptors(u32 bufferIndex)
		{
		}

		void ShadelessForwardRenderer::render(Ref<graphics::CommandBuffer> cb, const Scene &scene, ComponentReference camera, u32 bufferIndex)
		{
			auto models = scene.getRenderImplementation<SceneModelMap>();
			auto materials = scene.getRenderImplementation<SceneMaterialBuffer>();
			auto cameras = scene.getRenderImplementation<SceneCameraBuffer>();

			s32 materialTypeIndex = (s32)materials->getMaterialTypeIndex(getGLTFMaterialType());

			cb->renderPass(renderPass, frameBuffer, ClearData({1.0f}, 1.0f), [&]()
						   {
							   cb->graphicsPipeline(pipeline, [&]()
													{
														cb->descriptorSets({materials->getDescriptorSet(bufferIndex), cameras->getDescriptorSet(bufferIndex)}, [&]()
																		   {
																			   for (const auto &entry : models->getEntries())
																			   {
																				   auto vertexArray = entry.first->getRenderImplementation<ModelVertexArray>();
																				   cb->vertexArray(vertexArray->getVertexBuffer(), vertexArray->getIndexBuffer(), [&]()
																								   {
																									   for (auto instance : entry.second)
																									   {
																										   const TransformComponent *transform = instance->getTransform();
																										   cb->uniforms([&](StructAccessor accessor)
																														{
																															math::Mat4 transformMatrix;
																															if (transform)
																															{
																																transformMatrix = transform->getGlobalTransform();
																															}
																															accessor.setMat4("transform", transformMatrix);

																															accessor.setInt("camera", camera.index);

																															StructAccessor materialIndex = accessor.getStruct("material");
																															materialIndex.setInt("type", materialTypeIndex);
																															materialIndex.setInt("index", materials->getMaterialIndex(materialTypeIndex, entry.first->getMaterial().get()));
																														});
																										   cb->drawIndexed(vertexArray->getIndexBuffer()->length, 0, 0, 1, 0);
																									   }
																								   });
																			   }
																		   });
													});
						   });
		}

		void ShadelessForwardRenderer::resize(u32 width, u32 height)
		{
			colorAttachment = device->createImage2D(width, height, 1, colorFormat, ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSFER_SOURCE);
			colorAttachmentView = device->createImageView2D(colorAttachment, 0, 1);

			depthAttachment = device->createImage2D(width, height, 1, depthFormat, ImageUsage::DEPTH_ATTACHMENT);
			depthAttachmentView = device->createImageView2D(depthAttachment, 0, 1);

			frameBuffer = device->createFrameBuffer(width, height, renderPass, {colorAttachmentView, depthAttachmentView});
		}

		Ref<graphics::ImageView2D> ShadelessForwardRenderer::getResult(u32 bufferIndex) const
		{
			return colorAttachmentView;
		}

	}

}
