#include "DX12Viewport.h"

void DX12Viewport::Init(u32 x, u32 y,u32 Width, u32 Height)
{
	Pos = { (f32)x,(f32)y };
	Dim = { (f32)Width,(f32)Height };

	viewport.Width = Width;
	viewport.Height = Height;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.MinDepth = D3D12_MIN_DEPTH;
	viewport.MaxDepth = D3D12_MAX_DEPTH;

	scissorRect.left = x;
	scissorRect.right = Width;
	scissorRect.top =  y;
	scissorRect.bottom = Height;
}

void DX12Viewport::Resize(u32 Width, u32 Height)
{
	Dim = { (f32)Width,(f32)Height };
	viewport.Width = Width;
	viewport.Height = Height;

	scissorRect.right = Width;
	scissorRect.bottom = Height;
}

void DX12Viewport::SetViewport(DX12CommandQueue* Queue)
{
	Queue->SetViewport(&viewport);
	Queue->SetScissorRect(&scissorRect);
}