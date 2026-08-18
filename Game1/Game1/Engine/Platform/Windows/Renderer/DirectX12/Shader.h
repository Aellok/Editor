#pragma once
#include "../../../../System/Types.h"
#include "ShaderBuffers.h"
#include "DX12.h"
#include "../../../../System/Utils/File.h"
#define CONSTANT_BUFFER_COUNT 2
#define CONSTANT_BUFFER_SIZE  1024 * 1024 


struct VertexShader
{
	const char* name;
	File file;
	ID3D12Device* device;

	ID3D12RootSignature* rootSig;
	ID3D12Resource* constBufferUploadHeap[CONSTANT_BUFFER_COUNT]; // NOTE: if there is more than 2 frames this need to increase;
	u8* constBufferAddress[CONSTANT_BUFFER_COUNT];
	
	u32 cbOffset;

	u32 FrameIndex = 0;

	u32 cbPerFrameBufferSize = 0;
	u32 cbOnResizeBufferSize = 0;
	u8 Buffer[12400];
	void Create(ID3D12Device* Device, void* CBPerFrameData, u32 CBPerFrameSize, void* CBOnResizeData, u32 CBOnResizeSize, const char* Name, const char* Path);
	void UpdatePerFrame(void* Data);
	void UpdateOnResize(void* Data);
	//void UpdateView(Matrix View);
	void UpdateProj(Matrix Proj);
	void UpdateCamPos(Vector pos);
	void SetShader();
};

struct PixelShader
{
	const s8* Name;
	File file;

	void Create(const char* Name, const char* Path);
	void Update();
	void SetShader();
};