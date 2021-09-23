/*
 * VolumeLoader.cpp
 *
 *  Created on: Sep 23, 2021
 *      Author: konstantin
 */

#include "VolumeLoader.h"
#include "openvdb/openvdb.h"

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

	f32 *data = new f32[resolution.x() * resolution.y() * resolution.z()];
	for (openvdb::FloatGrid::ValueOnCIter iter = grid->cbeginValueOn(); iter; ++iter)
	{
		const openvdb::Coord coord = iter.getCoord() - min;
		u32 index = coord.x() + coord.y() * resolution.x() + coord.z() * resolution.x() * resolution.y();
		data[index] = iter.getValue();
	}

	return allocate<Image3DData>(resolution.x(), resolution.y(), resolution.z(), format, data);
}

}

}
