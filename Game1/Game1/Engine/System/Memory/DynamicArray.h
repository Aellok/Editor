#pragma once
#include "System\Types.h"
#define DYNAMIC_ARR_GET_CAST_DATA(DataType,Array) ((DataType*)((Array).data))
//Used when storing strings in the arrays.
struct DataString
{
	char Buffer[32];
};


struct DynamicArray
{
	u32 maxSize;
	u32 maxCount;
	u32 elementSize;
	u32 elementCount;
	char* data;

	void Init(u32 InitialCount,u32 ElementSize);
	void Insert(u32 Index,void* Element);
	void InsertArray(u32 Index, void* Element, u32 Count);
	void Add(void* Element);
	void PartialAdd(void* Element,u32 Size);
	void Copy(void* Data, u32 Size);
	void Replace(u32 Index, void* Element);
	void PartialReplace(u32 Index, void* Element, u32 ElementSize);
	void Delete(u32 Index);
	void Clear();
	void ResizeBuffer(u32 Size);
	void Free();
};