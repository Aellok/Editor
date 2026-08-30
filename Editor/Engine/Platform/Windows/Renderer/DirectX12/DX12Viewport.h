#pragma once
#include "DX12RenderTarget.h"
struct DX12Viewport
{
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	Vector Pos;
	Vector Dim;
	void Init(f32 x, f32 y, f32 Width, f32 Height);
	
	void Resize(f32 Width, f32 Height);
	void SetViewport(DX12CommandQueue* Queue);
};