#include "Client.h"
#include "ClientEvents.h"
#include <stdio.h>
#include "ServerEvents.h"

#include "Application\Engine.h"
#include "Application\GameState.h"

//Every 16ms this is called.
void Client_OnSend(void* Parent)
{
	Client* client = (Client*)Parent;
	u32 ObjectCount = GEngine.pNetworkManager->ObjectCount;
	NetworkObject* ObjList = GEngine.pNetworkManager->ObjectList;



	NE_ServerUpdateObjects UpdateEvent;
	memset(&UpdateEvent, 0, sizeof(NE_ServerUpdateObjects));
	UpdateEvent.EventID = eSNE_UpdateObjects;
	for (u32 i = 0; i < ObjectCount; i++)
	{
		if (i % 50 == 0 && ObjectCount > 50)
		{
			UpdateEvent.ObjectCount = 50;
			client->Send(&UpdateEvent, sizeof(UpdateEvent));
			memset(&UpdateEvent, 0, sizeof(NE_ServerUpdateObjects));
			UpdateEvent.EventID = eSNE_UpdateObjects;
		}
		if (ObjList[i].Broadcast)
		{
			GEngine.pNetworkManager->GetObjMutex().lock();
			UpdateEvent.ObjUUID[i % 50] = ObjList[i % 50].UUID;
			UpdateEvent.ObjPos[i % 50] = ObjList[i % 50].Object->Position;
			UpdateEvent.ObjRot[i % 50] = ObjList[i % 50].Object->Rotation;
			GEngine.pNetworkManager->GetObjMutex().unlock();
		}
	}
	UpdateEvent.ObjectCount = ObjectCount % 50;
	client->Send(&UpdateEvent, sizeof(UpdateEvent));
	
}
void Client_OnRecv(void* Parent, void* Buffer, u32 BufferLength)
{
	Client* client = (Client*)Parent;

	switch(((u32*)Buffer)[0])
	{
		case eSNE_AcceptPlayer:
		{
			NE_AcceptPlayer* acceptPlayer = (NE_AcceptPlayer*)Buffer;

			printf("Connected to server, retrieved client ID: %llu\n",acceptPlayer->PlayerID);
			break;
		}
		case eSNE_UpdateObjects:
		{
			u32 ObjectCount = GEngine.pNetworkManager->ObjectCount;
			NetworkObject* ObjList = GEngine.pNetworkManager->ObjectList;

			NE_ClientUpdateObjects* UpdateEvent = (NE_ClientUpdateObjects*)Buffer;
			
			GEngine.pNetworkManager->GetObjMutex().lock();
			for (u32 i = 0; i < UpdateEvent->ObjectCount; i++)
			{
				
				NetworkObject* Obj = GEngine.pNetworkManager->FindObject(UpdateEvent->ObjUUID[i]);
				if (Obj)
				{
					PhysicsObject2D* PhysObj = Obj->Object;

					PhysObj->Position = ObjList[i].Object->Position;
					PhysObj->Rotation = ObjList[i].Object->Rotation;
				}
			}
			GEngine.pNetworkManager->GetObjMutex().unlock();
			break;
		}
		case eSNE_SpawnDynamicObject:
		{
			NE_SpawnDynamicObject* Event = (NE_SpawnDynamicObject*)Buffer;
			switch(Event->Type)
			{
				case eNOT_Player:
				{
					//THIS CAUSES A MEMORY LEAK HACK FIX
					SpawnPlayerDesc* Desc = (SpawnPlayerDesc*)GEngine.Global.Allocate(sizeof(SpawnPlayerDesc));
					Desc->IsLocalPlayer = client->gs->PlayerCount == 0;
					Desc->UUID = Event->UUID;
					Desc->PlayerID = -1;
					Desc->Location = client->gs->SpawnLocations[0];
					
					GameStateCommand cmd;
					cmd.Type = eGSSpawnPlayer;

					client->gs->AddCommand(cmd, Desc);
					break;
				}
			}
			
			break;
		}
		case eSNE_PlayerShoot:
		{
			NE_PlayerShoot* Event = (NE_PlayerShoot*)Buffer;
		
			for (u32 i = 0; i < client->gs->PlayerCount;i++ )
			{
				if (client->gs->players[i].Model->NetworkID == Event->PlayerUUID)
				{
					//THIS CAUSES A MEMORY LEAK HACK FIX
					SpawnBulletDesc* Desc = (SpawnBulletDesc*)GEngine.Global.Allocate(sizeof(SpawnBulletDesc));
					Desc->Index = i;
					Desc->UUID = Event->BulletUUID;

					GameStateCommand cmd;
					cmd.Type = eGSSpawnBullet;
					client->gs->AddCommand(cmd, Desc);
				}
			}
			
		}
		default:
		{
			break;
		}

	}
}
void Client::Init(GameState* pGameState,const char* ServerIP, u32 ClientPort, u32 ServerPort,bool ServerHost)
{
	GameStarted = false;
	gs = pGameState;
	client = GEngine.pClient;
	client->Bind(Client_OnSend, Client_OnRecv, this);
}
void Client::SudoRecv(void* Data, u32 Length)
{
	Client_OnRecv(this, Data, Length);
}
void Client::Send(void* Data, u32 Length)
{
	client->Send(Data,Length);
}