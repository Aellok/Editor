#pragma once
#include "System/Types.h"

enum PropertyTypes
{
	eSelector,
	eTextBox
};

struct OnPropertyAddedParams
{
	void* Parent;
	u32 PropertyType;
	u32 SubType;
	u32 NumberOfProperties;
};

typedef void (*OnPropertyAddedCallback)(OnPropertyAddedParams);

struct ObjectChangeInfo
{
	void* Parent;
	u32 ShaderSize[5];
	const char* ShaderFileData[5];
	const char* PipelineName;
};

typedef void(*ObjectChanged)(ObjectChangeInfo);

