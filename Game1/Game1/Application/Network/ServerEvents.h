#pragma once

#include "../../Engine/System/Types.h"
#include "../../Engine/System/Vector.h"

enum NE_SEvents
{
	eSNE_AcceptPlayer,
	eSNE_UpdateObjects,
	eSNE_PlayerShoot,
	eSNE_SpawnDynamicObject
};
enum NetworkObjectTypes
{
	eNOT_Player,
	eNOT_Bullet
};
//Server -> client events
struct NE_AcceptPlayer
{
	u32 EventID = NE_SEvents::eSNE_AcceptPlayer;
	u64 PlayerID = 0;
};
struct NE_ServerUpdateObjects
{
	u32 EventID = NE_SEvents::eSNE_UpdateObjects;
	u32 ObjectCount;
	u64 ObjUUID[50];
	Vector ObjPos[50];
	f32 ObjRot[50];
};

struct NE_SpawnDynamicObject
{
	u32 EventID = eSNE_SpawnDynamicObject;
	u64 UUID;
	u32 Type;
};
