#pragma once
#include "../../Engine/System/Network/UDPClient.h"
#include <mutex>

struct GameState;

struct Client
{
	GameState* gs;
	std::mutex mutex;
	bool GameStarted;
	UDPClient* client;
	bool Updating = false;
	void Init(GameState* pGameState,const char* ServerIP, u32 ClientPort, u32 ServerPort, bool ServerHost);
	void Send(void* Data, u32 Length);
	void SudoRecv( void* Data, u32 Length);
};