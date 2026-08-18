#pragma once
#include "../Types.h"
struct MemoryArena
{
	u32 TotalSize;
	u8* Data;

	u8* writePtr;

	void Create(u32 InSize);
	void* Allocate(u32 Size);
	void Free();
};