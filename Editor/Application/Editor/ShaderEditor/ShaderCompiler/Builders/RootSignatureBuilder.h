#pragma once
#include "..\Lexer\LexerDataTypes.h"
#include "..\Parser\ParserDataTypes.h"
#include "Platform\Windows\Renderer\DirectX12\d3dx12.h"
struct RootSignature
{
	//u32
	DynamicArray RootParamTypes;
	//D3D12_ROOT_PARAMETER 
	DynamicArray RootParams;
	//D3D12_STATIC_SAMPLER_DESC 
	DynamicArray Samplers;
	//D3D12_DESCRIPTOR_RANGE
	DynamicArray DescriptorRanges;
	//u32
	DynamicArray BufferSizes;

	D3D12_ROOT_SIGNATURE_DESC Desc;
	ID3DBlob* CompiledBlob;
};


RootSignature BuildRootSignature(VSParserResult vsInfo, PSParserResult psInfo);
