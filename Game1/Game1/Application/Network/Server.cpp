#include "Server.h"
#include "ServerEvents.h"
#include "ClientEvents.h"
#include "System\Utils\Hash.h"
#include "Application\Engine.h"
#include "Application\GameState.h"
//called every 16ms
void Server_OnSend(void* Parent)
{
	AppServer* appServer = (AppServer*)Parent;
	NetworkObject* ObjList = GEngine.pNetworkManager->ObjectList;

	u32 ObjectCount = GEngine.pNetworkManager->ObjectCount;

	NE_ClientUpdateObjects UpdateEvent;
	UpdateEvent.ObjectCount = ObjectCount;

	assert(UpdateEvent.ObjectCount < 512);

	for (u32 i = 0; i < ObjectCount; i++)
	{
		if (ObjList[i].Broadcast)
		{
			GEngine.pNetworkManager->GetObjMutex().lock();
			UpdateEvent.ObjUUID[i] = ObjList[i].UUID;
			UpdateEvent.ObjPos[i] = ObjList[i].Object->Position;
			UpdateEvent.ObjRot[i] = ObjList[i].Object->Rotation;
			GEngine.pNetworkManager->GetObjMutex().unlock();
		}
	}
	for (u32 i = 0; i < appServer->GetClientCount();i++)
	{
		appServer->Send(&UpdateEvent, sizeof(UpdateEvent),i);
	}
	
}
void Server_OnRecv(void* parent,void* Data,u32 Index)
{
	AppServer* appServer = (AppServer*)parent;

	switch (((u32*)Data)[0])
	{
		case eCNE_ConnectToServer:
		{
			NE_ConnectToServer* Event = (NE_ConnectToServer*)Data;
			
			NE_AcceptPlayer AcceptPlayer;
			AcceptPlayer.PlayerID = GenerateSHA256Hash(appServer->GetClientIP(Index), sizeof(sockaddr_in));
			appServer->Send(&AcceptPlayer,sizeof(AcceptPlayer), Index);

			SpawnPlayerDesc Desc;
			Desc.Location = appServer->gs->SpawnLocations[appServer->gs->PlayerCount];
			Desc.PlayerID = -1;
			Desc.IsLocalPlayer = false;
			Desc.UUID = appServer->GenerateUUID();
			appServer->gs->SpawnPlayer(&Desc);

			NE_SpawnDynamicObject DynObject;
			DynObject.Type = eNOT_Player;
			DynObject.UUID = appServer->gs->players[appServer->gs->PlayerCount - 1].Model->NetworkID;

			appServer->Broadcast(&DynObject, sizeof(NE_SpawnDynamicObject),100);
			break;
		}
		case eCNE_UpdateObjects:
		{
			NE_ServerUpdateObjects* Event = (NE_ServerUpdateObjects*)Data;

			for (u32 i = 0; i < Event->ObjectCount; i++)
			{
				GEngine.pNetworkManager->GetObjMutex().lock();
				NetworkObject* Object = GEngine.pNetworkManager->FindObject(Event->ObjUUID[i]);
				if (Object)
				{
					PhysicsObject2D* PhysObject = Object->Object;

					PhysObject->Position = Event->ObjPos[i];
					PhysObject->Rotation = Event->ObjRot[i];
					printf("Position = (%f,%f)\n", PhysObject->Position.m128_f32[0], PhysObject->Position.m128_f32[1]);
				}
				GEngine.pNetworkManager->GetObjMutex().unlock();
			}

			break;
		}
		case eCNE_PlayerShoot:
		{
			NE_PlayerShoot* Event = (NE_PlayerShoot*)Data;
			
			Event->EventID = eSNE_PlayerShoot;

			appServer->gs->players[Index].Shoot(appServer->GenerateUUID());
			Event->BulletUUID = appServer->lastUUID;
			appServer->Broadcast(Event, sizeof(NE_PlayerShoot),100);
			break;
		}
		default:
		{
			printf("Unsupported network event %u\n", ((u32*)Data)[0]);
		}
	}

}
void AppServer::Init(GameState* pGameState,const char* iIP, const char* eIP, u32 Port)
{
	gs = pGameState;
	server = GEngine.pServer;
	server->Bind(Server_OnRecv, Server_OnSend, this);
}
void AppServer::Broadcast(void* data, u32 size, u32 excludeIndex)
{
	for (u32 i = 0; i < GetClientCount(); i++)
	{
		if (excludeIndex != i)
		{
			Send(data, size, i);
		}
	}
}
void AppServer::Send(void* Data,u32 Size,u32 Index)
{
	server->Send(Data,Size,Index);
}
u64 AppServer::GenerateUUID()
{
	lastUUID = ((6364136223846793005 * lastUUID) + 1442695040888963407);
	return lastUUID;
}