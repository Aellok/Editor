#pragma once
#include "System/Types.h"
struct UPnPEntry
{
	wchar_t InternalIp[128];
	wchar_t ExternalIp[128];
	wchar_t Desc[128];
	wchar_t Protocol[128];
	u32 Port;
};
struct UPnP
{
	u32 EntryCount;
	UPnPEntry Entries[8];
	void Init();
	bool AddEntry(const char* InternalIp, const char* ExternalIp, const char* Desc, const char* Protocol, long Port);
	bool Remove(UPnPEntry Entry);
	void PrintMappings();
};