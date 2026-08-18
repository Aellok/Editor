#pragma once
#include "System\Types.h"
#include "System\Vector.h"
#include "System\Renderer\Object2D.h"
#include "System\Renderer\ObjectManager2D.h"
#include "System\Physics\PhysicsObject2D.h"
struct MeleeEnemy
{
	bool Alive = true;
	bool InputState[8] = { false };

	Vector ForwardDir;
	Object2D* Target;
	f32 Rotation;
	f32 Speed = 100;
	Object2D* Model;
	PhysicsObject2D* PhysObj;
	f32 MousePos[2] = { 0 };
	
	void Init(Object2D* target);
	void Update();
	void Draw();
	void Die();
	void Spawn();
};