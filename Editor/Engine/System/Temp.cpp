#include "Temp.h"
#include "Application\Engine.h"
void DX12InstanceVertexBuffer::Initialize(void* Data, u32 InstanceCount, u32 InInstanceSize)
{
	InstanceSize = InInstanceSize;
	MaxSize = InstanceCount * InstanceSize; 
	ID3D12Device* device = GEngine.pRendererInterface->device;
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(MaxSize);

	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&GPUBuffer));
	GPUBuffer->Map(0,nullptr, &CPUHandle);
	
	Update(Data,InstanceCount);
}
void DX12InstanceVertexBuffer::Update(void* InstanceData,u32 Count)
{
	while (MaxSize <= Count * InstanceSize)
	{
		printf("DX12InstanceVertexBuffer::Update in size(%d) > MaxSize (%d). Resizing Buffer to (%d)\n",Count * InstanceSize, MaxSize,InstanceSize * Count * 2);
		ResizeBuffer(InstanceSize,Count * 2);
	}
	memcpy(CPUHandle, InstanceData, Count * InstanceSize);
}
void DX12InstanceVertexBuffer::Bind(DX12CommandQueue* Queue,u32 Slot)
{
	// Set Instance Data Buffer
	D3D12_VERTEX_BUFFER_VIEW instView = {};
	instView.BufferLocation = GPUBuffer->GetGPUVirtualAddress();
	instView.SizeInBytes = MaxSize;
	instView.StrideInBytes = InstanceSize;

	Queue->List->IASetVertexBuffers(Slot, 1, &instView); // Set in Slot 1
}
void DX12InstanceVertexBuffer::ResizeBuffer(u32 InInstanceSize,u32 InCount)
{
	Uninitialize();
	MaxSize = InCount * InInstanceSize;
	InstanceSize = InInstanceSize;

	ID3D12Device* device = GEngine.pRendererInterface->device;
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(MaxSize);
	
	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&GPUBuffer));
	GPUBuffer->Map(0, nullptr, &CPUHandle);
}
void DX12InstanceVertexBuffer::Uninitialize()
{
	MaxSize = 0;
	InstanceSize = 0;
	if (GPUBuffer)
	{
		GPUBuffer->Unmap(0, nullptr);
		GPUBuffer->Release();
		GPUBuffer = nullptr;
	}
}