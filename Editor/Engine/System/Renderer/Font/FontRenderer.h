#pragma once
#include "System/Types.h"
#include "Platform/Windows/Renderer/DirectX12/DX12Texture.h"
#include "Platform/Windows/Renderer/DirectX12/DirectX.h"
#include "System/Renderer/Object2D.h"
#include "System\Memory\DynamicArray.h"
struct Glyph
{
	DX12Texture Texture;
	s32 BitmapWidth;
	s32 BitmapHeight;
	s32 XOffset;
	s32 YOffset;
	s32 advanceWidth;
	s32 leftSideBearing;
	f32 scale;
	bool Renderable;
};

struct StringLineInfo
{
	u32 CharCount;
	f32 NewLineYPosition; 
	DynamicArray Contents; //char
	DynamicArray Positions; // Vector
	DynamicArray CharWidths; // f32
	void Create(MemoryArena* Arena, u32 MaxCharCount);
	char GetChar(u32 Index) { return ((char*)Contents.data)[Index]; };
	Vector GetPosition(u32 Index) { return ((Vector*)Positions.data)[Index]; };
	f32 GetWidth(u32 Index) { return ((f32*)CharWidths.data)[Index]; };
};
struct StringInfo
{
	f32 PixelLength;
	f32 PixelHeight;
	f32 YPadding;
	u32 LineCount;
	StringLineInfo* LineInfo;
	void Initialize(u32 MaxLines);
	void Clear();
};

struct Font
{
	DirectX12* DX12;
	u32 size;

	DX12Object2D Mesh;
	Glyph Glyphs[255];
	
	f32 Scale[255];
	//char dependant.
	s32 lsb[255]; // left side bearing. xoff from the baseline.
	s32 advanceWidth[255];
	s32 x0[255];
	s32 x1[255];
	s32 y0[255];
	s32 y1[255];
	f32 YOffsets[255];
	s32 LineGap;
	s32 ascent; // Upper bound
	s32 decent; // Lower bound
	
	DX12Texture TextureArray;
	DX12Texture Texture;
	void Init(void* pPlatformInterface, MemoryArena* Arena);
	void LoadFont(DirectX12* DX12,const char* FileName, u32 Size);
	s32 GetFontStringLength(const char* String);
	void GetStringPositions(char* String, Vector Start, u32 Size, StringInfo* OutInfo,u32 StartLineOffset);
	f32 GetScale(u32 FontSize);
	f32 GetCursorSize(u32 FontSize);
	u8 GetAsciiFromVK(u64 VK_Code);
};