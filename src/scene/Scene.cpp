/*
 * Scene.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#include "Scene.h"
#include "../Logging.h"

namespace matrix {

namespace scene {

void Renderable::init(const Array<RenderImplementationType> &renderImplementationTypes) {
	for (const auto &desc : TypeRegistry::getRenderImplementationTypeDescriptions()) {
		if (find(renderImplementationTypes.begin(), renderImplementationTypes.end(), desc.type) != renderImplementationTypes.end()) {
			auto impl = desc.createFunction(this);
			if (!impl) {
				continue;
			}
			renderImplementations.push_back(impl);
		}
	}
}

Array<ComponentTypeDescription> TypeRegistry::componentTypes = { };
Array<RenderImplementationTypeDescription> TypeRegistry::renderImplementationTypes = { };

void Entity::init(Scene *scene, Entity *parent) {
	this->scene = scene;
	this->parent = parent;
	for (auto componentRef : components) {
		scene->initComponent(*componentRef, this);
	}
	for (auto child : children) {
		child->init(scene, this);
	}
}

const ComponentReference* Entity::getComponentReference(ComponentType type) const {
	for (auto componentRef : components) {
		if (componentRef->type == type) {
			return componentRef;
		}
	}
	return null;
}

void Entity::addComponent(ComponentReference *component) {
	components.push_back(component);
}

Scene::Scene(Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes, u32 bufferCount) :
	renderImplementationTypes(renderImplementationTypes) {
	components.resize(TypeRegistry::getComponentTypeDescriptions().size());
	componentReferences.resize(TypeRegistry::getComponentTypeDescriptions().size());

	for (auto &description : TypeRegistry::getComponentTypeDescriptions()) {
		components[description.type] = description.createArrayFunction();
		componentReferences[description.type] = { };
	}

	init(renderImplementationTypes);

	for (u32 implementationIndex = 0; implementationIndex < getRenderImplementationCount(); implementationIndex++) {
		getRenderImplementation<SceneRenderImplementation>(implementationIndex)->init(device, bufferCount);
	}
}

Scene::~Scene() {
	for (ComponentType type = 0; type < components.size(); type++) {
		TypeRegistry::getComponentTypeDescription(type).destructFunction(components[type]);
	}
}

void Scene::update(const UpdateData &data) {
	for (ComponentType type = 0; type < components.size(); type++) {
		TypeRegistry::getComponentTypeDescription(type).updateFunction(components[type], data);
	}
}

void Scene::updateDescriptors(u32 bufferIndex) {
	for (u32 implementationIndex = 0; implementationIndex < getRenderImplementationCount(); implementationIndex++) {
		getRenderImplementation<SceneRenderImplementation>(implementationIndex)->updateDescriptors(*this, bufferIndex);
	}
}

void Scene::render() {
	for (u32 implementationIndex = 0; implementationIndex < getRenderImplementationCount(); implementationIndex++) {
		getRenderImplementation<SceneRenderImplementation>(implementationIndex)->render(*this);
	}
}

Ref<Entity> Scene::createEntity() {
	return allocate<Entity>(this);
}

void Scene::add(Ref<Entity> entity) {
	entity->init(this, null);
	entities.push_back(entity);
}

void Scene::remove(Ref<Entity> entity) {
	for (auto child : entity->getChildren()) {
		remove(child);
	}
	entities.erase(find(entities.begin(), entities.end(), entity));
	for (auto componentRef : entity->getComponents()) {
		removeComponent(*componentRef);
	}
}

void Scene::removeComponent(ComponentReference ref) {
	TypeRegistry::getComponentTypeDescription(ref.type).removeFunction(components[ref.type], ref.index);
	auto &references = componentReferences[ref.type];
	references.erase(references.begin() + ref.index);
	for (u32 i = ref.index; i < references.size(); i++) {
		references[i]->index = i;
	}
}

void Scene::initComponent(ComponentReference ref, Entity *entity) {
	auto &type = TypeRegistry::getComponentTypeDescription(ref.type);
	type.initFunction(components[ref.type], ref.index, entity, this);
}

}

}
