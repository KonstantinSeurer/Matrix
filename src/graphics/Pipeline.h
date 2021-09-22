/*
 * Pipeline.h
 *
 *  Created on: Jun 12, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_PIPELINE_H_
#define GRAPHICS_PIPELINE_H_

#include "../Base.h"
#include "../RL.h"
#include "Descriptor.h"
#include "Buffer.h"
#include "../math/Vec3i.h"
#include "Image.h"

namespace matrix {

namespace graphics {

enum class PipelineType {
	GRAPHICS, COMPUTE
};

enum class ShaderType {
	VERTEX, TESSELATION_CONTROL, TESSELATION_EVALUATION, GEOMETRY, FRAGMENT, COMPUTE
};

enum class FunctionParameterType {
	VALUE, INPUT, OUTPUT, INPUT_OUTPUT
};

FunctionParameterType getFunctionParameterType(bool input, bool output);

enum class DataTypeType {
	PRIMITIVE, STRUCT, ARRAY
};

enum class PrimitiveTypeType {
	FLOAT, BOOL, INT, UINT, VEC2, VEC3, VEC4, MAT2, MAT3, MAT4
};

struct DataType {
	DataTypeType type;

	DataType(DataTypeType type) :
		type(type) {
	}

	virtual ~DataType() {
	}

	virtual String getName() const = 0;
};

struct VariableSource {
	Ref<const DataType> type;
	String name;

	VariableSource() :
		type(null), name("") {
	}

	VariableSource(Ref<const DataType> type, const String &name) :
		type(type), name(name) {
	}

	bool operator==(const VariableSource &other) const {
		return type == other.type && name == other.name;
	}
};

struct ConstantSource: public VariableSource {
	String value;

	ConstantSource() :
		VariableSource(), value("") {
	}

	ConstantSource(Ref<const DataType> type, const String &name) :
		VariableSource(type, name), value("") {
	}

	ConstantSource(Ref<const DataType> type, const String &name, const String &value) :
		VariableSource(type, name), value(value) {
	}

	bool operator==(const ConstantSource &other) const {
		return type == other.type && name == other.name && value == other.value;
	}
};

struct PrimitiveType: public DataType {
	PrimitiveTypeType primitiveType;

	PrimitiveType(PrimitiveTypeType primitiveType) :
		DataType(DataTypeType::PRIMITIVE), primitiveType(primitiveType) {
	}

	virtual String getName() const;
};

Ref<const PrimitiveType> getPrimitiveType(PrimitiveTypeType type);

struct ArrayType: public DataType {
	Ref<const DataType> elementType;
	u32 length;

	ArrayType(Ref<const DataType> elementType, u32 length) :
		DataType(DataTypeType::ARRAY), elementType(elementType), length(length) {
	}

	bool isSized() const {
		return length > 0;
	}

	virtual String getName() const;

	static u32 ARRAY_LENGTH_UNSIZED;
};

struct StructType: public DataType {
	String name;
	Array<VariableSource> members;

	StructType() :
		DataType(DataTypeType::STRUCT) {
	}

	StructType(const String &name) :
		DataType(DataTypeType::STRUCT), name(name) {
	}

	StructType(const String &name, const Array<VariableSource> &members) :
		DataType(DataTypeType::STRUCT), name(name), members(members) {
	}

	virtual String getName() const;
};

Ref<const DataType> parseDataType(const String &source, const Array<Ref<const StructType>> &structs);

struct FunctionParameterSource: public VariableSource {
	FunctionParameterType parameterType;
};

struct FunctionSource {
	Ref<const DataType> returnType;
	String name;
	Array<FunctionParameterSource> parameters;
	String body;
};

struct ShaderSource {
	FunctionSource main;
	Array<VariableSource> inputs;
	Array<VariableSource> outputs;
};

struct DescriptorSource {
	DescriptorType type;
	String name;
	u32 arrayLength;

	DescriptorSource() :
		type(DescriptorType::UNIFORM_BUFFER), name(""), arrayLength(1) {
	}

	DescriptorSource(DescriptorType type, const String &name, u32 arrayLength) :
		type(type), name(name), arrayLength(arrayLength) {
	}

	virtual ~DescriptorSource() {
	}
};

struct BufferSource: public DescriptorSource {
	Ref<StructType> data;
	BufferLayout layout;
};

struct DescriptorSetSource {
	String name;
	Array<Ref<DescriptorSource>> descriptors;

	Ref<BufferSource> getBuffer(const String &name) const;
};

struct TextureSource: public DescriptorSource {
	u32 dimensions;
};

struct ImageSource: public DescriptorSource {
	u32 dimensions;
	ImageFormat format;
};

enum class Topology {
	TRIANGLE_LIST, TRIANGLE_STRIP, LINE_LIST, LINE_STRIP, POINT_LIST, PATCH_LIST
};

enum class FrontFace {
	CLOCKWISE, COUNTER_CLOCKWISE
};

enum class DrawMode {
	FILL, EDGES, VERTICES
};

enum class CompareOperation {
	EQUAL, LESS, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL, ALWAYS, NEVER, NOT_EQUAL
};

enum class BlendFactor {
	ONE,
	ZERO,
	SOURCE_ALPHA,
	DESTINATION_ALPHA,
	SOURCE_COLOR,
	DESTINATION_COLOR,
	ONE_MINUS_SOURCE_ALPHA,
	ONE_MINUS_DESTINATION_ALPHA,
	ONE_MINUS_SOURCE_COLOR,
	ONE_MINUS_DESTINATION_COLOR
};

enum class BlendOperation {
	ADD, SUBTRACT, MIN, MAX
};

struct PipelineSource {
	Array<VariableSource> uniforms;
	Array<Ref<const StructType>> structs;
	Array<Ref<DescriptorSetSource>> descriptorSets;
	Array<Ref<FunctionSource>> functions;
	Array<ConstantSource> constants;

	Ref<DescriptorSetSource> getDescriptorSet(const String &name) const;
	Ref<const StructType> getStruct(const String &name) const;
	Ref<FunctionSource> getFunction(const String &name) const;

	void add(Ref<const PipelineSource> pipeline);
};

struct GraphicsPipelineSource: public PipelineSource {
	UnorderedMap<ShaderType, ShaderSource> shaders;

	Topology topology = Topology::TRIANGLE_LIST;
	u32 patchSize = 1;

	bool cullFront = false;
	bool cullBack = false;
	FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;

	DrawMode drawMode = DrawMode::FILL;

	bool depthTest = false;
	bool depthClamp = false;
	bool depthWrite = false;
	CompareOperation depthCompareOperation = CompareOperation::LESS;

	bool blend = false;

	BlendFactor sourceColorFactor = BlendFactor::SOURCE_ALPHA;
	BlendFactor destinationColorFactor = BlendFactor::ONE_MINUS_SOURCE_ALPHA;
	BlendOperation colorBlendOperation = BlendOperation::ADD;

	BlendFactor sourceAlphaFactor = BlendFactor::ONE;
	BlendFactor destinationAlphaFactor = BlendFactor::ZERO;
	BlendOperation alphaBlendOperation = BlendOperation::ADD;
};

struct ComputePipelineSource: public PipelineSource {
	ShaderSource shaderSource;

	math::Vec3u32 localSize;
};

class GraphicsPipeline {
private:
	Ref<GraphicsPipelineSource> source;
public:
	GraphicsPipeline(Ref<GraphicsPipelineSource> source) :
		source(source) {
	}

	virtual ~GraphicsPipeline() {
	}

	Ref<const GraphicsPipelineSource> getSource() const {
		return source;
	}
};

class ComputePipeline {
private:
	Ref<ComputePipelineSource> source;
public:
	ComputePipeline(Ref<ComputePipelineSource> source) :
		source(source) {
	}

	virtual ~ComputePipeline() {
	}

	Ref<const ComputePipelineSource> getSource() const {
		return source;
	}
};

Ref<PipelineSource> parsePipeline(const String &source);
Ref<PipelineSource> parsePipeline(const RL &rl);

Ref<GraphicsPipelineSource> parseGraphicsPipeline(const String &source);
Ref<GraphicsPipelineSource> parseGraphicsPipeline(const RL &rl);

Ref<ComputePipelineSource> parseComputePipeline(const String &source);
Ref<ComputePipelineSource> parseComputePipeline(const RL &rl);

void addShaderInclude(const String &name, Ref<const PipelineSource> source);

Ref<const PipelineSource> resolveShaderInclude(const String &name);

}

}

#endif /* GRAPHICS_PIPELINE_H_ */
