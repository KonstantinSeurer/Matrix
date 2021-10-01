/*
 * GLTF.h
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#ifndef SCENE_GLTF_H_
#define SCENE_GLTF_H_

#include "../RL.h"
#include "Scene.h"
#include "../graphics/Device.h"
#include "Material.h"
#include "Model.h"

namespace matrix {

namespace scene {

Ref<Material> createDefaultGLTFMaterial(Ref<graphics::DeviceInstance> device);

void loadGLB(Ref<graphics::DeviceInstance> device, const RL& glb, Scene& target);

void loadGLTF(Ref<graphics::DeviceInstance> device, const RL& gltf, Scene& target);

const MaterialType* getGLTFMaterialType();

const ModelType* getGLTFModelType();

void registerGLTFShaderInclude();

Ref<Model> createGLTFCube(Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes, f32 r, Ref<const Material> material);
Ref<Model> createGLTFSphere(Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes, f32 r, u32 resolution, Ref<const Material> material);
Ref<Model> createGLTFCylinder(Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes, f32 r, f32 ry, u32 resolution, Ref<const Material> material);

}

}

#endif /* SCENE_GLTF_H_ */
