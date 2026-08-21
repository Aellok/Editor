#pragma once
#include "Platform/Windows/Networking/Win_UDPServer.h"
#include <mutex>
#define UDPSERVER_MSG_SIZE 255

struct UDPServer
{
	Win_UDPServer SystemServer;
	void* UserData;
	void (*OnRecv)(void* parent, void* Data, u32 Index);
	void (*OnSend)(void* Parent);
	void Init(const char* InternalIP, const char* ExternalIP, u32 Port);
	void Bind(void (*pOnRecv)(void* parent, void* Data, u32 Index), void (*pOnSend)(void* Parent), void* UserData);
	void Send(void* Data, u32 Size, u32 Index);
	void Shutdown();
};