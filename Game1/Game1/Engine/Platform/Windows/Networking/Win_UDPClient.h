#pragma once
#include "../../../System/Types.h"
struct Win_UDPClient
{
	s32 RecvSize;
	char RecvBuffer[65535];

	bool Init(const char* ServerIP,u32 ClientPort, u32 ServerPort);
	void Send(void* Data, u32 DataSize);
	void Recieve();
	void Shutdown();

};