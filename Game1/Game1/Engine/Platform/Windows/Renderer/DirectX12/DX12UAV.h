#pragma once
#include "DX12CommandQueue.h"
#include "DX12Resource.h"
#include "DirectX.h"

struct DX12UAV
{
	DX12Resource resource;
	ID3D12DescriptorHeap* Heap;
	void Initialize(DX12CommandQueue* Queue, DirectX12* DX12, u32 Width, u32 Height);
};