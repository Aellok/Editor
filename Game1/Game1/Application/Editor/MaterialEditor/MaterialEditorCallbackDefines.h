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

