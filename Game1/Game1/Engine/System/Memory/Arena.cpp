#include "Arena.h"
#include <memory>
void MemoryArena::Create(u32 InSize)
{
	TotalSize = InSize;
	Data = (u8*)calloc(1, TotalSize);
	writePtr = Data;
}
void* MemoryArena::Allocate(u32 Size)
{
	if (writePtr + Size > Data + TotalSize)
	{
		printf("Ran out of memory in arena.\n");
		return NULL;
	}
	u8* ptr = writePtr; 
	writePtr += Size;
	return ptr;
}
void MemoryArena::Free()
{
	TotalSize = 0;
	free(Data);
	Data = 0;
	writePtr = 0;
}