#pragma once
#include "DX12.h"
#include "VertexDef.h"
#include "Engine/System/Memory/Arena.h"
#include "DX12IndexBuffer.h"
#include "DX12VertexBuffer.h"
#define OBJECT2D_MAX_STATIC_INSTANCE 1024
#define OBJECT2D_MAX_DYNAMIC_INSTANCE 255

struct DirectX12;

struct InstanceDataDesc
{
	void* InstanceData;
	u32 InstanceDataSize;
	u32 InstanceElementSize;
};
struct DX12Object2D
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

	bool IsCentered = true;

	void Init(DirectX12* Context, MemoryArena* Arena, DX12VertexBufferDesc VDesc, DX12IndexBufferDesc IDesc, InstanceDataDesc IDDesc);
	void DrawInstanced(DX12CommandQueue* Queue, void* Buffer, u32 InstanceNum, u32 PipelineIndex);
	void Draw(DX12CommandQueue* Queue, DX12Pipeline* Pipeline,Vector color);
	int AddInstancedObject(Matrix Model);
	void UpdateInstancedObject(int ID, Matrix Model);
	void DeleteInstancedObject(int ID);
};