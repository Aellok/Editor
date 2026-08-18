#pragma once
#include "Types.h"
#include <string>
#pragma warning(disable : 4996)
#define STR_TO_WSTR(str,wstr) swprintf((wstr),L"%hs",(str))

wchar_t* CharToWChar(s8* String);
char* WCharToChar(wchar_t* String);
s32 GetStringIndex(const char** StrList, u32 StrCount, const char* TargetString);

u32 GetNewLineCount(char* String);