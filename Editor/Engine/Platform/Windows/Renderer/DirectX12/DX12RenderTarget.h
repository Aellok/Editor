#pragma once

#include "DX12.h"
#include "DX12Resource.h"
#include "DX12CommandQueue.h"
struct DX12RenderTarget
{
	DX12Resource Resource;
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	Vector BackgroundColor;

	void Initialize(ID3D12Device* Device, ID3D12Resource* SwapChainResource, D3D12_RESOURCE_STATES InitialState,ID3D12DescriptorHeap* rtvHeap, u32 rtvIndex,Vector Color);
	void SetTarget(DX12CommandQueue* Queue,  bool clear);
	void Present(DX12CommandQueue* Queue);
	void Reset(DX12CommandQueue* Queue);
	void Destroy();

};