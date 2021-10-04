
/*
 * VolumeLoader.cpp
 *
 *  Created on: Sep 23, 2021
 *	  Author: konstantin
 */

#include <graphics/VolumeLoader.h>
#include "openvdb/openvdb.h"
#include <openvdb/tools/Interpolation.h>

namespace matrix
{

	namespace graphics
	{

		Ref<Image3DData> loadVolume(Ref<DeviceInstance> device, const RL &rl)
		{
			MATRIX_ASSERT(rl.getProtocol() == "file")

			openvdb::initialize();

			openvdb::io::File file(rl.getPath());
			file.open();

			Ref<openvdb::GridBase> baseGrid;
			openvdb::io::File::NameIterator name = file.beginName();
			if (name != file.endName())
			{
				baseGrid = file.readGrid(name.gridName());
			}

			file.close();

			openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);

			// TODO: improve performance (there has to be a better way than iterating over every voxel)
			openvdb::Coord min = openvdb::Coord::max();
			openvdb::Coord max = openvdb::Coord::min();

			for (openvdb::FloatGrid::ValueOnCIter iter = grid->cbeginValueOn(); iter; ++iter)
			{
				const openvdb::Coord coord = iter.getCoord();
				min.minComponent(coord);
				max.maxComponent(coord);
			}

			const openvdb::Coord resolution = max - min + openvdb::Coord(1, 1, 1);

			auto format = device->createImageFormat(ImageFormatType::COLOR, 1, 4, true);

			f32 *data = new f32[resolution.x() * resolution.y() * resolution.z() * format->componentCount];
			// auto accessor = grid->getAccessor();

			// u32 index = 0;
			// for (s32 z = min.z(); z < max.z(); z++)
			// {
			// 	for (s32 y = min.y(); y < max.y(); y++)
			// 	{
			// 		for (s32 x = min.x(); x < max.x(); x++)
			// 		{
			// 			const openvdb::Coord coord(x, y, z);
			// 			data[index * format->componentCount] = accessor.getValue(coord);
			// 			index++;
			// 		}
			// 	}
			// }

			for (openvdb::FloatGrid::ValueOnCIter iter = grid->cbeginValueOn(); iter; ++iter)
			{
				const openvdb::Coord coord = iter.getCoord() - min;
				u32 index = coord.x() + coord.y() * resolution.x() + coord.z() * resolution.x() * resolution.y();
				data[index * format->componentCount] = iter.getValue();
			}

			return allocate<Image3DData>(data, resolution.x(), resolution.y(), resolution.z(), format, true);
		}

	}

}
