#include "DX12Object3D.h"
#include "VertexDef.h"
#include "Application/Engine.h"
void DX12Object3D::Init(DirectX12* Context, MemoryArena* Arena, DX12VertexBufferDesc VDesc, DX12IndexBufferDesc IDesc, Instance3DDataDesc IDDesc)
{
	DX12 = Context;

	if (DX12->LoadCommandQueue.IsClosed)
	{
		DX12->LoadCommandQueue.Reset(nullptr);
	}

	VertexBuffer.Initialize(DX12->device, &DX12->LoadCommandQueue, VDesc);
	IndexBuffer.Initialize(DX12->device, &DX12->LoadCommandQueue, IDesc);

	VertList = Arena->Allocate(VDesc.Size);
	memcpy(VertList, VDesc.Data, VDesc.Size);
	VertCount = VDesc.Count;

	if (IDDesc.InstanceData && IDDesc.InstanceElementSize)
	{
		InstanceDataElementSize = IDDesc.InstanceElementSize;
		InstanceData = Arena->Allocate(IDDesc.InstanceDataSize);

		DX12VertexBufferDesc desc;
		desc.Data = InstanceData;
		desc.Size = IDDesc.InstanceDataSize;
		desc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(desc.Size);
		desc.Stride = IDDesc.InstanceElementSize;

		InstancedVB.Initialize(DX12->device, &DX12->LoadCommandQueue, desc);

	}
	DX12->LoadCommandQueue.ExecuteAndWait();
}

void DX12Object3D::Draw(DX12CommandQueue* Queue, DX12Pipeline* Pipeline)
{
	DX12VertexBuffer* VertBuffer = &VertexBuffer;
	Queue->SetVertexBuffer(&VertBuffer, 0, 1);
	Queue->SetIndexBuffer(&IndexBuffer);

	DefaultPerFrameBuffer buffer;
	buffer.Model = DirectX::XMMatrixTranspose(Model);
	
	Pipeline->UpdateVSPerFrame(Queue, &buffer);

	Queue->DrawIndexedInstanced(VertCount, 1, 0, 0, 0);
}
void DX12Object3D::DrawInstanced(DX12CommandQueue* Queue, void* Buffer, u32 InstanceNum, u32 PipelineIndex)
{
	InstancedVB.Update(&DX12->MainCommandQueue, Buffer);

	//Draw all the Static Instances;

	DX12VertexBuffer* buffers[2] = { &VertexBuffer,&InstancedVB };

	DX12->MainCommandQueue.SetVertexBuffer(buffers, 0, 2);
	DX12->MainCommandQueue.SetIndexBuffer(&IndexBuffer);

	Object2DPerFrameBuffer buffer;
	buffer.Model = DirectX::XMMatrixTranspose(Model);
	buffer.Color = { 0.0f,0.0f,0.0f,0.0f };

	DX12->PipelineManager->GetPipeline(PipelineIndex)->UpdateVSPerFrame(Queue, &buffer);

	DX12->MainCommandQueue.DrawInstanced(6, InstanceNum, 0, 0);
}