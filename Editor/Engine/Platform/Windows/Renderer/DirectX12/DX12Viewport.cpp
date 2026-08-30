#include "DX12Viewport.h"

void DX12Viewport::Init(f32 x, f32 y,f32 Width, f32 Height)
{
	Pos = { x,y };
	Dim = { Width,Height };

	viewport.Width = Width;
	viewport.Height = Height;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.MinDepth = D3D12_MIN_DEPTH;
	viewport.MaxDepth = D3D12_MAX_DEPTH;

	scissorRect.left = (s32)x;
	scissorRect.right = (s32)Width;
	scissorRect.top = (s32)y;
	scissorRect.bottom = (s32)Height;
}

void DX12Viewport::Resize(f32 Width, f32 Height)
{
	Dim = { Width,Height };
	viewport.Width = Width;
	viewport.Height = Height;

	scissorRect.right = (s32)Width;
	scissorRect.bottom = (s32)Height;
}

void DX12Viewport::SetViewport(DX12CommandQueue* Queue)
{
	Queue->SetViewport(&viewport);
	Queue->SetScissorRect(&scissorRect);
}