#include "DX12IndexBuffer.h"

void DX12IndexBuffer::Initialize(ID3D12Device* Device,DX12CommandQueue* Queue, DX12IndexBufferDesc Desc)
{
	Resource.Initialize(Device, Desc.ResourceDesc,Desc.Size, D3D12_RESOURCE_STATE_COMMON, {});

	D3D12_SUBRESOURCE_DATA IndexData = {};
	IndexData.pData = Desc.Data;
	IndexData.RowPitch = Desc.Size;
	IndexData.SlicePitch = Desc.Size;
	
	Resource.UpdateSubresource(Queue,0,0, &IndexData);

	Resource.Transition(Queue, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	View.BufferLocation = Resource.GetGPUAddress();
	View.Format = DXGI_FORMAT_R32_UINT;
	View.SizeInBytes = Desc.Size;
}
void DX12IndexBuffer::Destroy()
{
	Resource.Destroy();
}
