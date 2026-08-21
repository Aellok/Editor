#pragma once
struct OnRegisterChangedParams
{
	void* Parent;
	u32 RegisterCount;
	//DataString
	DynamicArray RegisterNames;
};

typedef void (*OnRegisterChanged)(OnRegisterChangedParams);

