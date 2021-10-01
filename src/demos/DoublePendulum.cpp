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

void doublePendulumMain()
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
	camera.getTransform()->position = Vec3(0.0f, -0.75f, -3.0f);

	Ref<Material> defaultMaterial = createDefaultGLTFMaterial(device);

	Ref<Material> material = createDefaultGLTFMaterial(device);
	Ref<Sampler2D> materialSampler = device->createSampler2D(SamplingMode::NEAREST, SamplingMode::NEAREST, WrapMode::REPEAT, WrapMode::REPEAT);
	Ref<ImageFormat> albedoFormat = device->createImageFormat(ImageFormatType::COLOR, 4, 1, false);
	Ref<Image2D> albedo = device->createImage2D(4, 2, 1, albedoFormat, ImageUsage::ACCESS | ImageUsage::SAMPLED);
	albedo->access([](Image2DData accessor)
				   {
					   accessor.setUnorm8(0, 0, 0, 0, 0, 0xFF);
					   accessor.setUnorm8(1, 0, 0xFF, 0xFF, 0xFF, 0xFF);
					   accessor.setUnorm8(0, 1, 0xFF, 0xFF, 0xFF, 0xFF);
					   accessor.setUnorm8(1, 1, 0, 0, 0, 0xFF);
					   accessor.setUnorm8(2, 0, 0, 0, 0, 0xFF);
					   accessor.setUnorm8(3, 0, 0xFF, 0xFF, 0xFF, 0xFF);
					   accessor.setUnorm8(2, 1, 0xFF, 0xFF, 0xFF, 0xFF);
					   accessor.setUnorm8(3, 1, 0, 0, 0, 0xFF);
				   },
				   ImageLayout::SHADER_READ_ONLY);
	Ref<ImageView2D> albedoView = device->createImageView2D(albedo, 0, 1);
	Ref<ImageSampler2D> albedoSampler = allocate<ImageSampler2D>(albedoView, materialSampler);
	material->setImageSampler2D("baseColorTexture", albedoSampler);

	Ref<Model> cube = createGLTFCube(device, renderImplementations, 1.0f, defaultMaterial);
	Ref<Model> sphere = createGLTFSphere(device, renderImplementations, 1.0f, 32, material);
	Ref<Model> joint = createGLTFCylinder(device, renderImplementations, 1.0f, 1.0f, 32, defaultMaterial);

	Ref<Entity> anchorEntity = scene->createEntity();
	anchorEntity->addComponent<ModelComponent>(sphere);
	anchorEntity->addComponent<TransformComponent>()->scale = 0.1f;
	scene->add(anchorEntity);

	Ref<Entity> groundEntity = scene->createEntity();
	groundEntity->addComponent<ModelComponent>(cube);
	groundEntity->addComponent<TransformComponent>(Vec3(0.0f, -5.0f, 0.0f), Vec3(1000.0f, 0.1f, 1000.0f));
	scene->add(groundEntity);

	Ref<Entity> p1Entity = scene->createEntity();
	p1Entity->addComponent<ModelComponent>(sphere);
	p1Entity->addComponent<TransformComponent>(Vec3(1.0f, 0.0f, 0.0f), Vec3(0.1f));
	scene->add(p1Entity);

	Ref<Entity> p2Entity = scene->createEntity();
	p2Entity->addComponent<ModelComponent>(sphere);
	p2Entity->addComponent<TransformComponent>(Vec3(2.0f, 0.0f, 0.0f), Vec3(0.1f));
	scene->add(p2Entity);

	const f32 l1 = 1.0f;
	const f32 l2 = 0.5f;
	const f32 m1 = 1.0f;
	const f32 m2 = 0.1f;

	Ref<Entity> joint1Entity = scene->createEntity();
	joint1Entity->addComponent<ModelComponent>(joint);
	joint1Entity->addComponent<TransformComponent>(Vec3(), Vec3(0.02f, l1 * 0.5f, 0.02f));
	scene->add(joint1Entity);

	Ref<Entity> joint2Entity = scene->createEntity();
	joint2Entity->addComponent<ModelComponent>(joint);
	joint2Entity->addComponent<TransformComponent>(Vec3(), Vec3(0.02f, l2 * 0.5f, 0.02f));
	scene->add(joint2Entity);

	f32 dTheta1 = 0.0f;
	f32 dTheta2 = 0.0f;
	f32 ddTheta1 = 0.0f;
	f32 ddTheta2 = 0.0f;
	f32 theta1 = radians(90.0f);
	f32 theta2 = radians(90.0f);

	window.callbacks.update = [&](const UpdateData &data)
	{
		Vec3 &p1 = p1Entity->getComponent<TransformComponent>()->position;
		p1.x = sin(theta1) * l1;
		p1.y = -cos(theta1) * l1;

		Vec3 &p2 = p2Entity->getComponent<TransformComponent>()->position;
		p2.x = p1.x + sin(theta2) * l2;
		p2.y = p1.y - cos(theta2) * l2;

		TransformComponent *joint1 = joint1Entity->getComponent<TransformComponent>();
		joint1->position = p1 * 0.5f;
		joint1->rotation = Quat(Vec3(0.0f, 0.0f, 1.0f), theta1);

		TransformComponent *joint2 = joint2Entity->getComponent<TransformComponent>();
		joint2->position = (p1 + p2) * 0.5f;
		joint2->rotation = Quat(Vec3(0.0f, 0.0f, 1.0f), theta2);

		ddTheta1 = -m2 / (m1 + m2) * l2 / l1 * (ddTheta2 * cos(theta1 - theta2) + dTheta2 * dTheta2 * sin(theta1 - theta2)) - 9.81f / l1 * sin(theta1);
		ddTheta2 = -l1 / l2 * (ddTheta1 * cos(theta1 - theta2) - dTheta1 * dTheta1 * sin(theta1 - theta2)) - 9.81f / l2 * sin(theta2);

		f32 delta = data.delta.seconds();
		dTheta1 += ddTheta1 * delta;
		dTheta2 += ddTheta2 * delta;
		theta1 += dTheta1 * delta;
		theta2 += dTheta2 * delta;

		f32 dampening = 0.2f;
		dTheta1 -= dTheta1 * dampening * delta;
		dTheta2 -= dTheta2 * dampening * delta;

		const f64 sunRotationX = radians(85.0f) + data.time.seconds() * 0.0;
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
