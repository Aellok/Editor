#pragma once
#include "DirectX.h"
#include "DX12IndexBuffer.h"
#include "DX12VertexBuffer.h"
#include "System\Memory\Arena.h"

struct Instance3DDataDesc
{
	void* InstanceData;
	u32 InstanceDataSize;
	u32 InstanceElementSize;
};

struct DX12Object3D
{
	DirectX12* DX12;

	DX12VertexBuffer VertexBuffer;

	DX12VertexBuffer InstancedVB;
	//TODO hack move this to ObjectManager
	u32 InstancedTextureID;

	u32 InstanceCount;

	u32 InstanceDataElementSize;
	void* InstanceData;

	D3D12_VERTEX_BUFFER_VIEW InstancedVertexBufferView;
	DX12IndexBuffer IndexBuffer;

	DirectX::XMMATRIX Model;

	ID3D12Resource* constBufferUploadHeap[2];

	ID3D12Fence* fence;

	void* VertList;
	u32 VertCount;

	u32 IndexCount;
	void* IndexList;
	
	void Init(DirectX12* Context, MemoryArena* Arena, DX12VertexBufferDesc VDesc, DX12IndexBufferDesc IDesc, Instance3DDataDesc IDDesc);
	void DrawInstanced(DX12CommandQueue* Queue, void* Buffer, u32 InstanceNum, u32 PipelineIndex);
	void Draw(DX12CommandQueue* Queue, DX12Pipeline* Pipeline);
};