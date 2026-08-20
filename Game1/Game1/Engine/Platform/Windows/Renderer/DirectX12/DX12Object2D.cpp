#include "DirectX.h"
#include "DX12Object2D.h"
#include "Application\Engine.h"
void DX12Object2D::Init(DirectX12* Context,MemoryArena* Arena, DX12VertexBufferDesc VDesc, DX12IndexBufferDesc IDesc, InstanceDataDesc IDDesc)
{
	DX12 = Context;

	VertexBuffer.Initialize(DX12->device, &DX12->MainCommandQueue, VDesc);
	IndexBuffer.Initialize(DX12->device, &DX12->MainCommandQueue, IDesc);

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

		InstancedVB.Initialize(DX12->device, &DX12->MainCommandQueue, desc);

	}
	
	
}

int DX12Object2D::AddInstancedObject(Matrix Model)
{
	//InstanceModelMatrix[InstanceCount++].model = Model;
	return InstanceCount - 1;
}
void DX12Object2D::UpdateInstancedObject(int ID, Matrix Model)
{
	//InstanceModelMatrix[ID].model = Model;
}
void DX12Object2D::DeleteInstancedObject(int ID)
{
	//memcpy(InstanceModelMatrix + ID, InstanceModelMatrix + ID + 1, (InstanceCount - ID) * sizeof(InstanceBuffer2D));
}

void DX12Object2D::Draw(DX12CommandQueue* Queue,DX12Pipeline* Pipeline,Vector color)
{

	DX12VertexBuffer* VertBuffer = &VertexBuffer;
	Queue->SetVertexBuffer(&VertBuffer,0,1);
	Queue->SetIndexBuffer(&IndexBuffer);
	
	Object2DPerFrameBuffer buffer;
	buffer.Model = DirectX::XMMatrixTranspose(Model);
	buffer.Color = color;
	
	Pipeline->UpdateVSPerFrame(Queue,&buffer);

	Queue->DrawIndexedInstanced(6, 1, 0, 0, 0);

}
//REFACTOR PipelineIndex = "InstanceOrtho"
void DX12Object2D::DrawInstanced(DX12CommandQueue* Queue,void* Buffer,u32 InstanceNum,u32 PipelineIndex)
{
	InstancedVB.Update(&DX12->MainCommandQueue, Buffer);
	
	//Draw all the Static Instances;

	DX12VertexBuffer* buffers[2] = { &VertexBuffer,&InstancedVB };
	char* ShaderName = DX12->PipelineManager->CurrentPipeline->PipelineName;
	//DX12->PipelineManager->SetPipeline(Queue, DX12->frameIndex, PipelineIndex,false);

	DX12->MainCommandQueue.SetVertexBuffer(buffers,0, 2 );
	DX12->MainCommandQueue.SetIndexBuffer(&IndexBuffer);

	Object2DPerFrameBuffer buffer;
	buffer.Model = DirectX::XMMatrixTranspose(Model);
	buffer.Color = { 0.0f,0.0f,0.0f,0.0f };

	DX12->PipelineManager->GetPipeline2D(PipelineIndex)->UpdateVSPerFrame(Queue,&buffer);

	DX12->MainCommandQueue.DrawInstanced(6, InstanceNum, 0, 0);
}