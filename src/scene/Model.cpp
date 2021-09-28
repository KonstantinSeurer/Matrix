/*
 * Model.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#include "Model.h"
#include "Transform.h"
#include "../Logging.h"

namespace matrix {

using namespace graphics;

namespace scene {

static UnorderedMap<PrimitiveTypeType, u32> floatAttributeSizes = { { PrimitiveTypeType::FLOAT, 1 }, { PrimitiveTypeType::VEC2, 2 }, { PrimitiveTypeType::VEC3,
	3 }, { PrimitiveTypeType::VEC4, 4 }, { PrimitiveTypeType::MAT2, 4 }, { PrimitiveTypeType::MAT3, 9 }, { PrimitiveTypeType::MAT4, 16 }, {
	PrimitiveTypeType::INT, 0 } };

static UnorderedMap<PrimitiveTypeType, u32> intAttributeSizes = { { PrimitiveTypeType::FLOAT, 0 }, { PrimitiveTypeType::VEC2, 0 },
	{ PrimitiveTypeType::VEC3, 0 }, { PrimitiveTypeType::VEC4, 0 }, { PrimitiveTypeType::MAT2, 0 }, { PrimitiveTypeType::MAT3, 0 },
	{ PrimitiveTypeType::MAT4, 0 }, { PrimitiveTypeType::INT, 1 } };

void ModelType::addAttribute(const Pair<graphics::PrimitiveTypeType, String> &attribute) {
	vertexAttributes.push_back(attribute);

	offsets[attribute.second] = getAttributeCount();

	floatAttributeCount += floatAttributeSizes[attribute.first];
	intAttributeCount += intAttributeSizes[attribute.first];
}

void ModelType::setPositionAttribute(const Pair<graphics::PrimitiveTypeType, String> &attribute) {
	if (hasPositionAttribute) {
		return;
	}
	positionAttributeIndex = vertexAttributes.size();
	addAttribute(attribute);
	hasPositionAttribute = true;
}

Model::Model(const ModelType *type, Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes,
	const Array<void*> &attributes, u32 vertexCount, const Array<u32> &indices, Ref<const Material> material) :
	type(type), material(material) {

	init(renderImplementationTypes);

	for (u32 implementationIndex = 0; implementationIndex < getRenderImplementationCount(); implementationIndex++) {
		getRenderImplementation<ModelRenderImplementation>(implementationIndex)->init(type, device, attributes, vertexCount, indices);
	}
}

Model::Model(const Model &model, Ref<const Material> material) :
	Renderable(model), type(model.type), material(material) {

}

Model::Model(const Model &model) :
	Renderable(model), type(model.type), material(model.material) {

}

ModelComponent& ModelComponent::setModel(Ref<Model> model) {
	this->model = model;
	return *this;
}

void ModelComponent::init(Entity *parent, Scene *scene) {
	this->parent = parent;
}

const TransformComponent* ModelComponent::getTransform() const {
	return parent->getComponent<TransformComponent>();
}

}

}
