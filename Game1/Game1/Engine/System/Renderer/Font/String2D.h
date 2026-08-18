#pragma once
#include "Platform/Windows/Renderer/DirectX12/DX12String2D.h"
#include "System\Renderer\Font\FontRenderer.h"
#define MAX_SRO_LENGTH 255
struct String2D
{
	Vector Pos;
	u32 Index;
	u32 PipelineIndex;
	bool Visible;
	u32 StringIndex;
	u32 PixelLength;
	u32 PixelHeight;
	f32 YPadding;
	u32 TextSize;
	StringInfo Info;

	void Initialize(DirectX12* DX12, MemoryArena* Arena, const char* String);
	void Draw();

};