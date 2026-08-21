#pragma once
#include "DX12.h"
#include "DX12Resource.h"
#include "DX12CommandQueue.h"
//TODO: do the same thing for VertBuffer
struct DX12IndexBufferDesc
{
	u32 Count;
	u32* Data;
	u32 Size;
	D3D12_RESOURCE_DESC ResourceDesc;
};
struct DX12IndexBuffer
{
	u32 Size;
	u32* Data;
	DX12Resource Resource;
	D3D12_INDEX_BUFFER_VIEW View;

	void Initialize(ID3D12Device* Device, DX12CommandQueue* Queue,DX12IndexBufferDesc Desc);
	void Destroy();
};
