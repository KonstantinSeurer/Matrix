/*
 * Model.h
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#ifndef SCENE_MODEL_H_
#define SCENE_MODEL_H_

#include "Material.h"

#include "Scene.h"

#include "../math/Vec3.h"
#include "../math/Vec2.h"
#include "../math/Vec4.h"
#include "../math/Mat2.h"
#include "../math/Mat3.h"
#include "../math/Mat4.h"

#include "Transform.h"

#include "../Logging.h"

namespace matrix {

namespace scene {

class ModelType {
private:
	Array<Pair<graphics::PrimitiveTypeType, String>> vertexAttributes;
	UnorderedMap<String, u32> offsets;
	u32 positionAttributeIndex = 0;
	bool hasPositionAttribute = false;
	u32 floatAttributeCount = 0;
	u32 intAttributeCount = 0;
public:
	void addAttribute(const Pair<graphics::PrimitiveTypeType, String> &attribute);

	void setPositionAttribute(const Pair<graphics::PrimitiveTypeType, String> &attribute);

	const Array<Pair<graphics::PrimitiveTypeType, String>>& getAttributes() const {
		return vertexAttributes;
	}

	u32 getPositionAttributeIndex() const {
		return positionAttributeIndex;
	}

	u32 getFloatAttributeCount() const {
		return floatAttributeCount;
	}

	u32 getIntAttributeCount() const {
		return intAttributeCount;
	}

	u32 getAttributeCount() const {
		return getFloatAttributeCount() + getIntAttributeCount();
	}

	u32 getOffset(const String &name) const {
		return offsets.at(name);
	}
};

class ModelRenderImplementation: public RenderImplementation {
private:
public:
	ModelRenderImplementation() {
	}

	virtual ~ModelRenderImplementation() {
	}

	virtual void init(const ModelType *type, Ref<graphics::DeviceInstance> device, const Array<void*> &attributes, u32 vertexCount,
		const Array<u32> &indices) = 0;
};

class Model : public Renderable {
private:
	const ModelType *type;
	Ref<const Material> material;
public:
	Model(const ModelType *type, Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes,
		const Array<void*> &attributes, u32 vertexCount, const Array<u32> &indices, Ref<const Material> material);

	Model(const Model &model, Ref<const Material> material);

	Model(const Model &model);

	Ref<const Material> getMaterial() const {
		return material;
	}

	const ModelType *getType() const {
		return type;
	}
};

class ModelComponent: public Component {
private:
	Ref<Model> model;
	Entity *parent;
public:

	ModelComponent() :
		model(null), parent(null) {
	}

	ModelComponent(Ref<Model> model) :
		model(model), parent(null) {
	}

	virtual ~ModelComponent() {
		model = null;
	}

	ModelComponent& setModel(Ref<Model> model);

	virtual void init(Entity *parent, Scene *scene);

	Ref<Model> getModel() const {
		return model;
	}

	const TransformComponent* getTransform() const;

	MATRIX_SCENE_COMPONENT(ModelComponent, null,
		{	TransformComponent::type()})
};

}

}

#endif /* SCENE_MODEL_H_ */
