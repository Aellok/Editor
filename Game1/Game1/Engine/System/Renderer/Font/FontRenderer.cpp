#include "FontRenderer.h"
#include "../../Utils/File.h"
#include "../../../Platform/Windows/Renderer/DirectX12/VertexDef.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../../../ThirdParty/stb_truetype.h"

#include "System\String.h"
#include "Application\Engine.h"

stbtt_fontinfo font;

f32 KerningPairs[65536];
#define GET_KERNING_PAIR(lc,rc) KerningPairs[((s32)(lc) * 255) + (s32)(rc)]
u8* AddAlphaChannel(u8* Data,u32 Width,u32 Height)
{
	u8* Result = (u8*)calloc(Width * Height,4);
	u8* Source = Data;
	u8* DestRow = Result;
	for (u32 y = 0; y < Height;y++)
	{
		u32* Dest = (u32*)DestRow;
		for (u32 x = 0; x < Width; x++)
		{
			u8 Alpha = *Source++;
			*Dest++ = ((Alpha << 24) | (Alpha << 16) | (Alpha << 8) | Alpha);
		}
		DestRow += (4 * Width);
	}
	return Result;
}
void Font::Init(void* pPlatformInterface,MemoryArena* Arena)
{
	
	Vertex2D VertexList[6] =
	{
		{{0,0,0,1},{0,0}},
		{{1,0,0,1},{1,0}},
		{{0,1,0,1},{0,1}},
		{{0,1,0,1},{0,1}},
		{{1,0,0,1},{1,0}},
		{{1,1,0,1},{1,1}}
	};
	u32 IndexList[6] = { 0,1,2,3,4,5 };

	DX12VertexBufferDesc VDesc = { 6, VertexList,6 * sizeof(Vertex2D),sizeof(Vertex2D) };
	VDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(VDesc.Size);
	DX12IndexBufferDesc IDesc = { 6, IndexList, 6 * sizeof(u32) };
	IDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(IDesc.Size);
	InstanceDataDesc IDDesc;
	IDDesc.InstanceData = nullptr;
	IDDesc.InstanceDataSize = 0;
	IDDesc.InstanceElementSize = 0;
	Mesh.Init((DirectX12*)pPlatformInterface,Arena,VDesc,IDesc,IDDesc);
}

void Font::LoadFont(DirectX12* pDX12,const char* FileName,u32 Size)
{
	DX12 = pDX12;
	size = Size;
	u32 FontFileSize;
	File file;
	file.Load(FileName);

	bool b = stbtt_InitFont(&font, (u8*)file.Data, 0);
	if (!b)
	{
		return;
	}
	for (u32 i = 0; i < 255;i++)
	{
		Scale[i] = stbtt_ScaleForPixelHeight(&font, i);
		stbtt_GetCodepointHMetrics(&font, i, &advanceWidth[i], &lsb[i]);
		stbtt_GetCodepointBox(&font, i, &x0[i], &y0[i], &x1[i], &y1[i]);
	}
	
	
	float MaxWidth = 0;
	float MaxHeight = 0;
	MemoryArena Arena;
	Arena.Create((256 * Size * Size * 4) + 1);
	u32 TextureCount = 0;
	u8* UniformTexture = nullptr;
	u32 TotalWritten = 0;
	u32 c = 0;
	for (u32 i = 0; i < 255; i++)
	{
		char TextureName[8] = { 'F','o','n','t',i };
		s32 x = 0;
		s32 y = 0;
		u8* DataMono = stbtt_GetCodepointBitmap(&font, Scale[Size], Scale[Size], i, &Glyphs[i].BitmapWidth, &Glyphs[i].BitmapHeight, &Glyphs[i].XOffset, &Glyphs[i].YOffset);
		UniformTexture = (u8*)Arena.Allocate(Size * Size * 4);
		TextureCount++;

		Glyphs[i].Renderable = DataMono;
		if (DataMono)
		{
			u8* Data = AddAlphaChannel(DataMono, Glyphs[i].BitmapWidth, Glyphs[i].BitmapHeight);
			
			for (u32 SrcRow = 0; SrcRow < Glyphs[i].BitmapHeight; SrcRow++)
			{
				u32 DestRow = SrcRow * Size * 4 ;
				c += (Glyphs[i].BitmapWidth * 4);
				memcpy(&UniformTexture[DestRow], &Data[SrcRow * Glyphs[i].BitmapWidth * 4], (Glyphs[i].BitmapWidth * 4));
				TotalWritten += (Glyphs[i].BitmapWidth * 4);
			}
			
			//Glyphs[i].Texture.InitFromData(&DX12->MainCommandQueue,DX12, TextureName, Data, (u32)Glyphs[i].BitmapWidth, (u32)Glyphs[i].BitmapHeight, DXGI_FORMAT_R8G8B8A8_UNORM,true);

			stbtt_GetFontVMetrics(&font, &ascent, &decent, &LineGap);

			MaxWidth = MaxWidth < Glyphs[i].BitmapWidth ? Glyphs[i].BitmapWidth : MaxWidth;
			MaxHeight = MaxHeight < Glyphs[i].BitmapHeight ? Glyphs[i].BitmapHeight : MaxHeight;

			stbtt_FreeBitmap(DataMono, 0);
			free(Data);
		}
	}
	for (s32 i = 0; i < 255; i++)
	{
		for (s32 j = 0; j < 255; j++)
		{
			KerningPairs[i * 255 + j] = stbtt_GetCodepointKernAdvance(&font, i, j);
		}
	}

	TextureArray.CreateTextureArray(&DX12->MainCommandQueue, Arena.Data, TextureCount, Size, Size);
	
	Arena.Free();
	
	file.Close();
}

void Font::GetStringPositions(char* String,Vector Start,u32 Size, StringInfo* OutInfo)
{
	//1. Split the string into substrings.
	u32 NLCount = GetNewLineCount(String) + 1;
	OutInfo->LineCount = 0;
	
	if (NLCount > 0)
	{
		char* LastNewLine = String;	
		for (char* NewLine = strchr(String, '\n'); NewLine != NULL; NewLine = strchr(NewLine + 1, '\n') )
		{
			u32 LineLength = NewLine - LastNewLine;
			OutInfo->LineInfo[OutInfo->LineCount++].Contents.Copy( LastNewLine, LineLength );
			LastNewLine = NewLine + 1;
		}
		OutInfo->LineInfo[OutInfo->LineCount++].Contents.Copy( LastNewLine, strlen(LastNewLine));
	}
	//this is the highest point of the font.
	s32 ScaledA = (ascent * GetScale(Size));
	
	f32 BaseLineY = Start.m128_f32[1] + ScaledA; // set the baseline so that the BaselineY - Ascent = StartY.
	
	for (u32 LineIndex = 0; LineIndex < NLCount; LineIndex++)
	{
		StringLineInfo* Info = &OutInfo->LineInfo[LineIndex];
		Info->CharCount = 0;
		f32 BaseLineX = Start.m128_f32[0];
		
		for (char* c = Info->Contents.data; *c != '\0'; c++)
		{
			f32 Mult = 1;
			if (*c == '\t')
			{
				*c = ' ';
				Mult = 5;
			}

			//BaseLineX + leftBearing + any kearning = position x
			//BaseLineY - y1 = top of the character 
			f32 Kerning = GET_KERNING_PAIR(Info->CharCount > 0 ? *(c - 1) : *c, *c) * GetScale(Size);

			Vector Vec = { (float)(BaseLineX + Glyphs[*c].XOffset + Kerning) , (float)(BaseLineY + Glyphs[*c].YOffset)};
			Info->Positions.Add(&Vec);
			
			f32 Width = advanceWidth[*c] * GetScale(Size) * Mult;
			BaseLineX += Width + (Kerning * GetScale(Size));

			Info->CharWidths.Add(&Width);
			Info->CharCount++;
		}
		Info->NewLineYPosition = BaseLineY;
		//LineWidth + LineGap
		BaseLineY += (ascent - decent + LineGap) * GetScale(Size);
		OutInfo->PixelLength = BaseLineX - Start.m128_f32[0];
	}
	OutInfo->PixelHeight = (ascent - decent) * GetScale(Size);
	OutInfo->YPadding = ascent * GetScale(Size);
}
u32 Font::GetFontStringLength(const char* String)
{
	f32 OriginX = 0;
	f32 OriginY = ascent;

	for (u32 i = 0; i < strlen(String); i++)
	{
		Glyph* g = &Glyphs[String[i]];
		if (String[i] == ' ')
		{
			OriginX += advanceWidth[String[i]];
			continue;
		}
		OriginX += advanceWidth[String[i]];
	}
	return OriginX;
}

f32 Font::GetScale(u32 FontSize)
{
	return Scale[FontSize];
}
f32 Font::GetCursorSize(f32 FontSize)
{
	return (ascent - decent) * GetScale(FontSize);
}


void StringLineInfo::Create(MemoryArena* Arena, u32 MaxCharCount)
{
	Positions.Init(  MaxCharCount, sizeof(Vector));
	Contents.Init(   MaxCharCount, sizeof(char));
	CharWidths.Init( MaxCharCount, sizeof(f32));
}

void StringInfo::Initialize(u32 MaxLines)
{
	LineInfo = (StringLineInfo*)GEngine.Global.Allocate(sizeof(StringLineInfo) * MaxLines);
	for (u32 i = 0; i < MaxLines; i++)
	{
		LineInfo[i].Create(&GEngine.Global, 255);
	}
}
void StringInfo::Clear()
{
	for (u32 i = 0; i < LineCount; i++)
	{
		LineInfo[i].CharCount = 0;
		LineInfo[i].NewLineYPosition = 0;
		LineInfo[i].CharWidths.Clear();
		LineInfo[i].Contents.Clear();
		LineInfo[i].Positions.Clear();
	}
}


u8 Font::GetAsciiFromVK(u32 VK_Code)
{
	u16 c;
	BYTE ks[256];
	GetKeyboardState(ks);
	KeyboardManager* Manager = &GEngine.pWindow->keyboardManager;
	ToAscii(VK_Code, MapVirtualKey(VK_Code, MAPVK_VK_TO_VSC) , ks, &c, 0);
	return c;
}