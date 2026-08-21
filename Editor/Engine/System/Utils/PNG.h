#pragma once
struct PNG
{
	void* Data;
	int PixelDataBitLength;
	int Width;
	int Height;
	bool HasAlpha;
};
PNG LoadPNG(const char* FileName);