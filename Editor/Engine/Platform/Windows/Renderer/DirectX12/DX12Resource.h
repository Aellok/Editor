#pragma once
#include "DX12.h"
#include "DX12CommandQueue.h"
struct DX12Resource
{
	ID3D12Resource* Resource;
	ID3D12Resource* UploadBuffer;
	D3D12_RESOURCE_STATES State;
	void Initialize(ID3D12Device* Device, D3D12_RESOURCE_DESC Desc, u32 Size, D3D12_RESOURCE_STATES StartingState,Vector Color);
	void UpdateSubresource(DX12CommandQueue* Queue, u32 UploadBufferOffset,u32 Index, D3D12_SUBRESOURCE_DATA* src);
	void Transition(DX12CommandQueue* Queue, D3D12_RESOURCE_STATES DestState);
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress();
	void Destroy();

	void* GetData();
	void CopyResource(DX12CommandQueue* Queue, DX12Resource* SrcResource);
};