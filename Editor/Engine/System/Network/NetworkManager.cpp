#include "NetworkManager.h"
#include "System\Utils\Hash.h"


std::unordered_map<u64, NetworkObject*> map;
std::mutex ObjMutex;

void NetworkManager::Init()
{
	ObjectCount = 0;
	LastUUID = 0;

}
u64 NetworkManager::AddObject(PhysicsObject2D* Object,u64 UUID)
{
	NetworkObject* Obj = &ObjectList[ObjectCount++];
	Obj->UUID = UUID;
	Obj->Object = Object;
	Obj->Broadcast = true;
	map[UUID] = Obj;
	return UUID;
}
NetworkObject* NetworkManager::FindObject(u64 UUID)
{
	return map[UUID];
}
std::mutex& NetworkManager::GetObjMutex()
{
	return ObjMutex;
}