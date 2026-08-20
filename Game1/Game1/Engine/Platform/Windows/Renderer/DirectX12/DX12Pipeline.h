#pragma once
#include "Shader.h"
#include <d3d12.h>
#include "System/Vector.h"

struct DX12CommandQueue;
enum RootSigParamTypes
{
	CBV,
	UAV,
	TEXTURE,
	SAMPLER,
	RSKTCount
};
extern const char* RootSigParamTypesAscii[RSKTCount];

struct CBVInfo
{
	ID3D12Resource* constBufferUploadHeap[64];
	u8* constBufferAddress[64];
	u32 BufferOffsets[64];
	u32 BufferOffsetSizes[32];
	D3D12_GPU_VIRTUAL_ADDRESS GetResourceGPUAddress(u32 FrameIndex,u32 BufferIndex);
	u32 GetBufferOffset(u32 FrameIndex, u32 BufferIndex);
	void Reset(u32 FrameIndex);
};
#define DX12_PIPELINE_HEADER_SIZE 548
#define DX12_PIPELINE_HEADER_SIZE2 560
struct DX12PipelineDesc
{
	u32 RootSigSize;
	u32 RootSigParamCount;
	u32 RootSigParamTypes[64];

	u32 CBVCount;
	u32 CBVSizes[64];

	u32 InputCount;

	u32 CompiledVSSize;
	u32 CompiledHSSize;
	u32 CompiledDSSize;
	u32 CompiledGSSize;
	u32 CompiledPSSize;
	//44 bytes
	//Variable length data

	D3D12_INPUT_ELEMENT_DESC* InputLayout;
	void* rootSigBlob;
	void* CompiledVS;
	void* CompiledHS;
	void* CompiledDS;
	void* CompiledGS;
	void* CompiledPS;
	void* Serialize(u32* TotalSize);
	void  Deserialize(void* Data);
};
struct DX12PipelineDesc2
{
	struct {
		u32 RootSigSize;
		u32 RootSigParamCount;
		u32 RootSigParamTypes[64];

		u32 CBVCount;
		u32 CBVSizes[64];

		u32 InputCount;
		u32 InputBufferCount;
		u32 InputSizes[2];

		u32 CompiledVSSize;
		u32 CompiledHSSize;
		u32 CompiledDSSize;
		u32 CompiledGSSize;
		u32 CompiledPSSize;
	} header;
	
	//44 bytes
	//Variable length data

	D3D12_INPUT_ELEMENT_DESC* InputLayout;
	void* rootSigBlob;
	void* CompiledVS;
	void* CompiledHS;
	void* CompiledDS;
	void* CompiledGS;
	void* CompiledPS;
	void* Serialize(u32* TotalSize);
	void  Deserialize(void* Data);
	bool  IsValid();
};

struct DX12Pipeline
{
	char FilePath[MAX_PATH];
	char PipelineName[32];
	
	ID3D12Device* device;
	ID3D12PipelineState* pipelineState;

	ID3D12Resource* depthStencilBuffer; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ID3D12DescriptorHeap* dsDescriptorHeap; // This is a heap for our depth/stencil buffer descriptor

	ID3D12RootSignature* rootSig;

	u32 RootSigParamCount;
	u32 RootSigParamTypes[64];
	
	u32 FrameIndex;

	CBVInfo CBVs;
	
	u32 VertexBufferElementSize;
	u32 InstanceBufferElementSize;

	bool Compute = false;

	void Create(ID3D12Device* Device, D3D12_INPUT_ELEMENT_DESC* VSInputLayout, u32 VSInputLayoutCount, VertexShader& Vs, PixelShader& Ps, ID3D12RootSignature* Signature, u32 Width, u32 Height,bool AlphaEnabled);
	void Create(ID3D12Device* Device, DX12PipelineDesc2 desc,bool DepthTesting , bool MultiSample);
	void Resize(u32 Width, u32 Height);
	void Create(ID3D12Device* Device,const s8* Name,const s8* FileName, bool DepthTesting,bool MultiSample);
	void CreateCompute(ID3D12Device* Device,const  s8* fileName);
	void Bind(DX12CommandQueue* Queue, u32 NewFrameIndex, bool Reset);
	void UpdateVSPerFrame(DX12CommandQueue* Queue, void* Data);
	void UpdateVSOnResize(void* Data);
};