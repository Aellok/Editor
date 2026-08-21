#pragma once
#include "System/Memory/DynamicArray.h"


struct ParsedCBV
{
	u32 Count;	
	u32 Size[8];
	u32 Slot[8];

	//u32
	DynamicArray DataTypesIndex[8];
	// DataString
	DynamicArray DataTypesString[8];
	// DataString
	DynamicArray DataVariableNames[8];
};
struct ParsedVSInput
{
	//u32
	DynamicArray DataTypesIndex;
	// DataString
	DynamicArray DataTypesString;
	// DataString
	DynamicArray DataVariableNames;
	//DataString
	DynamicArray DataSematicsString;
	//u32
	DynamicArray SemanticFlags;
};
struct ParsedVSOutput
{
	//u32
	DynamicArray DataTypesIndex;
	// DataString
	DynamicArray DataTypesString; 
	// DataString
	DynamicArray DataVariableNames; 
	//DataString
	DynamicArray DataSematicsString;
	//u32
	DynamicArray SemanticFlags;
};

struct VSParserResult
{
	ParsedCBV cbvs;
	ParsedVSInput vsInput;
	ParsedVSOutput vsOutput;
	bool IsValid;
};

struct ParsedRegisters
{
	//u32 
	DynamicArray Types;
	//u32 
	DynamicArray Slots;
	//DataString 
	DynamicArray DataTypes;
	//DataString 
	DynamicArray VarNames;
};
struct ParsedPSInput
{
	//u32
	DynamicArray DataTypesIndex;
	// DataString
	DynamicArray DataTypesString;
	// DataString
	DynamicArray DataVariableNames;
	//DataString
	DynamicArray DataSematicsString;
	//u32
	DynamicArray SemanticFlags;
};
struct PSParserResult
{
	//ParsedRegister
	ParsedRegisters registers;
	ParsedPSInput psInput;
	bool IsValid;
};
