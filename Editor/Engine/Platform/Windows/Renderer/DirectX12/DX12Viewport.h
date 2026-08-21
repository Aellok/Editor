#pragma once
#include "DX12RenderTarget.h"
struct DX12Viewport
{
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	Vector Pos;
	Vector Dim;
	void Init(u32 x, u32 y, u32 Width, u32 Height);
	
	void Resize(u32 Width, u32 Height);
	void SetViewport(DX12CommandQueue* Queue);
};