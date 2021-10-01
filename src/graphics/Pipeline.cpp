/*
 * Pipeline.cpp
 *
 *  Created on: Jun 17, 2021
 *      Author: konstantin
 */

#include <graphics/Pipeline.h>
#include <Logging.h>
#include <graphics/Buffer.h>

namespace matrix
{

	namespace graphics
	{

		FunctionParameterType getFunctionParameterType(bool input, bool output)
		{
			if (input)
			{
				if (output)
				{
					return FunctionParameterType::INPUT_OUTPUT;
				}
				else
				{
					return FunctionParameterType::INPUT;
				}
			}
			if (output)
			{
				return FunctionParameterType::OUTPUT;
			}
			return FunctionParameterType::VALUE;
		}

		String parseBlock(const String &source, u32 &i)
		{
			s32 level = 0;
			u32 startIndex = i;
			while (i < source.length())
			{
				if (source[i] == '{')
				{
					level++;
				}
				else if (source[i] == '}')
				{
					level--;
				}
				if (level == -1)
				{
					String result = source.substr(startIndex, i - startIndex);
					i++;
					return result;
				}
				i++;
			}
			return "";
		}

		static UnorderedMap<String, ShaderType> shaderTypes = {{"vertexShader", ShaderType::VERTEX}, {"tesselationControlShader", ShaderType::TESSELATION_CONTROL}, {"tesselationEvaluationShader", ShaderType::TESSELATION_EVALUATION}, {"geometryShader", ShaderType::GEOMETRY}, {"fragmentShader", ShaderType::FRAGMENT}, {"computeShader", ShaderType::COMPUTE}};

		static bool skipWhiteSpace(const String &source, u32 &i)
		{
			bool result = false;
			while (isspace(source[i]) && i < source.length())
			{
				i++;
				result = true;
			}
			return result;
		}

		static bool startsWithNext(const String &source, u32 &i, const String &start)
		{
			bool result = source.find(start, i) == i;
			if (result)
			{
				i += start.length();
			}
			return result;
		}

		static void printErrorLocation(const String &source, u32 errorIndex, const String &error)
		{
			for (u32 i = 0; i < source.length();)
			{
				u64 newLineIndex = source.find('\n', i);
				if (newLineIndex == String::npos)
				{
					return;
				}
				err(source.substr(i, newLineIndex - i));
				if (errorIndex >= i && errorIndex <= newLineIndex)
				{
					String spaces = "";
					for (u32 indentation = i; indentation < errorIndex; indentation++)
					{
						spaces += " ";
					}
					err(spaces + "^ " + error);
					break;
				}
				i = newLineIndex + 1;
			}
		}

		static void parseStruct(const String &source, Array<VariableSource> *target, const Array<Ref<const StructType>> &structs)
		{
			for (u32 i = 0; i < source.length();)
			{
				skipWhiteSpace(source, i);

				if (i == source.length())
				{
					return;
				}

				VariableSource variable;

				u32 typeEnd = i;
				for (; typeEnd < source.length() && !isspace(source[typeEnd]); typeEnd++)
					;
				variable.type = parseDataType(source.substr(i, typeEnd - i), structs);
				i = typeEnd;

				skipWhiteSpace(source, i);
				u32 nameEnd = source.find(';', i);
				variable.name = source.substr(i, nameEnd - i);
				i = nameEnd + 1;

				target->push_back(variable);
			}
		}

		static ShaderSource parseShader(const String &source, const Array<Ref<const StructType>> &structs)
		{
			ShaderSource result;

			for (u32 i = 0; i < source.length();)
			{
				bool handled = skipWhiteSpace(source, i);

				Array<VariableSource> *interface = null;
				if (startsWithNext(source, i, "inputs"))
				{
					interface = &result.inputs;
				}
				if (startsWithNext(source, i, "outputs"))
				{
					interface = &result.outputs;
				}

				if (interface)
				{
					skipWhiteSpace(source, i);

					if (source[i] != '{')
					{
						err("Expected token '{'!");
						return ShaderSource();
					}
					i++;

					String interfaceBlock = parseBlock(source, i);
					parseStruct(interfaceBlock, interface, structs);
				}
				else if (startsWithNext(source, i, "main"))
				{
					skipWhiteSpace(source, i);

					if (source[i] != '{')
					{
						err("Expected token '{'!");
						return ShaderSource();
					}
					i++;

					result.main.body = parseBlock(source, i);
				}

				if (!handled)
				{
					printErrorLocation(source, i, "Unexpected token in shader!");
					return ShaderSource();
				}
			}

			return result;
		}

		static u32 parseArrayLength(const String &source, u32 &i)
		{
			u32 lengthStart = i;
			for (; i < source.length() && source[i] != ']'; i++)
				;
			String lengthString = source.substr(lengthStart, i - lengthStart);
			i++;
			if (lengthString.empty())
			{
				return ArrayType::ARRAY_LENGTH_UNSIZED;
			}
			return strtoul(lengthString.c_str(), null, 10);
		}

		static ImageFormat parseFormat(const String &source)
		{
			ImageFormat format;

			for (u32 i = 0; i < source.length();)
			{
				bool handled = skipWhiteSpace(source, i);

				bool isComponentCount = startsWithNext(source, i, "componentCount");
				bool isComponentSize = startsWithNext(source, i, "componentSize");
				bool floatingPoint = startsWithNext(source, i, "floatingPoint");

				if (isComponentCount || isComponentSize || floatingPoint)
				{
					skipWhiteSpace(source, i);

					if (source[i] != '=')
					{
						err("Expected token '='!");
						break;
					}
					i++;

					skipWhiteSpace(source, i);

					u32 valueStart = i;
					for (; i < source.length() && !isspace(source[i]) && source[i] != ';'; i++)
						;
					String valueString = source.substr(valueStart, i - valueStart);

					skipWhiteSpace(source, i);

					if (source[i] != ';')
					{
						err("Expected token '{';");
						break;
					}
					i++;

					if (isComponentCount)
					{
						format.componentCount = (u8)strtoul(valueString.c_str(), nullptr, 10);
					}

					if (isComponentSize)
					{
						format.componentSize = (u8)strtoul(valueString.c_str(), nullptr, 10);
					}

					if (floatingPoint)
					{
						format.floatingPoint = valueString == "true";
					}

					handled = true;
				}

				if (!handled)
				{
					printErrorLocation(source, i, "Unexpected token in image format!");
					break;
				}
			}

			return format;
		}

		static void parseDescriptorSet(const String &source, Ref<DescriptorSetSource> target, const Array<Ref<const StructType>> &structs)
		{
			for (u32 i = 0; i < source.length();)
			{
				bool handled = skipWhiteSpace(source, i);

				bool isUniformBuffer = startsWithNext(source, i, "uniformBuffer");
				bool isStorageBuffer = startsWithNext(source, i, "storageBuffer");
				if (isUniformBuffer || isStorageBuffer)
				{
					skipWhiteSpace(source, i);

					Ref<BufferSource> buffer = allocate<BufferSource>();
					if (isUniformBuffer)
					{
						buffer->type = DescriptorType::UNIFORM_BUFFER;
					}
					else if (isStorageBuffer)
					{
						buffer->type = DescriptorType::STORAGE_BUFFER;
					}

					if (startsWithNext(source, i, "STANDARD"))
					{
						buffer->layout = BufferLayout::STANDARD;
					}
					else if (startsWithNext(source, i, "PACKED"))
					{
						buffer->layout = BufferLayout::PACKED;
					}
					else
					{
						err("Unexpected buffer layout!");
						return;
					}

					skipWhiteSpace(source, i);

					if (source[i] != '{')
					{
						err("Expected token '{'!");
						return;
					}
					i++;

					String bufferBlock = parseBlock(source, i);

					skipWhiteSpace(source, i);

					if (source[i] == '[')
					{
						i++;
						buffer->arrayLength = parseArrayLength(source, i);
						skipWhiteSpace(source, i);
					}
					else
					{
						buffer->arrayLength = 1;
					}

					u32 nameEnd = source.find(';', i);
					buffer->name = source.substr(i, nameEnd - i);
					i = nameEnd + 1;

					buffer->data = allocate<StructType>("buffer_" + buffer->name);
					parseStruct(bufferBlock, &buffer->data->members, structs);

					target->descriptors.push_back(buffer);

					handled = true;
				}

				bool texture2D = startsWithNext(source, i, "texture2D");
				bool texture3D = startsWithNext(source, i, "texture3D");
				if (texture2D || texture3D)
				{
					skipWhiteSpace(source, i);

					Ref<TextureSource> texture = allocate<TextureSource>();
					texture->dimensions = 2;
					texture->type = texture2D ? DescriptorType::IMAGE_SAMPLER_2D : DescriptorType::IMAGE_SAMPLER_3D;

					if (source[i] == '[')
					{
						i++;
						texture->arrayLength = parseArrayLength(source, i);
						skipWhiteSpace(source, i);
					}
					else
					{
						texture->arrayLength = 1;
					}

					u32 nameStart = i;
					for (; i < source.length() && !isspace(source[i]) && source[i] != ';'; i++)
						;
					texture->name = source.substr(nameStart, i - nameStart);

					skipWhiteSpace(source, i);

					if (source[i] != ';')
					{
						err("Expected token ';' in descriptor " + texture->name + " of set " + target->name + "!");
						return;
					}
					i++;

					target->descriptors.push_back(texture);

					handled = true;
				}

				if (startsWithNext(source, i, "image2D"))
				{
					skipWhiteSpace(source, i);

					Ref<ImageSource> image = allocate<ImageSource>();
					image->dimensions = 2;
					image->type = DescriptorType::IMAGE_2D;

					if (source[i] != '{')
					{
						err("Expected token '{'!");
						return;
					}
					i++;

					String formatBlock = parseBlock(source, i);
					image->format = parseFormat(formatBlock);

					skipWhiteSpace(source, i);

					if (source[i] == '[')
					{
						i++;
						image->arrayLength = parseArrayLength(source, i);
						skipWhiteSpace(source, i);
					}
					else
					{
						image->arrayLength = 1;
					}

					u32 nameStart = i;
					for (; i < source.length() && !isspace(source[i]) && source[i] != ';'; i++)
						;
					image->name = source.substr(nameStart, i - nameStart);

					skipWhiteSpace(source, i);

					if (source[i] != ';')
					{
						err("Expected token ';' in descriptor " + image->name + " of set " + target->name + "!");
						return;
					}
					i++;

					target->descriptors.push_back(image);

					handled = true;
				}

				if (startsWithNext(source, i, "TopLevelAccelerationStructure"))
				{
					skipWhiteSpace(source, i);

					Ref<DescriptorSource> accelerationStructure = allocate<DescriptorSource>();
					accelerationStructure->type = DescriptorType::TOP_LEVEL_ACCELERATION_STRUCTURE;

					if (source[i] == '[')
					{
						i++;
						accelerationStructure->arrayLength = parseArrayLength(source, i);
						skipWhiteSpace(source, i);
					}
					else
					{
						accelerationStructure->arrayLength = 1;
					}

					u32 nameStart = i;
					for (; i < source.length() && !isspace(source[i]) && source[i] != ';'; i++)
						;
					accelerationStructure->name = source.substr(nameStart, i - nameStart);

					skipWhiteSpace(source, i);

					if (source[i] != ';')
					{
						err("Expected token ';' in descriptor " + accelerationStructure->name + " of set " + target->name + "!");
						return;
					}
					i++;

					target->descriptors.push_back(accelerationStructure);

					handled = true;
				}

				if (!handled)
				{
					printErrorLocation(source, i, "Unexpected token in descriptor set!");
					return;
				}
			}
		}

		static void parseFunctionParameters(const String &source, Array<FunctionParameterSource> &parameters, const Array<Ref<const StructType>> &structs)
		{
			if (source.empty())
			{
				return;
			}

			for (u32 i = 0; i < source.length();)
			{
				skipWhiteSpace(source, i);

				FunctionParameterSource parameter;

				bool input = false;
				bool output = false;

				if (source.find("input", i) == i)
				{
					i += 5;
					input = true;
					skipWhiteSpace(source, i);
				}

				if (source.find("output", i) == i)
				{
					i += 6;
					output = true;
					skipWhiteSpace(source, i);
				}

				parameter.parameterType = getFunctionParameterType(input, output);

				u32 typeStart = i;
				for (; i < source.length() && !isspace(source[i]); i++)
					;
				String typeSource = source.substr(typeStart, i - typeStart);
				parameter.type = parseDataType(typeSource, structs);

				skipWhiteSpace(source, i);

				u32 nameStart = i;
				for (; i < source.length() && !isspace(source[i]) && source[i] != ',' && source[i] != ')'; i++)
					;
				parameter.name = source.substr(nameStart, i - nameStart);

				parameters.push_back(parameter);

				skipWhiteSpace(source, i);

				if (source[i] == ',')
				{
					i++;
					continue;
				}
				else if (i == source.length() - 1)
				{
					err("Expected token ',' or ')' in function parameter list!");
					return;
				}
			}
		}

		static bool parsePipelineIteration(const String &source, u32 &i, Ref<PipelineSource> pipeline)
		{
			bool handled = skipWhiteSpace(source, i);

			if (startsWithNext(source, i, "uniforms"))
			{
				skipWhiteSpace(source, i);

				if (source[i] != '{')
				{
					err("Expected token '{'!");
					return false;
				}
				i++;

				String uniformsBlock = parseBlock(source, i);
				parseStruct(uniformsBlock, &pipeline->uniforms, pipeline->structs);

				handled = true;
			}

			if (startsWithNext(source, i, "struct"))
			{
				skipWhiteSpace(source, i);

				Ref<StructType> structType = allocate<StructType>();

				u32 nameStart = i;
				for (; i < source.length() && !isspace(source[i]) && source[i] != '{'; i++)
					;

				structType->name = source.substr(nameStart, i - nameStart);

				skipWhiteSpace(source, i);

				if (source[i] != '{')
				{
					err("Expected token '{'!");
					return false;
				}
				i++;

				String structBlock = parseBlock(source, i);
				parseStruct(structBlock, &structType->members, pipeline->structs);

				pipeline->structs.push_back(structType);

				handled = true;
			}

			if (startsWithNext(source, i, "set"))
			{
				skipWhiteSpace(source, i);

				u32 nameStart = i;
				for (; i < source.length() && !isspace(source[i]) && source[i] != '{'; i++)
					;

				Ref<DescriptorSetSource> set = allocate<DescriptorSetSource>();
				set->name = source.substr(nameStart, i - nameStart);

				skipWhiteSpace(source, i);

				if (source[i] != '{')
				{
					err("Expected token '{'!");
					return false;
				}
				i++;

				String setBlock = parseBlock(source, i);
				parseDescriptorSet(setBlock, set, pipeline->structs);

				pipeline->descriptorSets.push_back(set);

				handled = true;
			}

			if (startsWithNext(source, i, "function"))
			{
				skipWhiteSpace(source, i);

				u32 returnTypeStart = i;
				for (; i < source.length() && !isspace(source[i]); i++)
					;

				Ref<FunctionSource> function = allocate<FunctionSource>();
				String returnTypeSource = source.substr(returnTypeStart, i - returnTypeStart);
				function->returnType = parseDataType(returnTypeSource, pipeline->structs);

				skipWhiteSpace(source, i);

				u32 nameStart = i;
				for (; i < source.length() && !isspace(source[i]) && source[i] != '('; i++)
					;
				function->name = source.substr(nameStart, i - nameStart);

				skipWhiteSpace(source, i);

				if (source[i] != '(')
				{
					err("Expected token '('!");
					return false;
				}
				i++;

				skipWhiteSpace(source, i);

				u32 parametersStart = i;
				for (; i < source.length() && source[i] != ')'; i++)
					;
				String parameterSource = source.substr(parametersStart, i - parametersStart);
				parseFunctionParameters(parameterSource, function->parameters, pipeline->structs);

				skipWhiteSpace(source, i);

				if (source[i] != ')')
				{
					err("Expected token ')'!");
					return false;
				}
				i++;

				skipWhiteSpace(source, i);

				if (source[i] != '{')
				{
					err("Expected token '{'!");
					return false;
				}
				i++;

				function->body = parseBlock(source, i);

				pipeline->functions.push_back(function);

				handled = true;
			}

			if (startsWithNext(source, i, "const"))
			{
				skipWhiteSpace(source, i);

				ConstantSource constant;

				u32 typeStart = i;
				for (; i < source.length() && !isspace(source[i]); i++)
					;
				constant.type = parseDataType(source.substr(typeStart, i - typeStart), pipeline->structs);

				skipWhiteSpace(source, i);

				u32 nameStart = i;
				for (; i < source.length() && !isspace(source[i]) && source[i] != '='; i++)
					;
				constant.name = source.substr(nameStart, i - nameStart);

				skipWhiteSpace(source, i);

				if (source[i] != '=')
				{
					err("Expected token '='!");
					return false;
				}
				i++;

				skipWhiteSpace(source, i);

				u32 valueStart = i;
				for (; i < source.length() && source[i] != ';'; i++)
					;
				constant.value = source.substr(valueStart, i - valueStart);

				skipWhiteSpace(source, i);

				if (source[i] != ';')
				{
					err("Expected token ';'!");
					return false;
				}
				i++;

				pipeline->constants.push_back(constant);

				handled = true;
			}

			if (startsWithNext(source, i, "include"))
			{
				skipWhiteSpace(source, i);

				u32 nameStart = i;
				for (; i < source.length() && !isspace(source[i]) && source[i] != ';'; i++)
					;
				String name = source.substr(nameStart, i - nameStart);

				skipWhiteSpace(source, i);

				pipeline->add(resolveShaderInclude(name));

				if (source[i] != ';')
				{
					err("Expected token ';' at include '" + name + "'!");
					return false;
				}
				i++;

				handled = true;
			}

			return handled;
		}

		Ref<GraphicsPipelineSource> parseGraphicsPipeline(const String &source)
		{
			Ref<GraphicsPipelineSource> pipeline = allocate<GraphicsPipelineSource>();

			for (u32 i = 0; i < source.length();)
			{
				bool handled = parsePipelineIteration(source, i, pipeline);

				for (const auto &shaderType : shaderTypes)
				{
					if (source.find(shaderType.first, i) == i)
					{
						i += shaderType.first.length();
						skipWhiteSpace(source, i);

						if (source[i] != '{')
						{
							err("Expected token '{'!");
							return nullptr;
						}
						i++;

						String shaderBlock = parseBlock(source, i);
						pipeline->shaders[shaderType.second] = parseShader(shaderBlock, pipeline->structs);

						handled = true;
						break;
					}
				}

				if (!handled)
				{
					printErrorLocation(source, i, "Unexpected token in pipeline!");
					return null;
				}
			}

			return pipeline;
		}

		Ref<PipelineSource> parsePipeline(const String &source)
		{
			Ref<PipelineSource> pipeline = allocate<PipelineSource>();

			for (u32 i = 0; i < source.length();)
			{
				if (!parsePipelineIteration(source, i, pipeline))
				{
					printErrorLocation(source, i, "Unexpected token in pipeline!");
					return null;
				}
			}

			return pipeline;
		}

		Ref<PipelineSource> parsePipeline(const RL &rl)
		{
			return parsePipeline(rl.readText());
		}

		static UnorderedMap<PrimitiveTypeType, Ref<const PrimitiveType>> primitiveTypeCache;

		Ref<const PrimitiveType> getPrimitiveType(PrimitiveTypeType type)
		{
			if (primitiveTypeCache.find(type) == primitiveTypeCache.end())
			{
				Ref<const PrimitiveType> primitiveType = allocate<PrimitiveType>(type);
				primitiveTypeCache[type] = primitiveType;
				return primitiveType;
			}
			return primitiveTypeCache[type];
		}

		Ref<GraphicsPipelineSource> parseGraphicsPipeline(const RL &rl)
		{
			return parseGraphicsPipeline(rl.readText());
		}

		Ref<ComputePipelineSource> parseComputePipeline(const String &source)
		{
			Ref<ComputePipelineSource> pipeline = allocate<ComputePipelineSource>();

			for (u32 i = 0; i < source.length();)
			{
				bool handled = parsePipelineIteration(source, i, pipeline);

				if (startsWithNext(source, i, "main"))
				{
					skipWhiteSpace(source, i);

					if (source[i] != '{')
					{
						err("Expected token '{'!");
						return nullptr;
					}
					i++;

					pipeline->shaderSource.main.body = parseBlock(source, i);

					handled = true;
					break;
				}

				if (startsWithNext(source, i, "localSize"))
				{
					skipWhiteSpace(source, i);

					if (source[i] == '.')
					{
						i++;

						char coord = source[i];
						i++;

						skipWhiteSpace(source, i);

						if (source[i] != '=')
						{
							err("Expected token '='!");
							return nullptr;
						}
						i++;

						skipWhiteSpace(source, i);

						u32 dimensionStart = i;
						for (; i < source.length() && !isspace(source[i]) && source[i] != ';'; i++)
							;
						u32 dimension = strtoul(source.substr(dimensionStart, i - dimensionStart).c_str(), null, 10);

						skipWhiteSpace(source, i);

						if (source[i] != ';')
						{
							err("Expected token ';'!");
							return nullptr;
						}
						i++;

						switch (coord)
						{
						case 'x':
							pipeline->localSize.x = dimension;
							break;
						case 'y':
							pipeline->localSize.y = dimension;
							break;
						case 'z':
							pipeline->localSize.z = dimension;
							break;
						default:
							err(String("Unexpected token '") + coord + "'!");
							break;
						}
					}
					else
					{
						err("Expected token '.'!");
						return nullptr;
					}

					handled = true;
				}

				if (!handled)
				{
					printErrorLocation(source, i, "Unexpected token in pipeline!");
					return null;
				}
			}

			return pipeline;
		}

		Ref<ComputePipelineSource> parseComputePipeline(const RL &rl)
		{
			return parseComputePipeline(rl.readText());
		}

		Ref<const DataType> parseDataType(const String &source, const Array<Ref<const StructType>> &structs)
		{
			bool array = false;
			u32 arrayLength = 1;
			u32 i = 0;
			u32 nameEnd = 0;
			while (i < source.length())
			{
				i++;
				nameEnd = i;
				if (source[i] == '[')
				{
					i++;
					array = true;
					arrayLength = parseArrayLength(source, i);
					break;
				}
			}
			String typeName = source.substr(0, nameEnd);
			Ref<const DataType> innerType;

			if (typeName == "int")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::INT);
			}
			else if (typeName == "uint")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::UINT);
			}
			else if (typeName == "bool")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::BOOL);
			}
			else if (typeName == "float")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::FLOAT);
			}
			else if (typeName == "vec2")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::VEC2);
			}
			else if (typeName == "vec3")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::VEC3);
			}
			else if (typeName == "vec4")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::VEC4);
			}
			else if (typeName == "mat2")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::MAT2);
			}
			else if (typeName == "mat3")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::MAT3);
			}
			else if (typeName == "mat4")
			{
				innerType = getPrimitiveType(PrimitiveTypeType::MAT4);
			}
			else
			{
				for (const auto &_struct : structs)
				{
					if (typeName == _struct->name)
					{
						innerType = _struct;
					}
				}
			}

			if (array)
			{
				return allocate<ArrayType>(innerType, arrayLength);
			}

			return innerType;
		}

		static UnorderedMap<PrimitiveTypeType, String> primitiveTypeNames = {{PrimitiveTypeType::INT, "int"}, {PrimitiveTypeType::UINT, "uint"}, {PrimitiveTypeType::BOOL, "bool"}, {PrimitiveTypeType::FLOAT, "float"}, {PrimitiveTypeType::VEC2, "vec2"}, {PrimitiveTypeType::VEC3, "vec3"}, {PrimitiveTypeType::VEC4, "vec4"}, {PrimitiveTypeType::MAT2, "mat2"}, {PrimitiveTypeType::MAT3, "mat3"}, {PrimitiveTypeType::MAT4, "mat4"}};

		String PrimitiveType::getName() const
		{
			return primitiveTypeNames.at(primitiveType);
		}

		String ArrayType::getName() const
		{
			if (isSized())
			{
				return elementType->getName() + "[" + toString(length) + "]";
			}
			return elementType->getName() + "[]";
		}

		u32 ArrayType::ARRAY_LENGTH_UNSIZED = 0;

		String StructType::getName() const
		{
			return name;
		}

		Ref<BufferSource> DescriptorSetSource::getBuffer(const String &name) const
		{
			for (auto buffer : descriptors)
			{
				if (buffer->name == name)
				{
					return CastDown<BufferSource>(buffer);
				}
			}
			return null;
		}

		Ref<DescriptorSetSource> PipelineSource::getDescriptorSet(const String &name) const
		{
			for (auto set : descriptorSets)
			{
				if (set->name == name)
				{
					return set;
				}
			}
			return null;
		}

		Ref<const StructType> PipelineSource::getStruct(const String &name) const
		{
			for (auto s : structs)
			{
				if (s->name == name)
				{
					return s;
				}
			}
			return null;
		}

		Ref<FunctionSource> PipelineSource::getFunction(const String &name) const
		{
			for (auto function : functions)
			{
				if (function->name == name)
				{
					return function;
				}
			}
			return null;
		}

		static UnorderedMap<String, Ref<const PipelineSource>> includes;

		void addShaderInclude(const String &name, Ref<const PipelineSource> source)
		{
			includes[name] = source;
		}

		static_block
		{
			addShaderInclude("math.Base", parsePipeline(RL("file:///shaders/math/base.shader")));
			addShaderInclude("math.Random", parsePipeline(RL("file:///shaders/math/random.shader")));
			addShaderInclude("math.Dither", parsePipeline(RL("file:///shaders/math/dither.shader")));
			addShaderInclude("math.ImportanceSampler", parsePipeline(RL("file:///shaders/math/importanceSampler.shader")));
			addShaderInclude("math.Tonemapper", parsePipeline(RL("file:///shaders/math/tonemapper.shader")));
			addShaderInclude("math.Intersection", parsePipeline(RL("file:///shaders/math/intersection.shader")));
		}

		Ref<const PipelineSource> resolveShaderInclude(const String &name)
		{
			if (includes.find(name) == includes.end())
			{
				err("Could not resolve include '" + name + "'!");
				return null;
			}

			return includes.at(name);
		}

		void PipelineSource::add(Ref<const PipelineSource> pipeline)
		{
#define ADD_PIPELINE(member)                                       \
	for (auto &m : pipeline->member)                               \
		if (find(member.begin(), member.end(), m) == member.end()) \
			member.push_back(m);
			ADD_PIPELINE(uniforms)
			ADD_PIPELINE(structs)
			ADD_PIPELINE(descriptorSets)
			ADD_PIPELINE(functions)
			ADD_PIPELINE(constants)
#undef ADD_PIPELINE
		}

	}

}
