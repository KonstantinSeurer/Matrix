/*
 * GLTF.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: konstantin
 */

#include "GLTF.h"
#include "../Logging.h"
#include "Model.h"
#include "../graphics/ImageLoader.h"
#include "../graphics/Pipeline.h"
#include "renderer/SceneMaterialBuffer.h"
#include "renderer/ModelModelBuffer.h"

namespace matrix
{

namespace scene
{

using namespace math;
using namespace graphics;

void loadGLB(Ref<graphics::DeviceInstance> device, const RL &glb, Scene &target)
{

}

struct Buffer
{
	Array<u8> data;
};

struct BufferView
{
	Buffer *buffer;
	u32 offset;
	u32 size;
};

enum class GLTFType
{
	SCALAR, VEC2, VEC3, VEC4, MAT2, MAT3, MAT4
};

enum class GLTFComponentType : u32
{
	BYTE = 5120, UNSIGNED_BYTE = 5121, SHORT = 5122, UNSIGNED_SHORT = 5123, UNSIGNED_INT = 5125, FLOAT = 5126
};

struct Accessor
{
	BufferView *view;
	u32 offset;
	u32 length;
	GLTFType type;
	GLTFComponentType componentType;
};

static UnorderedMap<String, GLTFType> types = { { "SCALAR", GLTFType::SCALAR }, { "VEC2", GLTFType::VEC2 }, { "VEC3", GLTFType::VEC3 },
	{ "VEC4", GLTFType::VEC4 }, { "MAT2", GLTFType::MAT2 }, { "MAT3", GLTFType::MAT3 }, { "MAT4", GLTFType::MAT4 } };

struct GLTFMesh
{
	Array<Ref<Model>> primitives;
};

struct GLTFNode
{
	GLTFMesh *mesh = null;
	Array<u32> children;
	Vec3 position = { 0.0f, 0.0f, 0.0f };
	Vec3 scale = { 1.0f, 1.0f, 1.0f };
	Quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
};

static UnorderedMap<u32, WrapMode> wrapModes = { { 33071, WrapMode::CLAMP }, { 33648, WrapMode::MIRRORED_REPEAT }, { 10497, WrapMode::REPEAT } };

Ref<Material> createDefaultGLTFMaterial(Ref<graphics::DeviceInstance> device)
{
	static Ref<ImageFormat> colorFormat = device->createImageFormat(ImageFormatType::COLOR, 4, 1, false);
	static Ref<ImageSampler2D> defaultAlbedo = device->createColorTexture(1.0f, 1.0f, 1.0f, 1.0f, colorFormat);
	static Ref<ImageSampler2D> defaultMetallicRoughness = device->createColorTexture(1.0f, 1.0f, 0.0f, 0.0f, colorFormat);
	static Ref<ImageSampler2D> defaultNormal = device->createColorTexture(0.5f, 0.5f, 1.0f, 0.0f, colorFormat);
	static Ref<ImageSampler2D> defaultOcclusion = device->createColorTexture(1.0f, 0.0f, 0.0f, 0.0f, colorFormat);
	static Ref<ImageSampler2D> defaultEmissive = device->createColorTexture(0.0f, 0.0f, 0.0f, 0.0f, colorFormat);

	Ref<Material> material = allocate<Material>(getGLTFMaterialType());

	material->setImageSampler2D("baseColorTexture", defaultAlbedo);
	material->setImageSampler2D("metallicRoughnessTexture", defaultMetallicRoughness);
	material->setImageSampler2D("normalTexture", defaultNormal);
	material->setImageSampler2D("occlusionTexture", defaultOcclusion);
	material->setImageSampler2D("emissiveTexture", defaultEmissive);
	material->setVec4("baseColorFactor", { 1.0f, 1.0f, 1.0f, 1.0f });
	material->setFloat("metallicFactor", 1.0f);
	material->setFloat("roughnessFactor", 1.0f);
	material->setVec3("emissiveFactor", { });

	return material;
}

void loadGLTF(Ref<graphics::DeviceInstance> device, const RL &gltf, Scene &target)
{
	JSON json = JSON::parse(gltf.readText());

	Array<Buffer> buffers;
	if (json.find("buffers") != json.end())
	{
		JSON buffersJson = json["buffers"];

		for (u32 i = 0; i < buffersJson.size(); i++)
		{
			JSON bufferJson = buffersJson[i];

			u32 size = bufferJson["byteLength"];

			buffers.emplace_back();
			Buffer &buffer = buffers[buffers.size() - 1];

			if (bufferJson.find("uri") != bufferJson.end())
			{
				RL bufferRl = gltf.uri(bufferJson["uri"]);
				bufferRl.read(buffer.data);
				MATRIX_ASSERT(buffer.data.size() == size)
			}
		}
	}

	Array<BufferView> bufferViews;
	if (json.find("bufferViews") != json.end())
	{
		JSON bufferViewsJson = json["bufferViews"];

		for (u32 i = 0; i < bufferViewsJson.size(); i++)
		{
			JSON bufferViewJson = bufferViewsJson[i];

			BufferView view;
			view.buffer = &buffers[bufferViewJson["buffer"]];
			view.offset = bufferViewJson["byteOffset"];
			view.size = bufferViewJson["byteLength"];

			bufferViews.push_back(view);
		}
	}

	Array<Accessor> accessors;
	if (json.find("accessors") != json.end())
	{
		JSON accessorsJson = json["accessors"];

		for (u32 i = 0; i < accessorsJson.size(); i++)
		{
			JSON accessorJson = accessorsJson[i];

			Accessor accessor;
			accessor.view = &bufferViews[accessorJson["bufferView"]];
			accessor.offset = accessorJson["byteOffset"];
			accessor.length = accessorJson["count"];
			accessor.type = types[accessorJson["type"]];
			accessor.componentType = (GLTFComponentType) accessorJson["componentType"];

			accessors.push_back(accessor);
		}
	}

	Array<Ref<ImageView2D>> images;
	if (json.find("images") != json.end())
	{
		JSON imagesJson = json["images"];

		for (u32 i = 0; i < imagesJson.size(); i++)
		{
			JSON imageJson = imagesJson[i];

			Ref<Image2DData> imageData = null;

			if (imageJson.find("uri") != imageJson.end())
			{
				imageData = loadImage(device, gltf.uri(imageJson["uri"]));
			}
			else if (imageJson.find("bufferView") != imageJson.end())
			{
				BufferView view = bufferViews[imageJson["bufferView"]];
				imageData = loadImage(device, view.buffer->data.data() + view.offset, view.size);
			}

			Ref<Image2D> image = device->createImage2D(*imageData, ImageUsage::SAMPLED);

			images.push_back(device->createImageView2D(image, 0, 1));
		}
	}

	Array<Ref<Sampler2D>> samplers;
	if (json.find("samplers") != json.end())
	{
		JSON samplersJson = json["samplers"];

		for (u32 i = 0; i < samplersJson.size(); i++)
		{
			JSON samplerJson = samplersJson[i];

			WrapMode wrapS = wrapModes[samplerJson["wrapS"]];
			WrapMode wrapT = wrapModes[samplerJson["wrapT"]];

			samplers.push_back(device->createSampler2D(SamplingMode::LINEAR, SamplingMode::LINEAR, wrapS, wrapT)); // TODO: implement
		}
	}

	Array<Ref<ImageSampler2D>> textures;
	if (json.find("textures") != json.end())
	{
		JSON texturesJson = json["textures"];

		for (u32 i = 0; i < texturesJson.size(); i++)
		{
			JSON textureJson = texturesJson[i];

			textures.push_back(allocate<ImageSampler2D>(images[textureJson["source"]], samplers[textureJson["sampler"]]));
		}
	}

	Array<Ref<Material>> materials;
	if (json.find("materials") != json.end())
	{
		JSON materialsJson = json["materials"];

		for (u32 i = 0; i < materialsJson.size(); i++)
		{
			JSON materialJson = materialsJson[i];
			JSON pbrMetallicRoughnessJSON = materialJson["pbrMetallicRoughness"];

			Ref<Material> material = createDefaultGLTFMaterial(device);

			if (pbrMetallicRoughnessJSON.find("baseColorTexture") != pbrMetallicRoughnessJSON.end())
			{
				u32 textureIndex = pbrMetallicRoughnessJSON["baseColorTexture"]["index"];
				material->setImageSampler2D("baseColorTexture", textures[textureIndex]);
			}

			if (pbrMetallicRoughnessJSON.find("metallicRoughnessTexture") != pbrMetallicRoughnessJSON.end())
			{
				u32 textureIndex = pbrMetallicRoughnessJSON["metallicRoughnessTexture"]["index"];
				material->setImageSampler2D("metallicRoughnessTexture", textures[textureIndex]);
			}

			if (materialJson.find("normalTexture") != materialJson.end())
			{
				u32 textureIndex = materialJson["normalTexture"]["index"];
				material->setImageSampler2D("normalTexture", textures[textureIndex]);
			}

			if (materialJson.find("occlusionTexture") != materialJson.end())
			{
				u32 textureIndex = materialJson["occlusionTexture"]["index"];
				material->setImageSampler2D("occlusionTexture", textures[textureIndex]);
			}

			if (materialJson.find("emissiveTexture") != materialJson.end())
			{
				u32 textureIndex = materialJson["emissiveTexture"]["index"];
				material->setImageSampler2D("emissiveTexture", textures[textureIndex]);
			}

			if (pbrMetallicRoughnessJSON.find("baseColorFactor") != pbrMetallicRoughnessJSON.end())
			{
				JSON components = pbrMetallicRoughnessJSON["baseColorFactor"];
				material->setVec4("baseColorFactor", { components[0], components[1], components[2], components[3] });
			}

			if (pbrMetallicRoughnessJSON.find("metallicFactor") != pbrMetallicRoughnessJSON.end())
			{
				material->setFloat("metallicFactor", pbrMetallicRoughnessJSON["metallicFactor"]);
			}

			if (pbrMetallicRoughnessJSON.find("roughnessFactor") != pbrMetallicRoughnessJSON.end())
			{
				material->setFloat("roughnessFactor", pbrMetallicRoughnessJSON["roughnessFactor"]);
			}

			if (materialJson.find("emissiveFactor") != materialJson.end())
			{
				JSON components = materialJson["emissiveFactor"];
				material->setVec3("emissiveFactor", { components[0], components[1], components[2] });
			}

			materials.push_back(material);
		}
	}

	Array<GLTFMesh> meshes;
	if (json.find("meshes") != json.end())
	{
		JSON meshesJson = json["meshes"];

		for (u32 i = 0; i < meshesJson.size(); i++)
		{
			JSON meshJson = meshesJson[i];

			meshes.emplace_back();
			GLTFMesh &mesh = meshes[meshes.size() - 1];

			JSON primitivesJson = meshJson["primitives"];
			for (u32 primitiveIndex = 0; primitiveIndex < primitivesJson.size(); primitiveIndex++)
			{
				JSON primitiveJson = primitivesJson[primitiveIndex];
				JSON attributesJson = primitiveJson["attributes"];

				Accessor positionsAccessor = accessors[attributesJson["POSITION"]];

				Array<Vec3> positions(positionsAccessor.length);
				Array<Vec2> textureCoords(positionsAccessor.length);
				Array<Vec3> normals(positionsAccessor.length);
				Array<Vec4> tangents(positionsAccessor.length);

				MATRIX_ASSERT(positionsAccessor.type == GLTFType::VEC3)
				MATRIX_ASSERT(positionsAccessor.componentType == GLTFComponentType::FLOAT)
				f32 *positionsAddress = (f32*) (positionsAccessor.view->buffer->data.data() + positionsAccessor.view->offset + positionsAccessor.offset);
				for (u32 vertexIndex = 0; vertexIndex < positionsAccessor.length; vertexIndex++)
				{
					positions[vertexIndex] =
						{ positionsAddress[vertexIndex * 3], positionsAddress[vertexIndex * 3 + 1], -positionsAddress[vertexIndex * 3 + 2] };
				}

				if (attributesJson.find("NORMAL") != attributesJson.end())
				{
					Accessor normalsAccessor = accessors[attributesJson["NORMAL"]];
					MATRIX_ASSERT(normalsAccessor.componentType == GLTFComponentType::FLOAT)

					f32 *normalsAddress = (f32*) (normalsAccessor.view->buffer->data.data() + normalsAccessor.view->offset + normalsAccessor.offset);
					for (u32 vertexIndex = 0; vertexIndex < normalsAccessor.length; vertexIndex++)
					{
						normals[vertexIndex] = { normalsAddress[vertexIndex * 3], normalsAddress[vertexIndex * 3 + 1], -normalsAddress[vertexIndex * 3 + 2] };
					}
				}

				if (attributesJson.find("TANGENT") != attributesJson.end())
				{
					Accessor tangentsAccessor = accessors[attributesJson["TANGENT"]];
					MATRIX_ASSERT(tangentsAccessor.componentType == GLTFComponentType::FLOAT)

					f32 *tangentsAddress = (f32*) (tangentsAccessor.view->buffer->data.data() + tangentsAccessor.view->offset + tangentsAccessor.offset);
					for (u32 vertexIndex = 0; vertexIndex < tangentsAccessor.length; vertexIndex++)
					{
						tangents[vertexIndex] = { tangentsAddress[vertexIndex * 4], tangentsAddress[vertexIndex * 4 + 1], -tangentsAddress[vertexIndex * 4 + 2],
							tangentsAddress[vertexIndex * 4 + 3] };
					}
				}

				if (attributesJson.find("TEXCOORD_0") != attributesJson.end())
				{
					Accessor texCoordsAccessor = accessors[attributesJson["TEXCOORD_0"]];
					MATRIX_ASSERT(texCoordsAccessor.componentType == GLTFComponentType::FLOAT)

					f32 *texCoordsAddress = (f32*) (texCoordsAccessor.view->buffer->data.data() + texCoordsAccessor.view->offset + texCoordsAccessor.offset);
					for (u32 vertexIndex = 0; vertexIndex < texCoordsAccessor.length; vertexIndex++)
					{
						textureCoords[vertexIndex] = { texCoordsAddress[vertexIndex * 2], texCoordsAddress[vertexIndex * 2 + 1] };
					}
				}

				Array<u32> indices;
				if (primitiveJson.find("indices") != primitiveJson.end())
				{
					Accessor indicesAccessor = accessors[primitiveJson["indices"]];
					MATRIX_ASSERT(indicesAccessor.type == GLTFType::SCALAR)

					indices.resize(indicesAccessor.length);

					if (indicesAccessor.componentType == GLTFComponentType::UNSIGNED_INT)
					{
						u32 *indicesAddress = (u32*) (indicesAccessor.view->buffer->data.data() + indicesAccessor.view->offset + indicesAccessor.offset);
						for (u32 indexIndex = 0; indexIndex < indicesAccessor.length; indexIndex++)
						{
							indices[indexIndex] = indicesAddress[indexIndex];
						}
					}
					else if (indicesAccessor.componentType == GLTFComponentType::UNSIGNED_SHORT)
					{
						u16 *indicesAddress = (u16*) (indicesAccessor.view->buffer->data.data() + indicesAccessor.view->offset + indicesAccessor.offset);
						for (u32 indexIndex = 0; indexIndex < indicesAccessor.length; indexIndex++)
						{
							indices[indexIndex] = indicesAddress[indexIndex];
						}
					}
					else
					{
						err("Unsupported gltf componentType " + toString((u32) indicesAccessor.componentType) + " for 'indices'!");
					}
				}
				else
				{
					indices.resize(positionsAccessor.length);
					for (u32 index = 0; index < positionsAccessor.length; index++)
					{
						indices[index] = index;
					}
				}

				Ref<Material> material = materials[primitiveJson["material"]];

				Array<void*> attributes;
				attributes.push_back(positions.data());
				attributes.push_back(textureCoords.data());
				attributes.push_back(normals.data());
				attributes.push_back(tangents.data());

				mesh.primitives.push_back(
					allocate<Model>(getGLTFModelType(), device, target.getRenderImplementationTypes(), attributes, positionsAccessor.length, indices,
						material));
			}
		}
	}

	Array<GLTFNode> nodes;
	if (json.find("nodes") != json.end())
	{
		JSON nodesJson = json["nodes"];

		for (u32 i = 0; i < nodesJson.size(); i++)
		{
			JSON nodeJson = nodesJson[i];

			nodes.emplace_back();
			GLTFNode &node = nodes[nodes.size() - 1];

			if (nodeJson.find("mesh") != nodeJson.end())
			{
				node.mesh = &meshes[nodeJson["mesh"]];
			}

			if (nodeJson.find("translation") != nodeJson.end())
			{
				JSON translationJson = nodeJson["translation"];
				node.position = { translationJson[0], translationJson[1], translationJson[2] };
			}

			if (nodeJson.find("scale") != nodeJson.end())
			{
				JSON scaleJson = nodeJson["scale"];
				node.scale = { scaleJson[0], scaleJson[1], scaleJson[2] };
			}

			if (nodeJson.find("rotation") != nodeJson.end())
			{
				JSON rotationJson = nodeJson["rotation"];
				node.rotation = { rotationJson[0], rotationJson[1], rotationJson[2], rotationJson[3] };
			}

			if (nodeJson.find("children") != nodeJson.end())
			{
				JSON childrenJson = nodeJson["children"];
				for (u32 child : childrenJson)
				{
					node.children.push_back(child);
				}
			}
		}
	}

	if (json.find("scene") != json.end())
	{
		JSON sceneJson = json["scenes"][(u32) json["scene"]];

		Array<Ref<Entity>> entities;
		for (u32 i = 0; i < nodes.size(); i++)
		{
			const GLTFNode &node = nodes[i];

			Ref<Entity> entity = target.createEntity();

			TransformComponent *transform = entity->addComponent<TransformComponent>();
			transform->position = node.position;
			transform->scale = node.scale;
			transform->rotation = node.rotation;

			for (Ref<Model> model : node.mesh->primitives)
			{
				entity->addComponent<ModelComponent>(model);
			}

			entities.push_back(entity);
		}

		for (u32 i = 0; i < nodes.size(); i++)
		{
			const GLTFNode &node = nodes[i];
			Ref<Entity> entity = entities[i];

			for (u32 childIndex : node.children)
			{
				entity->addEntity(entities[childIndex]);
			}
		}

		JSON nodesJson = sceneJson["nodes"];
		for (u32 i = 0; i < nodesJson.size(); i++)
		{
			target.add(entities[nodesJson[i]]);
		}
	}
}

static MaterialType *gltfMaterialType = null;

const MaterialType* getGLTFMaterialType()
{
	if (!gltfMaterialType)
	{
		gltfMaterialType = new MaterialType();
		gltfMaterialType->addAttribute( { PrimitiveTypeType::VEC4, "baseColorFactor" });
		gltfMaterialType->addAttribute( { PrimitiveTypeType::FLOAT, "metallicFactor" });
		gltfMaterialType->addAttribute( { PrimitiveTypeType::FLOAT, "roughnessFactor" });
		gltfMaterialType->addAttribute( { PrimitiveTypeType::VEC3, "emissiveFactor" });
		gltfMaterialType->addDescriptor( { DescriptorType::IMAGE_SAMPLER_2D, "baseColorTexture" });
		gltfMaterialType->addDescriptor( { DescriptorType::IMAGE_SAMPLER_2D, "metallicRoughnessTexture" });
		gltfMaterialType->addDescriptor( { DescriptorType::IMAGE_SAMPLER_2D, "normalTexture" });
		gltfMaterialType->addDescriptor( { DescriptorType::IMAGE_SAMPLER_2D, "occlusionTexture" });
		gltfMaterialType->addDescriptor( { DescriptorType::IMAGE_SAMPLER_2D, "emissiveTexture" });
	}
	return gltfMaterialType;
}

static ModelType *gltfModelType = null;

const ModelType* getGLTFModelType()
{
	if (!gltfModelType)
	{
		gltfModelType = new ModelType();
		gltfModelType->setPositionAttribute( { PrimitiveTypeType::VEC3, "position" });
		gltfModelType->addAttribute( { PrimitiveTypeType::VEC2, "textureCoord" });
		gltfModelType->addAttribute( { PrimitiveTypeType::VEC3, "normal" });
		gltfModelType->addAttribute( { PrimitiveTypeType::VEC4, "tangent" });
	}
	return gltfModelType;
}

Ref<const PipelineSource> materialBufferGLTF = null;
Ref<const PipelineSource> modelBufferGLTF = null;

void registerGLTFShaderInclude()
{
	if (!materialBufferGLTF)
	{
		SceneMaterialBuffer::getSource();

		materialBufferGLTF = parsePipeline(RL("file:///shaders/scene/materialBufferGLTF.shader"));
		addShaderInclude("scene.MaterialBuffer.GLTF", materialBufferGLTF);
	}

	if (!modelBufferGLTF)
	{
		ModelModelBuffer::getSource();

		modelBufferGLTF = parsePipeline(RL("file:///shaders/scene/modelBufferGLTF.shader"));
		addShaderInclude("scene.ModelBuffer.GLTF", modelBufferGLTF);
	}
}

Ref<Model> createGLTFCube(Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes, f32 r,
	Ref<const Material> material)
{
	Array<Vec3> positions;
	Array<Vec2> textureCoords;
	Array<Vec3> normals;
	Array<Vec4> tangents;
	Array<u32> indices;

	auto addVertex = [&](Vec3 position, Vec3 normal, Vec3 tangent, Vec2 textureCoord)
	{
		positions.push_back(position);
		normals.push_back(normal);
		tangents.push_back(
			{	tangent, 1.0f});
		textureCoords.push_back(textureCoord);
	};

	// -Z
	addVertex( { -r, -r, -r }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f });
	addVertex( { r, -r, -r }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f });
	addVertex( { r, r, -r }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f });
	addVertex( { -r, r, -r }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f });

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	// +Z
	addVertex( { r, -r, r }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f });
	addVertex( { -r, -r, r }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f });
	addVertex( { -r, r, r }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f });
	addVertex( { r, r, r }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f });

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(4);

	// +X
	addVertex( { r, -r, -r }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f });
	addVertex( { r, -r, r }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f });
	addVertex( { r, r, r }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f });
	addVertex( { r, r, -r }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f });

	indices.push_back(8);
	indices.push_back(9);
	indices.push_back(10);
	indices.push_back(10);
	indices.push_back(11);
	indices.push_back(8);

	// -X
	addVertex( { -r, -r, r }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f });
	addVertex( { -r, -r, -r }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f });
	addVertex( { -r, r, -r }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f });
	addVertex( { -r, r, r }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f });

	indices.push_back(12);
	indices.push_back(13);
	indices.push_back(14);
	indices.push_back(14);
	indices.push_back(15);
	indices.push_back(12);

	// +Y
	addVertex( { -r, r, -r }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f });
	addVertex( { r, r, -r }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f });
	addVertex( { r, r, r }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f });
	addVertex( { -r, r, r }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f });

	indices.push_back(16);
	indices.push_back(17);
	indices.push_back(18);
	indices.push_back(18);
	indices.push_back(19);
	indices.push_back(16);

	// -Y
	addVertex( { -r, -r, r }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f });
	addVertex( { r, -r, r }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f });
	addVertex( { r, -r, -r }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f });
	addVertex( { -r, -r, -r }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f });

	indices.push_back(20);
	indices.push_back(21);
	indices.push_back(22);
	indices.push_back(22);
	indices.push_back(23);
	indices.push_back(20);

	Array<void*> attributes;
	attributes.push_back(positions.data());
	attributes.push_back(textureCoords.data());
	attributes.push_back(normals.data());
	attributes.push_back(tangents.data());

	return allocate<Model>(getGLTFModelType(), device, renderImplementationTypes, attributes, positions.size(), indices, material);
}

Ref<Model> createGLTFSphere(Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes, f32 r, u32 resolution,
	Ref<const Material> material)
{
	Array<Vec3> positions;
	Array<Vec2> textureCoords;
	Array<Vec3> normals;
	Array<Vec4> tangents;
	Array<u32> indices;

	auto addVertex = [&](Vec3 position, Vec3 normal, Vec3 tangent, Vec2 textureCoord)
	{
		positions.push_back(position);
		normals.push_back(normal);
		tangents.push_back(
			{	tangent, 1.0f});
		textureCoords.push_back(textureCoord);
	};

	const u32 resolutionX = resolution * 2;

	for (u32 y = 0; y <= resolution; y++)
	{
		const f32 textureCoordY = (f32) y / (f32) resolution;
		const f32 yAngle = (textureCoordY - 0.5f) * PI;
		const f32 cosY = cos(yAngle);
		const f32 sinY = sin(yAngle);
		for (u32 x = 0; x <= resolutionX; x++)
		{
			const f32 textureCoordX = (f32) x / (f32) resolutionX;
			const f32 xAngle = textureCoordX * 2.0f * PI;

			const Vec3 normal(cos(xAngle) * cosY, sinY, sin(xAngle) * cosY);
			const Vec3 tangent(cos(xAngle + PI * 0.5f), 0.0f, sin(xAngle + PI * 0.5f));
			const Vec3 position = normal * r;
			const Vec2 textureCoord(textureCoordX, textureCoordY);

			addVertex(position, normal, tangent, textureCoord);

			if (y < resolution && x < resolutionX)
			{
				indices.push_back(x + y * (resolutionX + 1));
				indices.push_back((x + 1) + y * (resolutionX + 1));
				indices.push_back((x + 1) + (y + 1) * (resolutionX + 1));
				indices.push_back((x + 1) + (y + 1) * (resolutionX + 1));
				indices.push_back(x + (y + 1) * (resolutionX + 1));
				indices.push_back(x + y * (resolutionX + 1));
			}
		}
	}

	Array<void*> attributes;
	attributes.push_back(positions.data());
	attributes.push_back(textureCoords.data());
	attributes.push_back(normals.data());
	attributes.push_back(tangents.data());

	return allocate<Model>(getGLTFModelType(), device, renderImplementationTypes, attributes, positions.size(), indices, material);
}

Ref<Model> createGLTFCylinder(Ref<graphics::DeviceInstance> device, const Array<RenderImplementationType> &renderImplementationTypes, f32 r, f32 ry,
	u32 resolution, Ref<const Material> material)
{
	Array<Vec3> positions;
	Array<Vec2> textureCoords;
	Array<Vec3> normals;
	Array<Vec4> tangents;
	Array<u32> indices;

	auto addVertex = [&](Vec3 position, Vec3 normal, Vec3 tangent, Vec2 textureCoord)
	{
		positions.push_back(position);
		normals.push_back(normal);
		tangents.push_back(
			{	tangent, 1.0f});
		textureCoords.push_back(textureCoord);
	};

	for (u32 x = 0; x <= resolution; x++)
	{
		const f32 textureCoordX = (f32) x / (f32) resolution;
		const f32 xAngle = textureCoordX * 2.0f * PI;

		const Vec3 normal(cos(xAngle), 0.0f, sin(xAngle));
		const Vec3 tangent(cos(xAngle + PI * 0.5f), 0.0f, sin(xAngle + PI * 0.5f));
		const Vec3 position = normal * r;
		const Vec2 textureCoord(textureCoordX, 0.0f);

		addVertex(position - Vec3(0.0f, ry, 0.0f), normal, tangent, textureCoord);
		addVertex(position + Vec3(0.0f, ry, 0.0f), normal, tangent, textureCoord + Vec2(0.0f, 1.0f));

		if (x < resolution)
		{
			indices.push_back(x * 2 + 1);
			indices.push_back(x * 2);
			indices.push_back((x + 1) * 2);
			indices.push_back((x + 1) * 2);
			indices.push_back((x + 1) * 2 + 1);
			indices.push_back(x * 2 + 1);
		}
	}

	Array<void*> attributes;
	attributes.push_back(positions.data());
	attributes.push_back(textureCoords.data());
	attributes.push_back(normals.data());
	attributes.push_back(tangents.data());

	return allocate<Model>(getGLTFModelType(), device, renderImplementationTypes, attributes, positions.size(), indices, material);
}

}

}
