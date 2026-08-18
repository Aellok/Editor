#pragma once
#include "System\Types.h"
#include "Platform\Windows\Renderer\DirectX12\d3dx12.h"
#include "Platform\Windows\Renderer\DirectX12\DX12CommandQueue.h"
struct DX12InstanceVertexBuffer 
{
	u32 MaxSize;
	u32 InstanceSize;
	ID3D12Resource* GPUBuffer;
	void* CPUHandle;

	void Initialize(void* Data,u32 TotalSize,u32 InstanceSize);
	void Update(void* InstanceData, u32 Size);
	void Bind(DX12CommandQueue* Queue, u32 Slot);
	void ResizeBuffer(u32 InstanceSize, u32 Count);
	void Uninitialize();
};