#include "UDPServer.h"
#include <thread>
std::thread ServerRecvThread;
std::thread ServerSendThread;

#define SEND_INTERVAL_MS 16 

void ServerRecv(void* Parent,const char* InternalIP,const char* ExternalIP,u32 Port)
{
	UDPServer* server = (UDPServer*)Parent;
	while(1)
	{
		//recv data
		u32 Index = server->SystemServer.Recieve();
		//send to ServerProc for processing

		if (server->OnSend)
		{
			server->OnRecv(server->UserData, server->SystemServer.RecentDatagram, Index);
		}
		
		server->SystemServer.BytesLeft = 0;
	}
}
void ServerSend(void* Parent)
{
	UDPServer* server = (UDPServer*)Parent;
	while (1)
	{
		Sleep(SEND_INTERVAL_MS);
		if (server->OnSend)
		{
			server->OnSend(server->UserData);
		}
		
	}
}
void UDPServer::Init(const char* InternalIP, const char* ExternalIP, u32 Port)
{
	SystemServer.Init(InternalIP, ExternalIP, Port);
	ServerRecvThread = std::thread(ServerRecv,this,InternalIP,ExternalIP,Port);
	ServerRecvThread.detach();
	ServerSendThread = std::thread(ServerSend, this);
	ServerSendThread.detach();
}
void UDPServer::Bind(void (*pOnRecv)(void* parent, void* Data, u32 Index), void (*pOnSend)(void* Parent),void* Parent)
{
	UserData = Parent;
	OnRecv = pOnRecv;
	OnSend = pOnSend;
}
void UDPServer::Send(void* Data, u32 Size, u32 Index)
{
	SystemServer.Send(Data, Size, SystemServer.Addresses[Index]);
}
void UDPServer::Shutdown()
{
	SystemServer.Shutdown();
	ServerRecvThread.~thread();
	ServerSendThread.~thread();
}