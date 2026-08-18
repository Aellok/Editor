#pragma once
#include "System/Types.h"
#include "System/Memory/DynamicArray.h"
enum LexerTokenTypes
{
	eBeginEnd			= 0x1,
	eDefine				= 0x2,
	eType				= 0x4,
	eSlot				= 0x8,
	eSemantic			= 0x10,
	eSemanticSeperator	= 0x20,
	eName				= 0x40
};
enum LexerTypes
{
	eCBuffer,
	eMATRIX,
	eFLOAT4X4,
	eFLOAT4,
	eFLOAT3,
	eFLOAT2,
	eFLOAT,
	eTEXTURE2D,
	eTEXTURE2DARRAY,
	eSAMPLERSTATE,
	eTypeCount
};
extern u32 TypeSizes[eTypeCount];
extern const char* TypeStrings[eTypeCount];
enum LexerDefines
{
	eNone,
	eCBV,
	eINST_INPUT,
	eVS_INPUT,
	eVS_OUTPUT,
	ePS_INPUT,
	//PS Register Defines. (these have to stay together)
	eSAMPLER,
	eTEXTURE,
	eUAV,
	//End PS Register defines
	eDefineCount
};
extern const char* DefineStrings[eDefineCount];
enum LexerSemantics
{
	ePOSITION,
	eSV_POSITION,
	eTEXCOORD,
	eCOLOR,
	eNORMAL,
	eINSTANCE_TRANSFORM,
	eINSTANCE_INDEX,
	eINSTANCE_UV,
	eINSTANCE_COLOR,
	eSemanticCount
};

extern const char* SemanticStrings[eSemanticCount];

struct Token
{
	LexerTokenTypes Type;
	u32 Subtype;

	char Buffer[32]; // Stores the word
};

struct LexerResult
{
	DynamicArray tokens; //Token
};

s32 IsDefine(char* Word);
s32 IsBeginEnd(char* Word);
s32 IsType(char* Word);
s32 IsSemantic(char* Word);
s32 IsSemanticSeperator(char* Word);
s32 IsSlot(char* Word);
u32 IsInst(char* Word);

u32 GetSemanticSlot(char* Semantic);