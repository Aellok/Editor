#pragma once

#include "../../Engine/System/Types.h"
#include "../../Engine/System/Vector.h"

enum NE_CEvents
{
	eCNE_ConnectToServer,
	eCNE_UpdateObjects,
	eCNE_PlayerShoot
};
//Returns a Player ID back
struct NE_ConnectToServer
{
	u32 EventID = NE_CEvents::eCNE_ConnectToServer;
	u32 e = 1;
};
struct NE_ClientUpdateObjects
{
	u32 EventID = NE_CEvents::eCNE_UpdateObjects;
	u32 ObjectCount;
	u64 ObjUUID[512];
	Vector ObjPos[512];
	f32 ObjRot[512];
};
struct NE_PlayerShoot
{
	u32 EventID = eCNE_PlayerShoot;
	Vector Dir;
	Vector Pos;
	u64 PlayerUUID;
	u64 BulletUUID;
	u32 DEBUGNumber = 0;
};
