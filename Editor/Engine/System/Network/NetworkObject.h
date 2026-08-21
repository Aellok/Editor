#pragma once
#include "System\Types.h"
#include "System\Renderer\Object2D.h"

struct PhysicsObject2D;
struct NetworkObject
{
	u64 UUID;
	PhysicsObject2D* Object;
	bool Broadcast = false;
};