#include <graphics/RenderWindow.h>
#include <Time.h>
#include <Logging.h>
#include <RL.h>
#include <scene/Scene.h>
#include <scene/Model.h>
#include <scene/Camera.h>
#include <scene/GLTF.h>
#include <scene/Transform.h>
#include <scene/Light.h>
#include <scene/renderer/ModelVertexArray.h>
#include <scene/renderer/ModelAccelerationStructure.h>
#include <scene/renderer/SceneAccelerationStructure.h>
#include <Events.h>
#include <math/Base.h>
#include <scene/renderer/PbrDeferredPathtracer.h>
#include <scene/renderer/PhysicalSkyRenderer.h>
#include <scene/renderer/SceneCameraBuffer.h>
#include <scene/renderer/SceneMaterialBuffer.h>
#include <scene/renderer/SceneModelMap.h>
#include <scene/renderer/SceneLightBuffer.h>
#include <scene/renderer/ModelModelBuffer.h>
#include <scene/renderer/SceneModelBuffer.h>
#include "DemoCamera.h"

using namespace matrix;
using namespace graphics;
using namespace math;
using namespace scene;
using namespace input;

void skyMain()
{
	RenderWindowSettings settings;
	RenderWindow window(settings);

	auto device = window.getDeviceInstance();

	TransformComponent::type();
	ModelComponent::type();
	CameraComponent::type();
	DirectionalLightComponent::type();
	Array<RenderImplementationType> renderImplementations = {SceneModelMap::type(), SceneMaterialBuffer::type(), SceneCameraBuffer::type(),
															 ModelVertexArray::type(), ModelAccelerationStructure::type(), SceneAccelerationStructure::type(), SceneLightBuffer::type(), ModelModelBuffer::type(),
															 SceneModelBuffer::type()};

	Ref<SkyRenderer> skyRenderer = allocate<PhysicalSkyRenderer>(device, 256);
	Ref<Renderer> renderer = allocate<PbrDeferredPathtracer>(device, skyRenderer);
	renderer->resize(window.getWidth(), window.getHeight());

	Ref<Scene> scene = allocate<Scene>(device, renderImplementations, window.getBufferCount());

	Ref<Entity> sunEntity = scene->createEntity();
	DirectionalLightComponent *sun = sunEntity->addComponent<DirectionalLightComponent>(Vec3(10.0f), Vec3(0.0f, -1.0f, 0.0f));
	scene->add(sunEntity);

	DemoCamera camera(*scene);
	camera.resize(window.getWidth(), window.getHeight());
	camera.getTransform()->position.z = -5.0f;

	window.callbacks.update = [&](const UpdateData &data)
	{
		const f64 sunRotationX = radians(45.0f) + data.time.seconds() * 0.0;
		const f32 sunRotationY = radians(30.0f);
		sun->direction = Vec3(-sin(sunRotationY) * sin(sunRotationX), -cos(sunRotationX), -cos(sunRotationY) * sin(sunRotationX));

		camera.update(data);
		scene->update(data);
	};

	window.callbacks.updateDescriptors = [&](u32 bufferIndex)
	{
		scene->updateDescriptors(bufferIndex);
		renderer->updateDescriptors(bufferIndex);
	};

	window.callbacks.getRenderResult = [&](u32 bufferIndex)
	{
		return renderer->getResult(bufferIndex);
	};

	window.callbacks.render = [&](Ref<CommandBuffer> commandBuffer, u32 bufferIndex)
	{
		scene->render();
		renderer->render(commandBuffer, *scene, camera.getCameraReference(), bufferIndex);
	};

	window.run();
}
