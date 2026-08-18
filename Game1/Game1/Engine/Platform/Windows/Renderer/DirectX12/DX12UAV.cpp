#include "DX12UAV.h"
#include "DX12.h"

void DX12UAV::Initialize(DX12CommandQueue* Queue,DirectX12* DX12,u32 Width,u32 Height)
{
	D3D12_RESOURCE_DESC ResourceDesc = {};
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.Width = Width;
	ResourceDesc.Height = Height;
	ResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	ResourceDesc.MipLevels = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	resource.Initialize(DX12->device,ResourceDesc,4 * Width * Height, D3D12_RESOURCE_STATE_COMMON, {});

	D3D12_DESCRIPTOR_HEAP_DESC DescHeap = {};
	DescHeap.NumDescriptors = 1;
	DescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	
	DX12->device->CreateDescriptorHeap(&DescHeap, IID_PPV_ARGS(&Heap));

	D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0;
	UAVDesc.Texture2D.PlaneSlice = 0;
	DX12->device->CreateUnorderedAccessView(resource.Resource, nullptr, &UAVDesc, Heap->GetCPUDescriptorHandleForHeapStart());
}