/*
 * VulkanUtil.cpp
 *
 *  Created on: Jun 3, 2021
 *      Author: konstantin
 */

#include "VulkanUtil.h"
#include <vulkan/vk_enum_string_helper.h>
#include "shaderc/shaderc.h"

namespace matrix
{

namespace graphics
{

void assertVkResult(VkResult result, const SourceLocation &location)
{
	if (result == VK_SUCCESS)
	{
		return;
	}
	err(String("Vulkan error ") + string_VkResult(result) + " at " + location.toString() + "!");
}

VkFormat getFloatFormat(ImageFormat format)
{
	if (format.componentSize == 2)
	{
		switch (format.componentCount)
		{
			case 1:
				return VK_FORMAT_R16_SFLOAT;
			case 2:
				return VK_FORMAT_R16G16_SFLOAT;
			case 3:
				return VK_FORMAT_R16G16B16_SFLOAT;
			case 4:
				return VK_FORMAT_R16G16B16A16_SFLOAT;
		}
	}
	if (format.componentSize == 4)
	{
		switch (format.componentCount)
		{
			case 1:
				return VK_FORMAT_R32_SFLOAT;
			case 2:
				return VK_FORMAT_R32G32_SFLOAT;
			case 3:
				return VK_FORMAT_R32G32B32_SFLOAT;
			case 4:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
		}
	}

	err("Unsupported component size for float formats " + toString(format.componentSize) + "!");
	return VK_FORMAT_UNDEFINED;
}

VkFormat getUnormFormat(ImageFormat format)
{
	if (format.componentSize == 1)
	{
		switch (format.componentCount)
		{
			case 1:
				return VK_FORMAT_R8_UNORM;
			case 2:
				return VK_FORMAT_R8G8_UNORM;
			case 3:
				return VK_FORMAT_R8G8B8_UNORM;
			case 4:
				return VK_FORMAT_R8G8B8A8_UNORM;
		}
	}
	if (format.componentSize == 2)
	{
		switch (format.componentCount)
		{
			case 1:
				return VK_FORMAT_R16_UNORM;
			case 2:
				return VK_FORMAT_R16G16_UNORM;
			case 3:
				return VK_FORMAT_R16G16B16_UNORM;
			case 4:
				return VK_FORMAT_R16G16B16A16_UNORM;
		}
	}

	err("Unsupported component size for unorm fornats " + toString(format.componentSize) + "!");
	return VK_FORMAT_UNDEFINED;
}

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const Array<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	err("Failed to find supported format!");
	return VK_FORMAT_UNDEFINED;
}

VkFormat getVkFormat(VkPhysicalDevice physicalDevice, ImageFormat format)
{
	if (format.componentCount > 4 || format.componentCount == 0)
	{
		err("Unsupported component count " + toString(format.componentCount) + "!");
		return VK_FORMAT_UNDEFINED;
	}
	if (format.componentSize > 4)
	{
		err("Unsupported component size " + toString(format.componentSize) + "!");
		return VK_FORMAT_UNDEFINED;
	}

	switch (format.type)
	{
		case ImageFormatType::COLOR:
			if (format.floatingPoint)
			{
				return getFloatFormat(format);
			}
			else
			{
				return getUnormFormat(format);
			}
		case ImageFormatType::DEPTH:
			return findSupportedFormat(physicalDevice, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM }, VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		case ImageFormatType::STENCIL:
			return findSupportedFormat(physicalDevice, { VK_FORMAT_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		case ImageFormatType::DEPTH_STENCIL:
			return findSupportedFormat(physicalDevice, { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	return VK_FORMAT_UNDEFINED;
}

VkFormat getVkFormat(PrimitiveTypeType type)
{
	switch (type)
	{
		case PrimitiveTypeType::INT:
			return VK_FORMAT_R32_SINT;
		case PrimitiveTypeType::UINT:
			return VK_FORMAT_R32_UINT;
		case PrimitiveTypeType::FLOAT:
			return VK_FORMAT_R32_SFLOAT;
		case PrimitiveTypeType::VEC2:
			return VK_FORMAT_R32G32_SFLOAT;
		case PrimitiveTypeType::VEC3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case PrimitiveTypeType::VEC4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		default:
			err("Unsupported PrimitiveTypeType " + toString((u32) type) + "!");
			return VK_FORMAT_UNDEFINED;
	}
}

static UnorderedMap<DescriptorType, VkDescriptorType> descriptorTypes = { { DescriptorType::UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, {
	DescriptorType::STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER }, { DescriptorType::IMAGE_SAMPLER_2D, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, {
	DescriptorType::IMAGE_2D, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE }, { DescriptorType::IMAGE_SAMPLER_3D, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, {
	DescriptorType::IMAGE_3D, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE }, { DescriptorType::TOP_LEVEL_ACCELERATION_STRUCTURE,
	VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR } };

VkDescriptorType getVkDescriptorType(DescriptorType type)
{
	return descriptorTypes[type];
}

static shaderc_compiler_t compiler = shaderc_compiler_initialize();
static UnorderedMap<ShaderType, shaderc_compile_options_t> compileOptions;

static shaderc_compile_options_t createCompileOptions()
{
	shaderc_compile_options_t options = shaderc_compile_options_initialize();
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_performance);
	shaderc_compile_options_set_target_spirv(options, shaderc_spirv_version_1_4);
	shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
	return options;
}

static_block
{
	shaderc_compile_options_t vertex = createCompileOptions();
	shaderc_compile_options_add_macro_definition(vertex, "vertexPosition", 14, "gl_Position", 11);
	shaderc_compile_options_add_macro_definition(vertex, "vertexIndex", 11, "gl_VertexIndex", 14);
	compileOptions[ShaderType::VERTEX] = vertex;

	shaderc_compile_options_t tesselationControl = createCompileOptions();
	compileOptions[ShaderType::TESSELATION_CONTROL] = tesselationControl;

	shaderc_compile_options_t tesselationEvaluation = createCompileOptions();
	compileOptions[ShaderType::TESSELATION_EVALUATION] = tesselationEvaluation;

	shaderc_compile_options_t geometry = createCompileOptions();
	compileOptions[ShaderType::GEOMETRY] = geometry;

	shaderc_compile_options_t fragment = createCompileOptions();
	compileOptions[ShaderType::FRAGMENT] = fragment;

	shaderc_compile_options_t compute = createCompileOptions();
	shaderc_compile_options_add_macro_definition(compute, "globalCoords", 12, "gl_GlobalInvocationID", 21);
	compileOptions[ShaderType::COMPUTE] = compute;
}

static shaderc_shader_kind getShaderKind(ShaderType type)
{
	switch (type)
	{
		case ShaderType::VERTEX:
			return shaderc_vertex_shader;
		case ShaderType::TESSELATION_CONTROL:
			return shaderc_tess_control_shader;
		case ShaderType::TESSELATION_EVALUATION:
			return shaderc_tess_evaluation_shader;
		case ShaderType::GEOMETRY:
			return shaderc_geometry_shader;
		case ShaderType::FRAGMENT:
			return shaderc_fragment_shader;
		case ShaderType::COMPUTE:
			return shaderc_compute_shader;
	}

	err("Unsupported shader type " + toString((u32) type) + "!");
	return shaderc_fragment_shader;
}

static void replaceAll(String &str, const String &from, const String &to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

static UnorderedMap<BufferLayout, String> bufferLayoutNames = { { BufferLayout::STANDARD, "std140" }, { BufferLayout::PACKED, "std430" } };

static UnorderedMap<DescriptorType, String> bufferTypeNames = { { DescriptorType::UNIFORM_BUFFER, "uniform" }, { DescriptorType::STORAGE_BUFFER, "buffer" } };

static String adjustFunctionBody(const String &body, const UnorderedMap<String, String> replacements)
{
	String result = body;
	replaceAll(result, "inputs.", "inputs_");
	replaceAll(result, "outputs.", "outputs_");
	for (const auto &replacement : replacements)
	{
		replaceAll(result, replacement.first, replacement.second);
	}
	return result;
}

static Array<String> imageFormatComponentNames = { "", "r", "rg", "rgb", "rgba" };

VkShaderModule compileShader(VkDevice device, const PipelineSource &source, const ShaderSource &shader, ShaderType stage)
{
	String sourceText = "#version 460\n#extension GL_EXT_ray_tracing : enable\n#extension GL_EXT_ray_query : enable\n";

	if (stage == ShaderType::COMPUTE)
	{
		const ComputePipelineSource &computeSource = (const ComputePipelineSource&) source;
		sourceText += "layout(local_size_x=" + toString(computeSource.localSize.x) + ", local_size_y=" + toString(computeSource.localSize.y) + ", local_size_z="
			+ toString(computeSource.localSize.z) + ") in;\n";
	}

	for (auto structType : source.structs)
	{
		sourceText += "struct " + structType->name + "{\n";
		for (const auto &member : structType->members)
		{
			sourceText += "\t" + member.type->getName() + " " + member.name + ";\n";
		}
		sourceText += "};\n";
	}

	for (u32 i = 0; i < shader.inputs.size(); i++)
	{
		const auto &input = shader.inputs[i];
		sourceText += "layout(location=" + toString(i) + ") in " + input.type->getName() + " inputs_" + input.name + ";\n";
	}

	for (u32 i = 0; i < shader.outputs.size(); i++)
	{
		const auto &output = shader.outputs[i];
		sourceText += "layout(location=" + toString(i) + ") out " + output.type->getName() + " outputs_" + output.name + ";\n";
	}

	if (!source.uniforms.empty())
	{
		sourceText += "layout (push_constant, std140) uniform UNIFORMS {\n";
		for (const auto &member : source.uniforms)
		{
			sourceText += "\t" + member.type->getName() + " " + member.name + ";\n";
		}
		sourceText += "} uniforms;\n";
	}

	for (const auto &constant : source.constants)
	{
		sourceText += "const " + constant.type->getName() + " " + constant.name + " = " + constant.value + ";\n";
	}

	UnorderedMap<String, String> replacements;

	u32 bufferIndex = 0;
	for (u32 setIndex = 0; setIndex < source.descriptorSets.size(); setIndex++)
	{
		auto set = source.descriptorSets[setIndex];
		for (u32 binding = 0; binding < set->descriptors.size(); binding++)
		{
			sourceText += "layout(set=" + toString(setIndex) + ", binding=" + toString(binding);
			auto descriptor = set->descriptors[binding];
			String name = set->name + "_" + descriptor->name;
			replacements[set->name + "." + descriptor->name] = name;
			switch (descriptor->type)
			{
				case DescriptorType::UNIFORM_BUFFER:
				case DescriptorType::STORAGE_BUFFER: {
					auto buffer = CastDown<const BufferSource>(descriptor);
					sourceText += ", " + bufferLayoutNames[buffer->layout] + ") " + bufferTypeNames[buffer->type] + " " + buffer->data->name
						+ toString(bufferIndex++) + "{\n";
					for (const auto &member : buffer->data->members)
					{
						sourceText += "\t" + member.type->getName() + " " + member.name + ";\n";
					}
					sourceText += "}" + name;
					break;
				}
				case DescriptorType::IMAGE_SAMPLER_2D: {
					sourceText += ") uniform sampler2D " + name;
					break;
				}
				case DescriptorType::IMAGE_SAMPLER_3D: {
					sourceText += ") uniform sampler3D " + name;
					break;
				}
				case DescriptorType::IMAGE_2D:
				case DescriptorType::IMAGE_3D: {
					auto image = CastDown<const ImageSource>(descriptor);
					sourceText += ", " + imageFormatComponentNames[image->format.componentCount] + toString(image->format.componentSize * 8);
					if (image->format.floatingPoint)
					{
						sourceText += "f";
					}
					if (descriptor->type == DescriptorType::IMAGE_2D)
					{
						sourceText += ") uniform image2D " + name;
					}
					else
					{
						sourceText += ") uniform image3D " + name;
					}
					break;
				}
				case DescriptorType::TOP_LEVEL_ACCELERATION_STRUCTURE: {
					sourceText += ") uniform accelerationStructureEXT " + name;
					break;
				}
			}
			if (descriptor->arrayLength > 1)
			{
				sourceText += "[" + toString(descriptor->arrayLength) + "]";
			}
			sourceText += ";\n";
		}
	}

	for (const auto &function : source.functions)
	{
		sourceText += function->returnType->getName() + " " + function->name + "(";
		for (u32 i = 0; i < function->parameters.size(); i++)
		{
			const auto &parameter = function->parameters[i];

			String typeString = "";
			switch (parameter.parameterType)
			{
				case FunctionParameterType::INPUT:
					typeString = "in ";
					break;
				case FunctionParameterType::OUTPUT:
					typeString = "out ";
					break;
				case FunctionParameterType::INPUT_OUTPUT:
					typeString = "inout ";
					break;
				default:
					break;
			}

			sourceText += typeString + parameter.type->getName() + " " + parameter.name;

			if (i < function->parameters.size() - 1)
			{
				sourceText += ", ";
			}
		}
		sourceText += ") {\n" + adjustFunctionBody(function->body, replacements) + "}\n";
	}

	sourceText += "void main() {\n" + adjustFunctionBody(shader.main.body, replacements) + "\n}";

	shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, sourceText.c_str(), sourceText.length(), getShaderKind(stage), "", "main",
		compileOptions[stage]);

	if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
	{
		u32 line = 1;
		for (u32 i = 0; i < sourceText.length();)
		{
			u64 newLineIndex = sourceText.find('\n', i);
			if (newLineIndex == String::npos)
			{
				break;
			}
			err(toString(line++) + ":\t" + sourceText.substr(i, newLineIndex - i));
			i = newLineIndex + 1;
		}
		err(shaderc_result_get_error_message(result));
		return VK_NULL_HANDLE;
	}

	VkShaderModuleCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = null;
	createInfo.flags = 0;
	createInfo.codeSize = shaderc_result_get_length(result);
	createInfo.pCode = (const uint32_t*) shaderc_result_get_bytes(result);

	VkShaderModule module;
	assertVkResult(vkCreateShaderModule(device, &createInfo, null, &module), SOURCE_LOCATION());
	return module;
}

VkShaderStageFlagBits getShaderStage(ShaderType type)
{
	switch (type)
	{
		case ShaderType::VERTEX:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderType::TESSELATION_CONTROL:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ShaderType::TESSELATION_EVALUATION:
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case ShaderType::GEOMETRY:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderType::FRAGMENT:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderType::COMPUTE:
			return VK_SHADER_STAGE_COMPUTE_BIT;
	}

	err("Unsupported shader type " + toString((u32) type) + "!");
	return VK_SHADER_STAGE_ALL;
}

static UnorderedMap<PrimitiveTypeType, u32> typeSizes = { { PrimitiveTypeType::INT, 4 }, { PrimitiveTypeType::UINT, 4 }, { PrimitiveTypeType::BOOL, 4 }, {
	PrimitiveTypeType::FLOAT, 4 }, { PrimitiveTypeType::VEC2, 8 }, { PrimitiveTypeType::VEC3, 12 }, { PrimitiveTypeType::VEC4, 16 }, { PrimitiveTypeType::MAT2,
	16 }, { PrimitiveTypeType::MAT3, 36 }, { PrimitiveTypeType::MAT4, 64 } };

u32 getPrimitiveTypeSize(PrimitiveTypeType type)
{
	return typeSizes[type];
}

static UnorderedMap<PrimitiveTypeType, u32> std140TypeAlignments = { { PrimitiveTypeType::INT, 4 }, { PrimitiveTypeType::UINT, 4 },
	{ PrimitiveTypeType::BOOL, 4 }, { PrimitiveTypeType::FLOAT, 4 }, { PrimitiveTypeType::VEC2, 8 }, { PrimitiveTypeType::VEC3, 16 }, { PrimitiveTypeType::VEC4,
		16 }, { PrimitiveTypeType::MAT2, 16 }, { PrimitiveTypeType::MAT3, 16 }, { PrimitiveTypeType::MAT4, 16 } };

static u32 align(u32 offset, u32 alignment)
{
	if (alignment == 0)
	{
		err("Member alignment is 0!");
		return 0;
	}
	if (offset % alignment == 0)
	{
		return offset;
	}
	return (offset / alignment + 1) * alignment;
}

static u32 getStd140Alignment(Ref<const DataType> type);

static u32 getStd140Size(Ref<const DataType> type, u32 unsizedArrayLength)
{
	if (type->type == DataTypeType::PRIMITIVE)
	{
		auto primitive = CastDown<const PrimitiveType>(type);
		return typeSizes.at(primitive->primitiveType);
	}
	if (type->type == DataTypeType::ARRAY)
	{
		auto array = CastDown<const ArrayType>(type);
		if (array->isSized())
		{
			return align(getStd140Size(array->elementType, unsizedArrayLength), 16) * array->length;
		}
		return align(getStd140Size(array->elementType, unsizedArrayLength), 16) * unsizedArrayLength;
	}
	if (type->type == DataTypeType::STRUCT)
	{
		auto s = CastDown<const StructType>(type);
		u32 offset = 0;
		u32 maxAlignment = 0;
		for (const auto &member : s->members)
		{
			u32 alignment = getStd140Alignment(member.type);
			maxAlignment = math::max(maxAlignment, alignment);
			offset = align(offset, alignment) + getStd140Size(member.type, unsizedArrayLength);
		}
		return align(offset, align(maxAlignment, 16));
	}
	return 0;
}

static u32 getStd140Alignment(Ref<const DataType> type)
{
	if (type->type == DataTypeType::PRIMITIVE)
	{
		auto primitive = CastDown<const PrimitiveType>(type);
		return std140TypeAlignments.at(primitive->primitiveType);
	}
	if (type->type == DataTypeType::ARRAY)
	{
		auto array = CastDown<const ArrayType>(type);
		return align(getStd140Alignment(array->elementType), 16);
	}
	if (type->type == DataTypeType::STRUCT)
	{
		auto s = CastDown<const StructType>(type);
		u32 maxAlignment = 0;
		for (const auto &member : s->members)
		{
			maxAlignment = math::max(maxAlignment, getStd140Alignment(member.type));
		}
		return align(maxAlignment, 16);
	}
	return 1;
}

static Ref<StructAccessorBlueprint> createStd140StructAccessorBlueprint(Ref<const StructType> s, const Array<Ref<const StructType>> &structs,
	u32 unsizedArrayLength);

static Ref<ArrayAccessorBlueprint> createStd140ArrayAccessorBlueprint(Ref<const ArrayType> array, const Array<Ref<const StructType>> &structs,
	u32 unsizedArrayLength);

static Ref<AccessorBlueprint> createStd140PrimitiveAccessorBlueprint(Ref<const PrimitiveType> type)
{
	Ref<AccessorBlueprint> result = allocate<AccessorBlueprint>();
	result->size = getStd140Size(type, 0);
	result->alignment = getStd140Alignment(type);
	return result;
}

static Ref<AccessorBlueprint> createStd140AccessorBlueprint(Ref<const DataType> type, const Array<Ref<const StructType>> &structs, u32 unsizedArrayLength)
{
	if (type->type == DataTypeType::PRIMITIVE)
	{
		auto primitive = CastDown<const PrimitiveType>(type);
		return createStd140PrimitiveAccessorBlueprint(primitive);
	}
	if (type->type == DataTypeType::ARRAY)
	{
		auto array = CastDown<const ArrayType>(type);
		return createStd140ArrayAccessorBlueprint(array, structs, unsizedArrayLength);
	}
	if (type->type == DataTypeType::STRUCT)
	{
		auto s = CastDown<const StructType>(type);
		return createStd140StructAccessorBlueprint(s, structs, unsizedArrayLength);
	}
	return null;
}

static Ref<ArrayAccessorBlueprint> createStd140ArrayAccessorBlueprint(Ref<const ArrayType> array, const Array<Ref<const StructType>> &structs,
	u32 unsizedArrayLength)
{
	Ref<ArrayAccessorBlueprint> result = allocate<ArrayAccessorBlueprint>();
	result->elementType = createStd140AccessorBlueprint(array->elementType, structs, unsizedArrayLength);
	if (array->elementType->type == DataTypeType::PRIMITIVE)
	{
		result->elementType->size = align(result->elementType->size, 16);
		result->elementType->alignment = result->elementType->size;
	}
	if (array->isSized())
	{
		result->length = array->length;
	}
	else
	{
		result->length = unsizedArrayLength;
	}
	result->alignment = result->elementType->alignment;
	result->size = result->elementType->size * result->length;
	return result;
}

static Ref<StructAccessorBlueprint> createStd140StructAccessorBlueprint(Ref<const StructType> s, const Array<Ref<const StructType>> &structs,
	u32 unsizedArrayLength)
{
	auto result = allocate<StructAccessorBlueprint>();
	u32 offset = 0;

	for (const auto &member : s->members)
	{
		offset = align(offset, getStd140Alignment(member.type));
		result->members[member.name] = { offset, createStd140AccessorBlueprint(member.type, structs, unsizedArrayLength) };
		offset += getStd140Size(member.type, unsizedArrayLength);
	}

	if (s->members.empty())
	{
		result->size = 0;
		result->alignment = 0;
	}
	else
	{
		result->size = getStd140Size(s, unsizedArrayLength);
		result->alignment = getStd140Alignment(s);
	}

	return result;
}

Ref<StructAccessorBlueprint> createStructAccessorBlueprint(Ref<const StructType> s, const Array<Ref<const StructType>> &structs, BufferLayout layout,
	u32 unsizedArrayLength)
{

	if (layout == BufferLayout::STANDARD)
	{
		return createStd140StructAccessorBlueprint(s, structs, unsizedArrayLength);
	}
	else if (layout == BufferLayout::PACKED_NO_PADDING)
	{
		auto result = allocate<StructAccessorBlueprint>();
		u32 offset = 0;

		for (const auto &member : s->members)
		{
			if (member.type->type == DataTypeType::PRIMITIVE)
			{
				auto primitive = CastDown<const PrimitiveType>(member.type);
				result->members[member.name] = { offset, null };
				offset += typeSizes[primitive->primitiveType];
			}
			else
			{
				err("Unknown type " + toString((u32) member.type->type) + "!");
			}
		}

		result->size = offset;
		result->alignment = 1;

		return result;
	}
	return null;
}

Ref<AccessorBlueprint> createArrayBufferElementBlueprint(Ref<const DataType> type)
{
	Ref<AccessorBlueprint> elementBlueprint;
	if (type->type == DataTypeType::PRIMITIVE)
	{
		elementBlueprint = allocate<AccessorBlueprint>();
		elementBlueprint->size = getPrimitiveTypeSize(CastDown<const PrimitiveType>(type)->primitiveType);
	}
	else if (type->type == DataTypeType::STRUCT)
	{
		auto structType = CastDown<const StructType>(type);
		Array<Ref<const StructType>> structs(0);
		elementBlueprint = createStructAccessorBlueprint(structType, structs, BufferLayout::PACKED_NO_PADDING);
	}
	elementBlueprint->alignment = elementBlueprint->size;
	return elementBlueprint;
}

static UnorderedMap<ImageLayout, VkImageLayout> imageLayouts = { { ImageLayout::UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED }, { ImageLayout::COLOR_ATTACHMENT,
	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }, { ImageLayout::DEPTH_ATTACHMENT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }, { ImageLayout::PRESENT,
	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }, { ImageLayout::SHADER_READ_ONLY, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }, { ImageLayout::SHADER_STORAGE,
	VK_IMAGE_LAYOUT_GENERAL }, { ImageLayout::TRANSFER_DESTINATION, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL }, { ImageLayout::TRANSFER_SOURCE,
	VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL } };

VkImageLayout getVkImageLayout(ImageLayout layout, ImageFormatType imageType)
{
	if (imageType == ImageFormatType::DEPTH && layout == ImageLayout::SHADER_READ_ONLY)
	{
		return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
	}
	return imageLayouts.at(layout);
}

static UnorderedMap<ImageLayout, VkPipelineStageFlags> pipelineShages = { { ImageLayout::UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT }, {
	ImageLayout::COLOR_ATTACHMENT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }, { ImageLayout::DEPTH_ATTACHMENT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
	| VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT }, { ImageLayout::PRESENT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT }, { ImageLayout::SHADER_READ_ONLY,
	VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }, { ImageLayout::SHADER_STORAGE, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT
	| VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }, { ImageLayout::TRANSFER_DESTINATION, VK_PIPELINE_STAGE_TRANSFER_BIT }, { ImageLayout::TRANSFER_SOURCE,
	VK_PIPELINE_STAGE_TRANSFER_BIT } };

static UnorderedMap<ImageLayout, VkAccessFlags> accessFlags = { { ImageLayout::UNDEFINED, 0 }, { ImageLayout::COLOR_ATTACHMENT,
	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT }, { ImageLayout::DEPTH_ATTACHMENT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT }, { ImageLayout::PRESENT, 0 }, {
	ImageLayout::SHADER_READ_ONLY, VK_ACCESS_SHADER_READ_BIT }, { ImageLayout::SHADER_STORAGE, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT }, {
	ImageLayout::TRANSFER_DESTINATION, VK_ACCESS_TRANSFER_WRITE_BIT }, { ImageLayout::TRANSFER_SOURCE, VK_ACCESS_TRANSFER_READ_BIT } };

static UnorderedMap<ImageFormatType, VkImageAspectFlags> aspectFlags = { { ImageFormatType::COLOR, VK_IMAGE_ASPECT_COLOR_BIT }, { ImageFormatType::DEPTH,
	VK_IMAGE_ASPECT_DEPTH_BIT }, { ImageFormatType::STENCIL, VK_IMAGE_ASPECT_STENCIL_BIT }, { ImageFormatType::DEPTH_STENCIL, VK_IMAGE_ASPECT_DEPTH_BIT
	| VK_IMAGE_ASPECT_STENCIL_BIT } };

void pipelineBarrier(VkCommandBuffer cmd, VkImage image, ImageLayout oldLayout, ImageLayout newLayout, u32 baseLevel, u32 levelCount, ImageFormatType imageType)
{
	VkImageMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = null;

	barrier.oldLayout = getVkImageLayout(oldLayout, imageType);
	barrier.newLayout = getVkImageLayout(newLayout, imageType);

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;
	barrier.subresourceRange.aspectMask = aspectFlags.at(imageType);
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.baseMipLevel = baseLevel;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = levelCount;

	barrier.srcAccessMask = accessFlags.at(oldLayout);
	barrier.dstAccessMask = accessFlags.at(newLayout);

	vkCmdPipelineBarrier(cmd, pipelineShages.at(oldLayout), pipelineShages.at(newLayout), 0, 0, null, 0, null, 1, &barrier);
}

static UnorderedMap<SamplingMode, VkFilter> filters = { { SamplingMode::LINEAR, VK_FILTER_LINEAR }, { SamplingMode::NEAREST, VK_FILTER_NEAREST } };

VkFilter getVkFilter(SamplingMode samplingMode)
{
	return filters.at(samplingMode);
}

}

}

