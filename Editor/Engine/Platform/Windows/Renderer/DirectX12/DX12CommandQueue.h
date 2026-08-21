#pragma once
#include "DX12.h"
#include "DX12Pipeline.h"
#include <mutex>

struct DX12VertexBuffer;
struct DX12IndexBuffer;
struct DX12RenderTarget;

struct DX12CommandQueue
{
	u32 CommandListID;
	ID3D12GraphicsCommandList* List;
	ID3D12CommandQueue* Queue;
	ID3D12CommandAllocator* Allocator;
	ID3D12Resource* UploadBuffer;
	DX12Pipeline* CurrentPipeline;

	//Syncronization
	u64 fenceValue;
	HANDLE fenceEvent;
	ID3D12Fence* fence;
	
	bool oldScissorRectIndex;

	bool IsClosed = false;
	bool Initialize(ID3D12Device* Device);
	void SetIndexBuffer(DX12IndexBuffer* View);
	void SetVertexBuffer(DX12VertexBuffer** View, u32 StartSlot, u32 Count);
	void WaitForGPU();
	void Execute(ID3D12CommandList* const* commandLists,u32 Count);
	void ExecuteAndWait();
	void SetPipeline(DX12Pipeline* Pipeline);
	void SetRenderTarget(DX12RenderTarget* RenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilHandle, bool Clear);
	void SetViewport(D3D12_VIEWPORT* Viewport);
	void SetScissorRect(D3D12_RECT* ScissorRect);
	void SetTopology(D3D_PRIMITIVE_TOPOLOGY Type);
	void ClearDepthStencil(DX12Pipeline* pipeline);
	void Reset(DX12Pipeline* pipeline);
	void SetConstantBuffer(u32 Index, D3D12_GPU_VIRTUAL_ADDRESS BufferAddress);
	void DrawInstanced(u32 IndexCount, u32 InstancedCount, u32 StartVertex, u32 StartInstance);
	void DrawIndexedInstanced(u32 IndexCount, u32 InstanceCount, u32 StartIndex, s32 BaseVertex, u32 StartInstance);
	void SetDescriptorTable(u32 ParamIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);
	void SetComputeDescriptorTable(u32 ParamIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);
	void SetDescriptorHeaps(u32 Count, ID3D12DescriptorHeap* const* Heaps);
	void UpdateSubresource(ID3D12Resource* Resource, ID3D12Resource* UploadBuffer, u32 UploadBufferOffset,u32 Index, D3D12_SUBRESOURCE_DATA* src);
	void UndoScissorRect();
	void Dispatch();
	void CloseCommandList();
};