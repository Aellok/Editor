#include "GameState.h"
#include "Application\Engine.h"
void GameState::SpawnPlayer(SpawnPlayerDesc* Desc)
{
	if (Desc->PlayerID == 0xffffffff)
	{
		players[PlayerCount].Init(Desc->Location, 10, 10, Desc->IsLocalPlayer,true);
		players[PlayerCount].Model->NetworkID = GEngine.pNetworkManager->AddObject(players[PlayerCount].PhysObj, Desc->UUID);
	}
	else
	{
		players[Desc->PlayerID].PhysObj->Position = Desc->Location;
	}
	PlayerCount++;
}
void GameState::Update()
{
	for (u32 i = 0; i < PlayerCount; i++)
	{
		players[i].Update();
	}
}
void GameState::Draw()
{
	for (u32 i = 0; i < PlayerCount; i++)
	{
		players[i].Draw();
	}
}
void GameState::AddCommand(GameStateCommand CommandList, void* CommandListParam)
{
	GSCommandList[CommandListCount] = CommandList;
	GSCommandListParams[CommandListCount++] = CommandListParam;
}
void GameState::RunCommands()
{
	for (u32 i = 0; i < CommandListCount; i++)
	{
		switch(GSCommandList[i].Type)
		{
			case eGSSpawnPlayer:
			{
				SpawnPlayer((SpawnPlayerDesc*)GSCommandListParams[i]);
				break;
			}
			case eGSSpawnBullet:
			{
				
				SpawnBulletDesc* Desc = (SpawnBulletDesc*)GSCommandListParams[i];
				printf("SHOOT! Count: %u\n", players[Desc->Index].BulletCount);
				players[Desc->Index].Shoot(Desc->UUID);
				
				break;
			}
		}
	}
	CommandListCount = 0;
}
void GameState::Initialize()
{
	CommandListCount = 0;
}