#include "UDPClient.h"
#include <thread>
#pragma warning(disable:4996)
std::thread RecvThread;
std::thread SendThread;
//60 sends per sec
#define SEND_INTERVAL_MS 100 

void SendProccess(UDPClient* Parent)
{
	while(1)
	{
		_sleep(SEND_INTERVAL_MS);
		if (Parent->OnSend)
		{
			Parent->OnSend(Parent->UserData);
		}
	}
}
void RecvProccess(UDPClient* Parent)
{
	while(1)
	{
		if (Parent->OnRecv)
		{
			Parent->client.Recieve();
			Parent->OnRecv(Parent->UserData, Parent->client.RecvBuffer, Parent->client.RecvSize);
		}
	}
}
void UDPClient::Init(const char* ServerIP, u32 ClientPort,u32 ServerPort )
{
	client.Init(ServerIP, ClientPort, ServerPort);
	
	RecvThread = std::thread(RecvProccess, this);
	RecvThread.detach();
	SendThread = std::thread(SendProccess, this);
	SendThread.detach();
}
void UDPClient::Bind(void(*pOnSend)(void* Parent), void(*pOnRecv)(void* Parent, void* Buffer, u32 BufferSize), void* pUserData)
{
	UserData = pUserData;
	OnSend = pOnSend;
	OnRecv = pOnRecv;
}
void UDPClient::Send(void* Data, u32 Length)
{
	client.Send(Data, Length);
}