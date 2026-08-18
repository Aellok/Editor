#pragma once
#include "Player\Player.h"
enum GameStateCommandID
{
	eGSSpawnPlayer,
	eGSSpawnBullet
};
struct GameStateCommand
{
	u32 Type;
};
struct SpawnBulletDesc
{
	u64 UUID;
	u32 Index;
};
struct SpawnPlayerDesc
{
	Vector Location;
	u32 PlayerID;
	bool IsLocalPlayer;
	u64 UUID;
};
struct GameState
{
	u32 PlayerCount = 0;
	Player players[4];
	Vector SpawnLocations[4] = { {100,100},{200,100},{300,100},{400,100} };

	u32 CommandListCount = 0;
	GameStateCommand GSCommandList[255];
	void* GSCommandListParams[255];
	void Initialize();
	void AddCommand(GameStateCommand CommandList,void* CommandListParam);
	void RunCommands();
	void SpawnPlayer(SpawnPlayerDesc* Desc);

	void Update();
	void Draw();
};