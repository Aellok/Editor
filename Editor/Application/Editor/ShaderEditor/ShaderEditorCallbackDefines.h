#pragma once
struct OnRegisterChangedParams
{
	void* Parent;
	u32 RegisterCount;
	//DataString
	DynamicArray RegisterNames;
	//u32
	DynamicArray Slots;
	bool reset;
};

typedef void (*OnRegisterChanged)(OnRegisterChangedParams);

