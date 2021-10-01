/*
 * MaterialBuffer.h
 *
 *  Created on: Jul 14, 2021
 *      Author: konstantin
 */

#ifndef SCENE_RENDERER_SCENEMATERIALBUFFER_H_
#define SCENE_RENDERER_SCENEMATERIALBUFFER_H_

#include "../Material.h"
#include "SceneModelMap.h"

namespace matrix {

namespace scene {

struct MaterialBufferEntry {
	const MaterialType *type;
	Array<const Material*> materials;

	MaterialBufferEntry() :
		type(null) {
	}

	MaterialBufferEntry(const MaterialType *type) :
		type(type) {
	}
};

class SceneMaterialBuffer: public SceneRenderImplementation {
private:
	struct FrameData {
		Ref<graphics::DescriptorSet> descriptorSet;
		Ref<graphics::StructBuffer> typeTable;
		Ref<graphics::StructBuffer> attributes;
	};

	Array<FrameData> buffers;

	Array<MaterialBufferEntry> materials;
public:
	SceneMaterialBuffer();

	virtual ~SceneMaterialBuffer();

	virtual void init(Ref<graphics::DeviceInstance> device, u32 bufferCount);
	virtual void updateDescriptors(const Scene &scene, u32 bufferIndex);
	virtual void render(const Scene &scene);

	u32 getMaterialTypeIndex(const MaterialType *type);
	u32 getMaterialIndex(u32 typeIndex, const Material *material);
	u32 getMaterialIndex(const Material *material);

	Ref<graphics::DescriptorSet> getDescriptorSet(u32 bufferIndex) const {
		return buffers[bufferIndex].descriptorSet;
	}

	static Ref<graphics::PipelineSource> getSource();

	MATRIX_SCENE_RENDER_IMPLEMENTATION(Scene, SceneMaterialBuffer, {SceneModelMap::type()})
};

}

}

#endif /* SCENE_RENDERER_SCENEMATERIALBUFFER_H_ */
