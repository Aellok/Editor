#pragma once
#include "System\Physics\PhysicsObject2D.h"
#include "System\Renderer\ObjectManager2D.h"
struct Barrier
{
	Object2D* Obj;
	PhysicsObject2D* PhysObject;

	void Init(Vector Pos,Vector Dim);
	void Update();
	void Draw();
};