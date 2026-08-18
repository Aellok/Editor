#pragma once
#include "System/Network/UDPServer.h"
#include "System/Types.h"
#include "System/Vector.h"
#include <mutex>
#include "Client.h"
struct ServerPlayerInfo
{
	std::mutex Mutex;
	u32 ID;
	Vector Pos;
	Vector Forward;
	Matrix viewMat;
};

struct GameState;
struct AppServer
{
	GameState* gs;
	//players
	Client* client;
	UDPServer* server;
	u64 lastUUID;
	u64 GenerateUUID();
	void Init(GameState* pGameState,const char* iIP, const char* eIP, u32 Port);
	void Send(void* Data,u32 Size,u32 Index);
	void Broadcast(void* data, u32 size, u32 excludeIndex);
	u32 GetClientCount() { return server->SystemServer.ClientCount; }
	char* GetClientIP(u32 Index) { return (char*)&server->SystemServer.Addresses[Index]; }

};