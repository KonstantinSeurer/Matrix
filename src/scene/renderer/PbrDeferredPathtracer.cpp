/*
 * ShadelessForwardRenderer.cpp
 *
 *  Created on: Jul 9, 2021
 *      Author: konstantin
 */

#include <scene/renderer/PbrDeferredPathtracer.h>

#include <scene/renderer/ModelVertexArray.h>
#include <Logging.h>
#include <scene/GLTF.h>
#include <scene/renderer/SceneAccelerationStructure.h>
#include <math/Base.h>
#include <scene/renderer/SceneCameraBuffer.h>
#include <scene/renderer/SceneMaterialBuffer.h>
#include <scene/renderer/SceneModelMap.h>
#include <scene/renderer/SceneLightBuffer.h>
#include <scene/renderer/SceneModelBuffer.h>
#include <graphics/Noise.h>

#include <graphics/VolumeLoader.h>

namespace matrix
{

	using namespace graphics;

	namespace scene
	{

		PbrDeferredPathtracer::PbrDeferredPathtracer(Ref<DeviceInstance> device, Ref<SkyRenderer> skyRenderer) : skyRenderer(skyRenderer), device(device)
		{
			gBufferRenderer = allocate<DeferredRenderer>(device);
			gBufferSampler = device->createSampler2D(SamplingMode::NEAREST, SamplingMode::NEAREST, WrapMode::CLAMP, WrapMode::CLAMP);

			skySampler = device->createSampler2D(SamplingMode::LINEAR, SamplingMode::NEAREST, WrapMode::REPEAT, WrapMode::CLAMP);
			Ref<ImageSampler2D> environmentSampler = allocate<ImageSampler2D>(skyRenderer->getEnvironmentView(), skySampler);
			Ref<ImageSampler2D> transmittanceSampler = allocate<ImageSampler2D>(skyRenderer->getTransmittanceView(), skySampler);

			resultFormat = device->createImageFormat(ImageFormatType::COLOR, 4, 1, false);

			registerGLTFShaderInclude();
			SceneCameraBuffer::getSource();
			SceneAccelerationStructure::getSource();
			SceneLightBuffer::getSource();
			ModelModelBuffer::getSource();
			initializeNoise(device);

			Ref<ComputePipelineSource> source = parseComputePipeline(RL("file:///shaders/scene/renderer/pbrTraced.shader"));

			auto volume = loadVolume(device, RL("file:///volumes/cloud.vdb"));
			volumeImage = device->createImage3D(*volume, ImageUsage::SAMPLED);
			volumeImageView = device->createImageView3D(volumeImage, 0, 1);
			volumeSampler = device->createSampler3D(SamplingMode::LINEAR, SamplingMode::NEAREST, WrapMode::CLAMP, WrapMode::CLAMP, WrapMode::CLAMP);
			volumeImageSampler = allocate<ImageSampler3D>(volumeImageView, volumeSampler);

			descriptorSet = device->createDescriptorSet(source->getDescriptorSet("data"));
			descriptorSet->access(
				[environmentSampler, transmittanceSampler, this](DescriptorSetAccessor &accessor)
				{
					accessor.set("environment", environmentSampler.get());
					accessor.set("transmittance", transmittanceSampler.get());
					accessor.set("volume", volumeImageSampler.get());
				});

			pipeline = device->createComputePipeline(source);
		}

		PbrDeferredPathtracer::~PbrDeferredPathtracer()
		{
		}

		void PbrDeferredPathtracer::updateDescriptors(u32 bufferIndex)
		{
		}

		void PbrDeferredPathtracer::render(Ref<graphics::CommandBuffer> cb, const Scene &scene, ComponentReference camera, u32 bufferIndex)
		{
			gBufferRenderer->render(cb, scene, camera, bufferIndex);
			skyRenderer->render(cb, scene, camera, bufferIndex);

			cb->barrier(
				{ImageLayoutTransition(gBufferRenderer->getAlbedoMetallic(), ImageLayout::COLOR_ATTACHMENT, ImageLayout::SHADER_READ_ONLY),
				 ImageLayoutTransition(gBufferRenderer->getNormalRoughness(), ImageLayout::COLOR_ATTACHMENT, ImageLayout::SHADER_READ_ONLY),
				 ImageLayoutTransition(gBufferRenderer->getEmissiveOcclusion(), ImageLayout::COLOR_ATTACHMENT, ImageLayout::SHADER_READ_ONLY),
				 ImageLayoutTransition(gBufferRenderer->getDepth(), ImageLayout::DEPTH_ATTACHMENT, ImageLayout::SHADER_READ_ONLY),
				 ImageLayoutTransition(skyRenderer->getEnvironment(), ImageLayout::SHADER_STORAGE, ImageLayout::SHADER_READ_ONLY),
				 ImageLayoutTransition(skyRenderer->getTransmittance(), ImageLayout::SHADER_STORAGE, ImageLayout::SHADER_READ_ONLY)});

			auto models = scene.getRenderImplementation<SceneModelMap>();
			auto materials = scene.getRenderImplementation<SceneMaterialBuffer>();
			auto cameras = scene.getRenderImplementation<SceneCameraBuffer>();
			auto accelerationStructure = scene.getRenderImplementation<SceneAccelerationStructure>();
			auto lightBuffer = scene.getRenderImplementation<SceneLightBuffer>();
			auto modelBuffer = scene.getRenderImplementation<SceneModelBuffer>();

			cb->barrier({ImageLayoutTransition(result, ImageLayout::UNDEFINED, ImageLayout::SHADER_STORAGE)});

			cb->computePipeline(
				pipeline, [&]()
				{
					cb->descriptorSets(
						{materials->getDescriptorSet(bufferIndex), cameras->getDescriptorSet(bufferIndex), accelerationStructure->getDescriptorSet(bufferIndex),
						 lightBuffer->getDescriptorSet(bufferIndex), modelBuffer->getDescriptorSet(bufferIndex), getNoiseDescriptorSet(), descriptorSet},
						[&]()
						{
							cb->uniforms(
								[&](StructAccessor accessor)
								{
									accessor.setInt("camera", camera.index);
									accessor.setUInt("seed", frameIndex);
								});
							cb->compute({result->width, result->height, 1}, ComputeSize::GLOBAL);
						});
				});

			cb->barrier(
				{ImageLayoutTransition(result, ImageLayout::SHADER_STORAGE, ImageLayout::TRANSFER_SOURCE),
				 ImageLayoutTransition(skyRenderer->getEnvironment(), ImageLayout::SHADER_READ_ONLY, ImageLayout::SHADER_STORAGE),
				 ImageLayoutTransition(skyRenderer->getTransmittance(), ImageLayout::SHADER_READ_ONLY, ImageLayout::SHADER_STORAGE)});

			frameIndex++;
		}

		void PbrDeferredPathtracer::resize(u32 width, u32 height)
		{
			gBufferRenderer->resize(width, height);

			albedoMetallicSampler = allocate<ImageSampler2D>(gBufferRenderer->getAlbedoMetallicView(), gBufferSampler);
			normalRoughnessSampler = allocate<ImageSampler2D>(gBufferRenderer->getNormalRoughnessView(), gBufferSampler);
			emissiveOcclusionSampler = allocate<ImageSampler2D>(gBufferRenderer->getEmissiveOcclusionView(), gBufferSampler);
			depthSampler = allocate<ImageSampler2D>(gBufferRenderer->getDepthView(), gBufferSampler);

			result = device->createImage2D(width, height, 1, resultFormat, ImageUsage::STORAGE | ImageUsage::TRANSFER_SOURCE);
			resultView = device->createImageView2D(result, 0, 1);

			descriptorSet->access(
				[this](DescriptorSetAccessor &accessor)
				{
					accessor.set("result", resultView.get());
					accessor.set("albedoMetallic", albedoMetallicSampler.get());
					accessor.set("normalRoughness", normalRoughnessSampler.get());
					accessor.set("emissiveOcclusion", emissiveOcclusionSampler.get());
					accessor.set("depth", depthSampler.get());
				});
		}

		Ref<graphics::ImageView2D> PbrDeferredPathtracer::getResult(u32 bufferIndex) const
		{
			return resultView;
		}

	}

}
