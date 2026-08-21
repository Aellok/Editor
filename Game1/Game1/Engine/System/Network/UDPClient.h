#pragma once
#include "Platform/Windows/Networking/Win_UDPClient.h"

struct UDPClient
{
	Win_UDPClient client;
	void* UserData;
	void (*OnSend)(void* Parent);
	void (*OnRecv)(void* Parent,void* Buffer,u32 BufferSize);
	void Init(const char* ServerIP, u32 ClientPort,u32 ServerPort);
	void Bind(void(*pOnSend)(void* Parent), void(*pOnRecv)(void* Parent, void* Buffer, u32 BufferSize), void* pUserData);
	void Send(void* Data, u32 Size);
};