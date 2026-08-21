#pragma once
#include "System\Types.h"
#include "DX12Object2D.h"
#include "DX12Texture.h"
#include "System\Temp.h"
#include "System\Memory\DynamicArray.h"
struct DX12String2D
{
	u32 Length;
	DynamicArray data;

	DX12Object2D Mesh;
	DX12Texture* Texture;
	DX12InstanceVertexBuffer IVertexBuffer;

	DynamicArray InstanceBufferData;
	

	void Init(const s8* str, u32 InstanceElementSize);
	void Update();
	void Draw(DX12CommandQueue* Queue,DX12Pipeline* FontPipeline);
};