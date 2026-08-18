#include "Win_UDPServer.h"
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
void Win_UDPServer::Init(const char* InternalIP,const char* ExternalIP,u32 Port)
{
	upnp.Init();
	if (!upnp.AddEntry(InternalIP,ExternalIP,"SERVER","UDP",Port))
	{
		printf("Failed to create UPnP\n");
	}
	UDPServerEntry = upnp.Entries[upnp.EntryCount - 1];
	upnp.PrintMappings();

	WSADATA data;
	u16 version = MAKEWORD(2, 2);
	
	if (WSAStartup(version,&data) != 0)
	{
		printf("Error WSAStartup\n");
		return;
	}
	
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	socketInfo.sin_addr.S_un.S_addr = ADDR_ANY;
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_port = htons(Port);

	if (bind(RecvSocket, (sockaddr*)&socketInfo, sizeof(socketInfo)) == SOCKET_ERROR)
	{
		printf("Error on bind\n");
		return;
	}

	memset(RecentDatagram, 0, MAX_PACKET_SIZE);
	ClientCount = 0;
	BytesLeft = 0;
}
u32 Win_UDPServer::Recieve()
{
	sockaddr_in cInfo;
	int ClientSize = sizeof(cInfo);
	BytesLeft = recvfrom(RecvSocket, RecentDatagram, MAX_PACKET_SIZE - 1, 0, (sockaddr*)&cInfo, &ClientSize);
	if (BytesLeft == SOCKET_ERROR)
	{
		return -1;
	}

	for (u32 i = 0; i < ClientCount;i++)
	{
		if (!memcmp(&cInfo,&Addresses,sizeof(sockaddr_in)))
		{
			return i;
		}
	}
	Addresses[ClientCount++] = cInfo;
	return ClientCount - 1;
}
void Win_UDPServer::Send(void* Data,u32 DataLength, sockaddr_in cInfo)
{
	u32 SendBytes = sendto(RecvSocket, (char*)Data, DataLength, 0, (sockaddr*)&cInfo, sizeof(cInfo));
	//printf("Ip: %s\n", inet_ntoa(clientInfo[ClientIndex].sin_addr));
	//printf("port: %d\n", ntohs(clientInfo[ClientIndex].sin_port));
	if (!SendBytes)
	{
		printf("Send failed %d\n",WSAGetLastError());
	}
}
void Win_UDPServer::Shutdown()
{
	upnp.Remove(UDPServerEntry);
}
