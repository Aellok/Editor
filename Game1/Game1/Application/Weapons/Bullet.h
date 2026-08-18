#pragma once
#include "System\Vector.h"
#include "System\Renderer\Object2D.h"
#include "System\Physics\PhysicsObject2D.h"
struct Bullet
{
	f32 Speed = 100;
	Object2D* Obj;
	PhysicsObject2D* PhysObj;
	void Init(Vector Pos, f32 Rotation, Vector Dim,f32 Speed);
	void Destroy();
	void Update();
	void Draw();
};