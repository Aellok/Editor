#include "Shader.h"
#include "System/Utils/File.h"



#define ALLIGN_256(size) (((size) + 255) & ~255)
void VertexShader::Create(ID3D12Device* Device, void* CBPerFrameData, u32 CBPerFrameSize,void* CBOnResizeData,u32 CBOnResizeSize, const char* Name, const char* Path)
{
	cbOnResizeBufferSize = CBOnResizeSize;
	cbPerFrameBufferSize = CBPerFrameSize;
	name = Name;
	file.Load(Path);
	device = Device;
	
	for (u32 i = 0; i < CONSTANT_BUFFER_COUNT; i++)
	{
		CD3DX12_HEAP_PROPERTIES props(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(CONSTANT_BUFFER_SIZE);
		device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBufferUploadHeap[i]));

		CD3DX12_RANGE readRange(0, 0);
		constBufferUploadHeap[i]->Map(0, &readRange, (void**)&constBufferAddress[i]);

		memcpy(constBufferAddress[i], CBOnResizeData, cbOnResizeBufferSize);
		memcpy(constBufferAddress[i] + cbOnResizeBufferSize, CBPerFrameData, cbPerFrameBufferSize);
	}
	cbOffset = cbOnResizeBufferSize;
}
//Updates the per frame data of an object;
void VertexShader::UpdatePerFrame(void* Data)
{
	memcpy((u8*)constBufferAddress[FrameIndex] + cbOffset, Data, cbPerFrameBufferSize);
	cbOffset += cbPerFrameBufferSize;
}
void VertexShader::UpdateOnResize(void* Data)
{
	memcpy(Buffer, Data, cbOnResizeBufferSize);
	memcpy((u8*)constBufferAddress[FrameIndex], Data, cbOnResizeBufferSize);
}
void VertexShader::UpdateProj(Matrix Proj)
{
	memcpy((u8*)constBufferAddress[0] + 64, &Proj, sizeof(Matrix)); // TODO: if OnResizeBuffer changes this also needs to change...
	memcpy((u8*)constBufferAddress[1] + 64, &Proj, sizeof(Matrix)); // TODO: if OnResizeBuffer changes this also needs to change...
	//cbOffset += cbPerFrameBufferSize;
}
void VertexShader::UpdateCamPos(Vector pos)
{
	memcpy((u8*)constBufferAddress[FrameIndex] + 128, &pos, sizeof(Vector));
}
//------------------ PIXEL SHADER ---------------------

void PixelShader::Create( const char* ShaderName, const char* Path)
{
	Name = ShaderName;
	file.Load(Path);
}