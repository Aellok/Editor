#pragma once

#include "DX12.h"
#include "System/Types.h"
#include "DX12Resource.h"
#include "DX12CommandQueue.h"

struct DX12VertexBufferDesc
{
	u32 Count;
	void* Data;
	u32 Size;
	u32 Stride;
	D3D12_RESOURCE_DESC ResourceDesc;
};
struct DX12VertexBuffer
{
	u32 Size;
	void* RawData;
	DX12Resource Resource;
	D3D12_VERTEX_BUFFER_VIEW View;
	D3D12_SUBRESOURCE_DATA VertexData;

	void Initialize(ID3D12Device* Device, DX12CommandQueue* Queue, DX12VertexBufferDesc Desc);
	void Update(DX12CommandQueue* Queue, void* VertexData);
};
