#pragma once
#include "System\Types.h"
#include "System\Vector.h"
#include "System\Renderer\Object2D.h"
#include "PhysicsObject2D.h"


struct PhysicsManager2D
{
	PhysicsObject2D ObjectList[MAX_OBJECTS_2D];
	u32 ObjectCount;
	MemoryArena Arena;

	void Init();
	void Update(f32 TimeStep);
	void AddObject(void* Parent,Object2D* Obj, Vector* VertexList, u32 Count);
	void RemoveObject(PhysicsObject2D* Obj);

};