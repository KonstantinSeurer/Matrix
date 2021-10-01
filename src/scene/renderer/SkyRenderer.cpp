/*
 * SkyRenderer.cpp
 *
 *  Created on: Sep 3, 2021
 *      Author: konstantin
 */

#include <scene/renderer/SkyRenderer.h>

namespace matrix
{

	using namespace graphics;

	namespace scene
	{

		SkyRenderer::SkyRenderer(Ref<graphics::DeviceInstance> device, u32 resolution)
		{
			auto environmentFormat = device->createImageFormat(ImageFormatType::COLOR, 4, 4, true);
			environment = device->createImage2D(resolution * 2, resolution, 1, environmentFormat, ImageUsage::STORAGE | ImageUsage::SAMPLED);
			environmentView = device->createImageView2D(environment, 0, 1);

			auto transmittanceFormat = device->createImageFormat(ImageFormatType::COLOR, 4, 1, false);
			transmittance = device->createImage2D(resolution * 2, resolution, 1, transmittanceFormat, ImageUsage::STORAGE | ImageUsage::SAMPLED);
			transmittanceView = device->createImageView2D(transmittance, 0, 1);
		}

	}

}
