#pragma once

#include "UPnP.h"
#include <WS2tcpip.h>
#define MAX_CLIENTS 4
#define MAX_PACKET_SIZE 65535
struct Win_UDPServer
{
	UPnP upnp;
	UPnPEntry UDPServerEntry;
	SOCKET RecvSocket;
	SOCKET SendSocket;
	sockaddr_in socketInfo;
	u32 ClientCount;
	s32 BytesLeft;
	char RecentDatagram[MAX_PACKET_SIZE];
	sockaddr_in Addresses[MAX_CLIENTS];

	void Init(const char* InternalIP, const char* ExternalIP, u32 Port);
	void Shutdown();
	void Send(void* Data, u32 DataLength, sockaddr_in Client);
	u32 Recieve();
	u32 GetClientIndex(sockaddr_in addr);
};