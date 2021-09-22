/*
 * Scene.h
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#ifndef SCENE_SCENE_H_
#define SCENE_SCENE_H_

#include "../Base.h"
#include "../Time.h"
#include "../graphics/Device.h"
#include "../Events.h"

namespace matrix {

namespace scene {

class Scene;
class Entity;

using ComponentType = u32;
using RenderImplementationType = u32;

struct ComponentReference {
	ComponentType type;
	u32 index;
};

struct ComponentTypeDescription {
	Function<Any()> createArrayFunction;
	Function<ComponentReference* (Any)> createFunction;
	Function<void(Any, u32, Entity*, Scene*)> initFunction;
	Function<void(Any, const UpdateData&)> updateFunction;
	Function<void(Any, u32)> removeFunction;
	Function<void(Any)> destructFunction;
	Array<ComponentType> dependencies;
	ComponentType type;
};

class RenderImplementation {
private:
public:
	RenderImplementation() {
	}

	virtual ~RenderImplementation() {
	}
};

class Renderable {
private:
	Array<Ref<RenderImplementation>> renderImplementations;
public:
	Renderable() {
	}

	Renderable(const Renderable &renderable) :
		renderImplementations(renderable.renderImplementations) {
	}

	void init(const Array<RenderImplementationType> &renderImplementationTypes);

	virtual ~Renderable() {
	}

	u32 getRenderImplementationCount() const {
		return renderImplementations.size();
	}

	template<typename T> T* getRenderImplementation(u32 index) const {
		return CastDown<T>(renderImplementations[index].get());
	}

	template<typename T> Ref<T> getRenderImplementation() const {
		for (auto impl : renderImplementations) {
			if (Ref<T> result = CastDown<T>(impl)) {
				return result;
			}
		}
		return null;
	}
};

struct RenderImplementationTypeDescription {
	Function<Ref<RenderImplementation>(Renderable*)> createFunction;
	Array<RenderImplementationType> dependencies;
	RenderImplementationType type;
};

class TypeRegistry {
private:
	static Array<ComponentTypeDescription> componentTypes;
	static Array<RenderImplementationTypeDescription> renderImplementationTypes;
public:
	template<typename ParentType, typename Type> static RenderImplementationType registerRenderImplementation(
		const Array<RenderImplementationType> &dependencies) {
		RenderImplementationTypeDescription desc;
		desc.type = renderImplementationTypes.size();
		desc.dependencies = dependencies;
		desc.createFunction = [](Renderable *parent) -> Ref<Type> {
			if (!CastDown<ParentType>(parent)) {
				return null;
			}
			return allocate<Type>();
		};
		renderImplementationTypes.push_back(desc);
		return desc.type;
	}

	template<typename T> static ComponentType registerComponent(Function<void(Array<T>&, const UpdateData&)> updateFunction,
		const Array<ComponentType> &dependencies) {
		ComponentTypeDescription type;
		type.type = componentTypes.size();
		type.createArrayFunction = []() {
			return new Array<T>();
		};
		type.createFunction = [type](Any target) {
			auto components = CastDown<Array<T>>(target);
			ComponentReference *ref = new ComponentReference;
			ref->type = type.type;
			ref->index = components->size();
			components->push_back(T());
			return ref;
		};
		type.initFunction = [](Any components, u32 index, Entity *parent, Scene *scene) {
			CastDown<Array<T>>(components)->operator[](index).init(parent, scene);
		};
		type.updateFunction = [updateFunction](Any components, const UpdateData &data) {
			if (updateFunction) {
				updateFunction(*CastDown<Array<T>>(components), data);
			}
		};
		type.removeFunction = [](Any components, u32 index) {
			auto c = CastDown<Array<T>>(components);
			c->erase(c->begin() + index);
		};
		type.destructFunction = [](Any components) {
			auto c = CastDown<Array<T>>(components);
			delete c;
		};
		componentTypes.push_back(type);
		return type.type;
	}

	static const ComponentTypeDescription& getComponentTypeDescription(ComponentType type) {
		return componentTypes[type];
	}

	static const Array<ComponentTypeDescription>& getComponentTypeDescriptions() {
		return componentTypes;
	}

	static const Array<RenderImplementationTypeDescription>& getRenderImplementationTypeDescriptions() {
		return renderImplementationTypes;
	}
};

class Component {
public:
	Component() {
	}

	virtual ~Component() {
	}

	virtual void init(Entity *parent, Scene *scene) {
	}
};

#define MATRIX_SCENE_COMPONENT(Type, updateFunction, dependencies) static matrix::scene::ComponentType type() {\
	static matrix::scene::ComponentType TYPE =\
	matrix::scene::TypeRegistry::registerComponent<Type>(updateFunction, dependencies);\
	return TYPE;}

class SceneRenderImplementation: public RenderImplementation {
private:
public:
	SceneRenderImplementation() {
	}

	virtual ~SceneRenderImplementation() {
	}

	virtual void init(Ref<graphics::DeviceInstance> device, u32 bufferCount) {
	}

	virtual void updateDescriptors(const Scene &scene, u32 bufferIndex) {
	}

	virtual void render(const Scene &scene) {
	}
};

#define MATRIX_SCENE_RENDER_IMPLEMENTATION(ParentType, Type, dependencies) static matrix::scene::RenderImplementationType type() {\
	static matrix::scene::RenderImplementationType TYPE =\
	matrix::scene::TypeRegistry::registerRenderImplementation<ParentType, Type>(dependencies);\
	return TYPE;}

class Scene: public Renderable {
private:
	Array<Any> components;
	Array<Array<ComponentReference*>> componentReferences;
	Array<Ref<Entity>> entities;
	Array<RenderImplementationType> renderImplementationTypes;
public:
	Scene(Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes, u32 bufferCount);
	~Scene();

	const Array<RenderImplementationType>& getRenderImplementationTypes() const {
		return renderImplementationTypes;
	}

	void update(const UpdateData &data);

	void updateDescriptors(u32 bufferIndex);
	void render();

	template<typename T, typename ... Args> ComponentReference* createComponent(Args &&... args) {
		ComponentType type = T::type();

		Array<T> *target = CastDown<Array<T>>(components[type]);

		ComponentReference *ref = new ComponentReference();
		ref->type = type;
		ref->index = target->size();
		componentReferences[type].push_back(ref);

		target->emplace_back(std::forward<Args>(args)...);

		return ref;
	}

	Ref<Entity> createEntity();

	void add(Ref<Entity> entity);
	void remove(Ref<Entity> entity);

	void removeComponent(ComponentReference ref);

	void initComponent(ComponentReference ref, Entity *entity);

	template<typename T> T* getComponent(ComponentReference ref) {
		return &CastDown<Array<T>>(components[ref.type])->operator[](ref.index);
	}

	template<typename T> const T* getComponent(ComponentReference ref) const {
		return &CastDown<Array<T>>(components[ref.type])->operator[](ref.index);
	}

	template<typename T> const Array<T>& getComponents(ComponentType type) const {
		return *CastDown<Array<T>>(components[type]);
	}

	template<typename T> const Array<T>& getComponents() const {
		return getComponents<T>(T::type());
	}
};

class Entity {
private:
	Scene *scene;
	Entity *parent;
	Array<Ref<Entity>> children;
	Array<ComponentReference*> components;
public:
	Entity(Scene *scene) :
		scene(scene), parent(null) {
	}

	void init(Scene *scene, Entity *parent);

	void addComponent(ComponentReference *component);

	template<typename T, typename ... Args> T* addComponent(Args &&... args) {
		ComponentReference *ref = scene->createComponent<T>(std::forward<Args>(args)...);
		addComponent(ref);
		return scene->getComponent<T>(*ref);
	}

	void addEntity(Ref<Entity> entity) {
		children.push_back(entity);
	}

	Scene* getScene() {
		return scene;
	}

	const Scene* getScene() const {
		return scene;
	}

	Entity* getParent() {
		return parent;
	}

	const Entity* getParent() const {
		return parent;
	}

	const Array<Ref<Entity>>& getChildren() const {
		return children;
	}

	const Array<ComponentReference*>& getComponents() const {
		return components;
	}

	const ComponentReference* getComponentReference(ComponentType type) const;

	template<typename T> const ComponentReference* getComponentReference() const {
		return getComponentReference(T::type());
	}

	template<typename T> T* getComponent() {
		const ComponentReference *ref = getComponentReference(T::type());
		if (ref) {
			return scene->getComponent<T>(*ref);
		}
		return null;
	}

	template<typename T> const T* getComponent() const {
		const ComponentReference *ref = getComponentReference(T::type());
		if (ref) {
			return scene->getComponent<T>(*ref);
		}
		return null;
	}
};

}

}

#endif /* SCENE_SCENE_H_ */
