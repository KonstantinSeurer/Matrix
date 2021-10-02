
/*
 * VolumeLoader.h
 *
 *  Created on: Sep 23, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VOLUMELOADER_H_
#define GRAPHICS_VOLUMELOADER_H_

#include "Image.h"
#include "Device.h"

namespace matrix
{

    namespace graphics
    {

        Ref<Image3DData> loadVolume(Ref<DeviceInstance> device, const RL &rl);

    }

}

#endif /* GRAPHICS_VOLUMELOADER_H_ */
