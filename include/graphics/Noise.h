/*
 * Noise.h
 *
 *  Created on: Sep 11, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_NOISE_H_
#define GRAPHICS_NOISE_H_

#include "Device.h"

namespace matrix
{

namespace graphics
{

void initializeNoise(Ref<DeviceInstance> device);
void freeNoise();

Ref<PipelineSource> getNoiseSource();
Ref<DescriptorSet> getNoiseDescriptorSet();

}

}

#endif /* GRAPHICS_NOISE_H_ */
