#include "Win_UDPClient.h"
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

sockaddr_in SendInfo;
sockaddr_in RecvInfo;
SOCKET RecvSocket;
bool Win_UDPClient::Init(const char* ServerIP, u32 ClientPort,u32 ServerPort)
{
	WSADATA data;
	short version = MAKEWORD(2, 2);

	if (WSAStartup(version, &data))
	{
		printf("Error WSAStartup\n");
		return false;
	}
	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	SendInfo.sin_family = AF_INET;
	SendInfo.sin_port = htons(ServerPort);
	inet_pton(AF_INET, ServerIP, &SendInfo.sin_addr);
	
	RecvInfo.sin_addr.S_un.S_addr = ADDR_ANY;
	RecvInfo.sin_family = AF_INET;
	RecvInfo.sin_port = htons(ClientPort);

	if (bind(RecvSocket, (sockaddr*)&RecvInfo, sizeof(RecvInfo)) == SOCKET_ERROR)
	{
		printf("Error: Bind failed %d\n",WSAGetLastError());
	}
	memset(RecvBuffer, 0, 65535);
	return true;
}
void Win_UDPClient::Send(void* Data, u32 DataSize)
{
	int sendok = sendto(RecvSocket, (char*)Data, DataSize, 0, (sockaddr*)&SendInfo, sizeof(SendInfo));
	if (sendok == SOCKET_ERROR)
	{
		printf("Error sending data to server,%d\n", WSAGetLastError());
	}
}
void Win_UDPClient::Recieve()
{
	//TODO: Make sure that its actually from the server..
	s32 size = sizeof(RecvInfo);
	RecvSize = recvfrom(RecvSocket, RecvBuffer, 65534, 0, (sockaddr*)&RecvInfo, &size);

	if (RecvSize == SOCKET_ERROR)
	{
		printf("Error: Recv Error: %d\n",WSAGetLastError());
	}
}
void Win_UDPClient::Shutdown()
{

}