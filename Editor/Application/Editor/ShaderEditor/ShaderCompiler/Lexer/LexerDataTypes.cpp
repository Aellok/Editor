#include "LexerDataTypes.h"
#include <string>

u32 TypeSizes[eTypeCount]
{
	0,64,64,16,12,8,4
};
const char* TypeStrings[eTypeCount]
{
	"cbuffer",
	"matrix",
	"float4x4",
	"float4",
	"float3",
	"float2",
	"float",
	"Texture2D",
	"Texture2DArray",
	"SamplerState"
};
const char* DefineStrings[eDefineCount]
{
	"None",
	"CBV",
	"INST_INPUT",
	"VS_INPUT",
	"VS_OUTPUT",
	"PS_INPUT",
	"SAMPLER",
	"TEXTURE",
	"UAV"
};

const char* SemanticStrings[eSemanticCount]
{
	"POSITION",
	"SV_POSITION",
	"TEXCOORD",
	"COLOR",
	"NORMAL",
	"INSTANCE_TRANSFORM",
	"INSTANCE_INDEX",
	"INSTANCE_UV",
	"INSTANCE_COLOR"
};

s32 IsDefine(char* Word)
{
	for (u32 i = 0; i < eDefineCount; i++)
	{
		u32 length = min(strlen(Word), strlen(DefineStrings[i]));
		if (!strcmp(Word, DefineStrings[i]))
		{
			return i;
		}
	}
	return -1;
}
s32 IsBeginEnd(char* Word)
{
	return (Word[0] == '{') ? 0 : (Word[0] == '}' ? 1 : -1);
}
s32 IsType(char* Word)
{
	for (u32 i = 0; i < eTypeCount; i++)
	{
		if (!strcmp(Word, TypeStrings[i]))
		{
			return i;
		}
	}
	return -1;
}
s32 IsSemanticSeperator(char* Word)
{
	return Word[0] == ':' ? 1 : -1;
}
s32 IsSemantic(char* Word)
{
	for (u32 i = 0; i < eSemanticCount; i++)
	{
		u32 Length = min(strlen(Word), strlen(SemanticStrings[i]));
		if (!strcmp(Word, SemanticStrings[i]))
		{
			return i;
		}
	}
	return -1;
}
s32 IsSlot(char* Word)
{
	u32 MinLength = strlen("register");
	if (strlen(Word) <= MinLength)
	{
		return -1;
	}
	return !memcmp(Word, "register", MinLength) ? eSlot : -1;
}
u32 IsInst(char* Word)
{
	u32 MinLength = strlen("INSTANCE_");
	if (strlen(Word) < MinLength)
	{
		return 0;
	}
	return !memcmp(Word, "register", MinLength);
}
u32 GetSemanticSlot(char* Semantic)
{
	char* SlotStart = Semantic + strlen(Semantic) - 1;
	if (!(*SlotStart >= '0' && *SlotStart <= '9')) // there isnt a slot specified.
	{
		return 0;
	}
	for (char* c = SlotStart; *c >= '0' && *c <= '9' && SlotStart != Semantic; c--)
	{
		SlotStart = c;
	}
	u32 Slot = (u32)atoi(SlotStart);
	*SlotStart = 0;
	return Slot;
}
