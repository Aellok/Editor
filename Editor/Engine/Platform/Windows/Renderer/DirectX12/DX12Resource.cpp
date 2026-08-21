#include "DX12Resource.h"

void DX12Resource::Initialize(ID3D12Device* Device, D3D12_RESOURCE_DESC Desc,u32 Size,D3D12_RESOURCE_STATES StartingState,Vector Color)
{
	State = StartingState;
	CD3DX12_HEAP_PROPERTIES Properties(D3D12_HEAP_TYPE_DEFAULT);
	//Create the defualt heap that will hold the data;
	if ( (Desc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)) != 0)
	{
		D3D12_CLEAR_VALUE value;
		value.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		value.Color[0] = Color.m128_f32[0];
		value.Color[1] = Color.m128_f32[1];
		value.Color[2] = Color.m128_f32[2];
		value.Color[3] = Color.m128_f32[3];

		Device->CreateCommittedResource(&Properties, D3D12_HEAP_FLAG_NONE, &Desc, State, &value, IID_PPV_ARGS(&Resource));
	}
	else
	{
		Device->CreateCommittedResource(&Properties, D3D12_HEAP_FLAG_NONE, &Desc, State, nullptr, IID_PPV_ARGS(&Resource));
	}
	
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(Size);
	
	Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	Device->CreateCommittedResource(&Properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&UploadBuffer));
	
}
void DX12Resource::Transition(DX12CommandQueue* Queue, D3D12_RESOURCE_STATES DestState)
{
	if (DestState == State)
	{
		return;
	}
	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(Resource, State, DestState);
	Queue->List->ResourceBarrier(1, &Barrier);
	State = DestState;
}
void DX12Resource::Destroy()
{
	Resource->Release();
	//UploadBuffer->Release();
}
void DX12Resource::UpdateSubresource(DX12CommandQueue* Queue,u32 UploadBufferOffset,u32 Index, D3D12_SUBRESOURCE_DATA* src)
{
	Transition(Queue, D3D12_RESOURCE_STATE_COPY_DEST);
	Queue->UpdateSubresource(Resource, UploadBuffer, UploadBufferOffset,Index, src);
}
D3D12_GPU_VIRTUAL_ADDRESS DX12Resource::GetGPUAddress()
{
	return Resource->GetGPUVirtualAddress();
}

void DX12Resource::CopyResource(DX12CommandQueue* Queue,DX12Resource* SrcResource)
{
	CD3DX12_RESOURCE_BARRIER Barrier[] =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(Resource, State, D3D12_RESOURCE_STATE_COMMON),
		CD3DX12_RESOURCE_BARRIER::Transition(SrcResource->Resource, SrcResource->State, D3D12_RESOURCE_STATE_COPY_SOURCE)
	};

	Queue->List->ResourceBarrier(2, Barrier);

	Queue->List->CopyResource(Resource, SrcResource->Resource);
	
	CD3DX12_RESOURCE_BARRIER Barrier2[] =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_COMMON, State),
		CD3DX12_RESOURCE_BARRIER::Transition(SrcResource->Resource, D3D12_RESOURCE_STATE_COPY_SOURCE,SrcResource->State)
	};
	Queue->List->ResourceBarrier(2, Barrier2);

	//Queue->WaitForGPU();
}