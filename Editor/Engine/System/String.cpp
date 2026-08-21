#include "String.h"

s32 GetStringIndex(const char** StrList, u32 StrCount, const char* TargetString)
{
	if (TargetString)
	{
		for (u32 i = 0; i < StrCount; i++)
		{
			if (StrList[i] && !strcmp(StrList[i], TargetString))
			{
				return i;
			}
		}
	}
	
	return -1;
}
wchar_t* CharToWChar(s8* String)
{
	u32 StringLength = strlen(String) + 1;
	wchar_t* Result = (wchar_t*)calloc(StringLength, sizeof(wchar_t));
	u64 BytesConverted = 0;

	mbstowcs_s(&BytesConverted, Result, StringLength, String, StringLength);

	return Result;
}
char* WCharToChar(wchar_t* String)
{
	u32 Length = wcslen(String);
	char* Buffer = (char*)calloc(1,Length + 1);
	wcstombs(Buffer,String,Length);
	return Buffer;
}
u32 GetNewLineCount(char* String)
{
	u32 Count = 0;
	char* index = String;
	while (*index != '\0')
	{
		Count += *(index++) == '\n';
	}
	return Count;
}