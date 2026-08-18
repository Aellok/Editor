#include "DX12String2D.h"
#include "Application\Engine.h"

void DX12String2D::Init(const s8* str,u32 InstanceElementSize)
{
	data.Init(255, 1);
	data.Copy((void*)str, strlen(str));
	Length = (strlen(str) < 255 ? 255 : strlen(str));

	InstanceBufferData.Init(Length, InstanceElementSize);

	FontVertex2D VertexList[6] =
	{
		{{0,0,0,1},{0,0}},
		{{1,0,0,1},{1,0}},
		{{0,1,0,1},{0,1}},
		{{0,1,0,1},{0,1}},
		{{1,0,0,1},{1,0}},
		{{1,1,0,1},{1,1}}
	};
	u32 IndexList[6] = { 0,1,2,3,4,5 };

	DX12VertexBufferDesc VDesc = { 6, VertexList,6 * sizeof(FontVertex2D),sizeof(FontVertex2D) };
	VDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(VDesc.Size);
	DX12IndexBufferDesc IDesc = { 6, IndexList, 6 * sizeof(u32) };
	IDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(IDesc.Size);

	InstanceDataDesc IDDesc;
	IDDesc.InstanceData = InstanceBufferData.data;
	IDDesc.InstanceDataSize = Length * InstanceElementSize;
	IDDesc.InstanceElementSize = InstanceElementSize;

	//TODO: this doesnt expand when the number of characters change.
	Mesh.Init(GEngine.pRendererInterface, &GEngine.Global, VDesc, IDesc, IDDesc);
	IVertexBuffer.Initialize(InstanceBufferData.data, Length, InstanceElementSize);
}
void DX12String2D::Draw(DX12CommandQueue* Queue,DX12Pipeline* FontPipeline)
{
	DirectX12* DX12 = GEngine.pRendererInterface;
	IVertexBuffer.Update(InstanceBufferData.data,Length);
	//Mesh.InstancedVB.Update(Queue, InstanceBufferData);

	//Draw all the Static Instances;

	DX12VertexBuffer* buffers[2] = { &Mesh.VertexBuffer,&Mesh.InstancedVB };

	Queue->SetVertexBuffer(buffers, 0, 1);
	IVertexBuffer.Bind(Queue, 1);
	Queue->SetIndexBuffer(&Mesh.IndexBuffer);

	Object2DPerFrameBuffer buffer;
	buffer.Model = DirectX::XMMatrixTranspose(Mesh.Model);
	buffer.Color = { 0.0f,0.0f,0.0f,0.0f };
	FontPipeline->UpdateVSPerFrame(Queue,&buffer);

	Queue->DrawInstanced(6, Length, 0, 0);
}