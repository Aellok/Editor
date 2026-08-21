#include "DX12RenderTarget.h"
#include "Application\Engine.h"
void DX12RenderTarget::Initialize(ID3D12Device* Device,ID3D12Resource* SwapChainResource,D3D12_RESOURCE_STATES InitialState, ID3D12DescriptorHeap* rtvHeap,u32 rtvIndex,Vector Color)
{
	BackgroundColor = Color;
	Resource.Resource = SwapChainResource;
	Resource.State = InitialState;
	rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	
	rtvHandle.Offset(rtvIndex, Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	
	Device->CreateRenderTargetView(Resource.Resource, nullptr, rtvHandle);
}

void DX12RenderTarget::Destroy()
{
	Resource.Destroy();
	Resource.State = D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;
}
void DX12RenderTarget::Present(DX12CommandQueue* Queue)
{
	// Indicate that the back buffer will now be used to present.
	Resource.Transition(Queue, D3D12_RESOURCE_STATE_PRESENT);
}
void DX12RenderTarget::Reset(DX12CommandQueue* Queue)
{
	Resource.Transition(Queue, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void DX12RenderTarget::SetTarget(DX12CommandQueue* Queue,bool clear)
{
	
	Queue->SetRenderTarget(this, CD3DX12_CPU_DESCRIPTOR_HANDLE(Queue->CurrentPipeline->dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart()), true);
	
}