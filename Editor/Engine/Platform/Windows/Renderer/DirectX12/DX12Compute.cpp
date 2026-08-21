
#include "DX12Compute.h"

void DX12Compute::Initialize(DirectX12* DX12, const s8* Shader)
{
	UAV.Initialize(&DX12->MainCommandQueue, DX12, 100, 100);
	pipeline.CreateCompute(DX12->device, Shader);
}
void DX12Compute::Execute(DX12CommandQueue* Queue)
{
	Queue->SetPipeline(&pipeline);
	ID3D12DescriptorHeap* descheap[1] = { UAV.Heap };
	Queue->SetDescriptorHeaps(1, descheap);
	Queue->SetComputeDescriptorTable(0, UAV.Heap->GetGPUDescriptorHandleForHeapStart());
	Queue->Dispatch();
}
