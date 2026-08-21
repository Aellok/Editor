#pragma once
#include "../Parser/ParserDataTypes.h"

struct InputLayout
{
	DynamicArray SemanticNames;
	//D3D12_INPUT_ELEMENT_DESC
	DynamicArray Inputs;	

	u32 BufferSizes[2]; // stride of (1) Vertex buffer (2) InstanceBuffer;
};

InputLayout BuildInputLayout(ParsedVSInput VSInput);