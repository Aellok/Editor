#include "DX12VertexBuffer.h"
#include "VertexDef.h"
void DX12VertexBuffer::Initialize(ID3D12Device* Device, DX12CommandQueue* Queue, DX12VertexBufferDesc Desc)
{
	Size = Desc.Size;
	RawData = calloc(1, Size);
	memcpy(RawData, Desc.Data, Size);

	Resource.Initialize(Device, Desc.ResourceDesc,Desc.Size, D3D12_RESOURCE_STATE_COMMON, {});

	VertexData.pData = RawData;
	VertexData.RowPitch = Size;
	VertexData.SlicePitch = Size;

	Resource.UpdateSubresource(Queue, 0, 0, &VertexData);
	Resource.Transition(Queue,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	
	View.BufferLocation = Resource.GetGPUAddress();
	View.StrideInBytes = Desc.Stride;// sizeof(Vertex2D);
	View.SizeInBytes = Size;
}

void DX12VertexBuffer::Update(DX12CommandQueue* Queue,void* Data)
{
	memcpy(RawData, Data, Size);
	Resource.Transition(Queue, D3D12_RESOURCE_STATE_COMMON);
	Resource.UpdateSubresource(Queue, 0, 0, &VertexData);
	Resource.Transition(Queue, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
}