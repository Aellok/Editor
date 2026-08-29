#pragma once
#include "System\Types.h"

struct DialogInfo
{
	void* data;
	u32 Size;
	char FilePath[260];
};
const char* WinDialog_SaveSelector();
bool WinDialog_Save(void* FileData, u32 FileSize);
DialogInfo WinDialog_Load();