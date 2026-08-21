#pragma once
#include "System\Physics\PhysicsObject2D.h"
#include "NetworkObject.h"
#include <unordered_map>
#include <mutex>
struct NetworkManager
{
	u32 ObjectCount;
	NetworkObject ObjectList[255];
	u64 LastUUID;

	void Init();
	u64 AddObject(PhysicsObject2D* Object,u64 UUID);
	NetworkObject* FindObject(u64 UUID);
	std::mutex& GetObjMutex();
};