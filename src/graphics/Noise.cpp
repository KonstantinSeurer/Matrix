/*
 * Noise.cpp
 *
 *  Created on: Sep 11, 2021
 *      Author: konstantin
 */

#include <graphics/Noise.h>
#include <graphics/Pipeline.h>
#include <graphics/ImageLoader.h>

namespace matrix
{

	namespace graphics
	{

		static Ref<PipelineSource> source = null;
		static Ref<DescriptorSet> descriptorSet = null;
		static Ref<Sampler2D> sampler2D;
		static Array<Ref<Image2D>> blueImages2D;
		static Array<Ref<ImageView2D>> blueImageViews2D;
		static Array<Ref<ImageSampler2D>> blueImageSamplers2D;

		void initializeNoise(Ref<DeviceInstance> device)
		{
			if (source)
			{
				return;
			}

			source = parsePipeline(RL("file:///shaders/math/noise.shader"));
			addShaderInclude("math.Noise", source);

			sampler2D = device->createSampler2D(SamplingMode::NEAREST, SamplingMode::NEAREST, WrapMode::REPEAT, WrapMode::REPEAT);

			for (u32 i = 0; i < 64; i++)
			{
				auto imageData = loadImage(device, RL("file:///images/bluenoise/256/HDR_L_" + toString(i) + ".png"));
				auto image = device->createImage2D(*imageData, ImageUsage::SAMPLED);
				auto imageView = device->createImageView2D(image, 0, 1);
				auto imageSampler = allocate<ImageSampler2D>(imageView, sampler2D);

				blueImages2D.push_back(image);
				blueImageViews2D.push_back(imageView);
				blueImageSamplers2D.push_back(imageSampler);
			}

			descriptorSet = device->createDescriptorSet(source->getDescriptorSet("noise"));
			descriptorSet->access([&](DescriptorSetAccessor &accessor)
								  {
									  for (u32 levelIndex = 0; levelIndex < blueImageSamplers2D.size(); levelIndex++)
									  {
										  accessor.set("blue64", levelIndex, blueImageSamplers2D[levelIndex].get());
									  }
								  });

			device->addFreeFunction(freeNoise);
		}

		void freeNoise()
		{
			sampler2D = null;
			blueImages2D.clear();
			blueImageViews2D.clear();
		}

		Ref<PipelineSource> getNoiseSource()
		{
			return source;
		}

		Ref<DescriptorSet> getNoiseDescriptorSet()
		{
			return descriptorSet;
		}

	}

}
