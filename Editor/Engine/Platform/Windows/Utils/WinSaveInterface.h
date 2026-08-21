#pragma once
#include "System\Types.h"

struct DialogInfo
{
	void* data;
	u32 Size;
	char FilePath[260];
};

bool WinDialog_Save(void* FileData, u32 FileSize);
DialogInfo WinDialog_Load();