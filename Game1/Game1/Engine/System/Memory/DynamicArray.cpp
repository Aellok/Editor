#include "DynamicArray.h"
#include <memory.h>
#include <stdio.h>
void DynamicArray::ResizeBuffer(u32 Count)
{
	u32 OldMaxSize = maxSize;
	maxSize = Count * elementSize;
	maxCount = Count;
	if (data)
	{
		data = (char*)realloc(data,maxSize);
		memset(data + OldMaxSize, 0, maxSize - OldMaxSize);
	}
	else 
	{
		printf("DynamicArray::ResizeBuffer Error: data isnt initialized\n");
	}
}

void DynamicArray::Init(u32 InitialCount, u32 ElementSize)
{
	maxSize = InitialCount * ElementSize;
	maxCount = InitialCount;
	elementSize = ElementSize;
	elementCount = 0;
	
	data = (char*)calloc(InitialCount,ElementSize);
}
void DynamicArray::Clear()
{
	memset(data, 0, maxSize);
	elementCount = 0;
}

void DynamicArray::Delete(u32 Index)
{
	if (elementCount <= Index)
	{
		printf("Error: DynamicArray::Delete() Index is out of bounds");
	}
	memcpy(data + (Index * elementSize),data + ((Index + 1) * elementSize) ,(elementCount - Index) * elementSize);
	elementCount--;
}
void DynamicArray::Replace(u32 Index,void* Element)
{
	if (Index <= elementCount)
	{
		memcpy(data + (Index * elementSize), Element, elementSize);
		return;
	}
	printf("DynamicArray::Replace Error: Index was outside the bounds of elementCount\n");
}
void DynamicArray::PartialReplace(u32 Index, void* Element,u32 ElementSize)
{
	if (Index <= elementCount)
	{
		memcpy(data + (Index * elementSize), Element, ElementSize);
		return;
	}
	printf("DynamicArray::Replace Error: Index was outside the bounds of elementCount\n");
}
void DynamicArray::Insert(u32 Index, void* Element)
{
	if (elementCount + 1 > maxCount)
	{
		ResizeBuffer(maxCount * 1.5f);
	}

	memcpy(data + ((Index + 1) * elementSize),data + (Index * elementSize),((elementCount - Index) * elementSize));
	memcpy(data + (Index * elementSize), Element, elementSize);
	elementCount++;
}
void DynamicArray::InsertArray(u32 Index, void* Element,u32 Count)
{
	while (elementCount + Count + 1 > maxCount)
	{
		ResizeBuffer(maxCount * 1.5f);
	}

	memcpy(data + ((Index + Count) * elementSize), data + (Index * elementSize), ((elementCount - Index) * elementSize));
	memcpy(data + (Index * elementSize), Element, elementSize * Count);
	elementCount += Count;
}
void DynamicArray::Add(void* Element)
{
	if (elementCount + 1 >= maxCount)
	{
		ResizeBuffer(maxCount * 1.5f);
	}
	Replace(elementCount++,Element);
}
void DynamicArray::PartialAdd(void* Element, u32 Size)
{
	if (elementCount + 1 >= maxCount)
	{
		ResizeBuffer(maxCount * 1.5f);
	}
	PartialReplace(elementCount++, Element,Size);
}
void DynamicArray::Copy(void* Data,u32 Size)
{
	while (Size >= maxSize)
	{
		ResizeBuffer(maxCount * 1.5f);
	}
	elementCount = Size / elementSize;
	memset(data, 0, maxSize);
	memcpy(data, Data, Size);
}
void DynamicArray::Free()
{
	free(data);
	data = NULL;
}