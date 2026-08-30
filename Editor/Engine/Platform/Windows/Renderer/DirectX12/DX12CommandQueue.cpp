#include "DX12CommandQueue.h"
#include "DX12IndexBuffer.h"
#include "DX12VertexBuffer.h"
#include "DX12RenderTarget.h"
#include <mutex>
std::mutex CommandListLocks[8];
static u32 CommandListIDCounter = 0;
// 1mb
#define UPLOAD_BUFFER_SIZE 256*1024*1024 
bool DX12CommandQueue::Initialize(ID3D12Device* Device)
{
	CommandListID = CommandListIDCounter++;
	CommandListLocks[CommandListID].lock();
	//Create command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&Queue))))
	{
		CommandListLocks[CommandListID].unlock();
		return false;
	}
	//Create Command Allocator
	if (FAILED(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Allocator))))
	{
		CommandListLocks[CommandListID].unlock();
		return false;
	}
	// Create the command list.
	if (FAILED(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Allocator, nullptr, IID_PPV_ARGS(&List))))
	{
		CommandListLocks[CommandListID].unlock();
		return false;
	}

	CD3DX12_HEAP_PROPERTIES Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(UPLOAD_BUFFER_SIZE);

	Device->CreateCommittedResource(&Properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&UploadBuffer));

	if (FAILED(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
	{
		CommandListLocks[CommandListID].unlock();
		return false;
	}
	fenceValue = 1;

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	CommandListLocks[CommandListID].unlock();
	return true;
}
void DX12CommandQueue::Execute(ID3D12CommandList* const* commandLists,u32 Count)
{
	CommandListLocks[CommandListID].lock();
	Queue->ExecuteCommandLists(1, commandLists);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::ExecuteAndWait()
{
	CloseCommandList();
	ID3D12CommandList* CommandLists[1]{ List };
	Execute(CommandLists, 1);
	WaitForGPU();
}
void DX12CommandQueue::Reset(DX12Pipeline* pipeline)
{
	CommandListLocks[CommandListID].lock();
	//Reset command Allocator after gpu has finished.
	if (FAILED(Allocator->Reset()))
	{
		CommandListLocks[CommandListID].unlock();
		return;
	}
	//reset the commandList
	
	if (FAILED(List->Reset(Allocator, pipeline ? pipeline->pipelineState : nullptr)))
	{
		CommandListLocks[CommandListID].unlock();
		return;
	}
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetIndexBuffer(DX12IndexBuffer* IndexBuffer)
{
	CommandListLocks[CommandListID].lock();
	List->IASetIndexBuffer(&IndexBuffer->View);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetVertexBuffer(DX12VertexBuffer** VertexBuffer,u32 StartSlot,u32 Count)
{
	D3D12_VERTEX_BUFFER_VIEW buffers[16] = {0};
	for (u32 i = 0; i < Count;i++)
	{
		buffers[i] = VertexBuffer[i]->View;
	}
	CommandListLocks[CommandListID].lock();
	List->IASetVertexBuffers(StartSlot, Count, buffers);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::WaitForGPU()
{
	//CommandListLocks[CommandListID].lock();
	const u64 fenceVal = fenceValue;
	Queue->Signal(fence, fenceVal); // Set the fence value
	if (fence->GetCompletedValue() < fenceVal)//
	{
		fence->SetEventOnCompletion(fenceVal, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	fenceValue++;
	//CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetPipeline(DX12Pipeline* Pipeline)
{
	CurrentPipeline = Pipeline;
	CommandListLocks[CommandListID].lock();
	
	if (!Pipeline->Compute)
	{
		List->SetGraphicsRootSignature(Pipeline->rootSig);
	}
	else
	{
		List->SetComputeRootSignature(Pipeline->rootSig);
	}
	
	List->SetPipelineState(Pipeline->pipelineState);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::ClearDepthStencil(DX12Pipeline* pipeline)
{
	CommandListLocks[CommandListID].lock();
	List->ClearDepthStencilView(pipeline->dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetRenderTarget(DX12RenderTarget* RenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilHandle,bool Clear)
{
	CommandListLocks[CommandListID].lock();
	List->OMSetRenderTargets(1, &RenderTarget->rtvHandle, FALSE, &DepthStencilHandle);
	if (Clear)
	{
		List->ClearRenderTargetView(RenderTarget->rtvHandle, RenderTarget->BackgroundColor.m128_f32, 0, nullptr);
	}
	
	
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetViewport(D3D12_VIEWPORT* Viewport)
{
	CommandListLocks[CommandListID].lock();
	List->RSSetViewports(1, Viewport);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetScissorRect(D3D12_RECT* ScissorRect)
{
	CommandListLocks[CommandListID].lock();
	List->RSSetScissorRects(1, ScissorRect);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetTopology(D3D_PRIMITIVE_TOPOLOGY Type)
{
	CommandListLocks[CommandListID].lock();
	List->IASetPrimitiveTopology(Type);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetConstantBuffer(u32 Index,D3D12_GPU_VIRTUAL_ADDRESS BufferAddress)
{
	if (Index >= CurrentPipeline->RootSigParamCount && !(CurrentPipeline->RootSigParamTypes[Index] == RootSigParamTypes::CBV))
	{
		printf("Error: DX12CommandQueue: SetConstantBuffer failed, the parameter type at index %d is a CBV not %s  \n", Index,RootSigParamTypesAscii[Index]);
		return;
	}
	CommandListLocks[CommandListID].lock();
	List->SetGraphicsRootConstantBufferView(Index, BufferAddress);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::DrawInstanced(u32 IndexCount,u32 InstancedCount,u32 StartVertex,u32 StartInstance)
{
	if (InstancedCount > 0)
	{
		CommandListLocks[CommandListID].lock();
		List->DrawInstanced(IndexCount, InstancedCount, StartVertex, StartInstance);
		CommandListLocks[CommandListID].unlock();
	}
}
void DX12CommandQueue::DrawIndexedInstanced(u32 IndexCount,u32 InstanceCount,u32 StartIndex,s32 BaseVertex,u32 StartInstance)
{
	CommandListLocks[CommandListID].lock();
	List->DrawIndexedInstanced(IndexCount, InstanceCount, StartIndex, BaseVertex, StartInstance);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetComputeDescriptorTable(u32 ParamIndex, D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHandle)
{
	CommandListLocks[CommandListID].lock();
	List->SetComputeRootDescriptorTable(ParamIndex, DescriptorHandle);
	CommandListLocks[CommandListID].unlock();

}
void DX12CommandQueue::SetDescriptorTable(u32 ParamIndex, D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHandle)
{
	CommandListLocks[CommandListID].lock();
	List->SetGraphicsRootDescriptorTable(ParamIndex, DescriptorHandle);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::SetDescriptorHeaps(u32 Count, ID3D12DescriptorHeap* const * Heaps)
{
	CommandListLocks[CommandListID].lock();
	List->SetDescriptorHeaps(1, Heaps);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::UpdateSubresource(ID3D12Resource* Resource, ID3D12Resource* UploadBuffer,u32 UploadBufferOffset, u32 Index,D3D12_SUBRESOURCE_DATA* src)
{
	CommandListLocks[CommandListID].lock();
	//printf("Update Subresource\n");
	UpdateSubresources(List, Resource, UploadBuffer, UploadBufferOffset, Index, 1, src);
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::UndoScissorRect()
{
	CommandListLocks[CommandListID].lock();
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::CloseCommandList()
{
	CommandListLocks[CommandListID].lock();
	List->Close();
	IsClosed = true;
	CommandListLocks[CommandListID].unlock();
}
void DX12CommandQueue::Dispatch()
{
	CommandListLocks[CommandListID].lock();
	List->Dispatch(100, 100, 1);
	CommandListLocks[CommandListID].unlock();
}