#pragma once
#include "System\KeyboardManager.h"
#include "System\MouseManager.h"
#include "System\Types.h"
#include "System\Vector.h"
#include "System\Renderer\Object2D.h"
#include "System\Renderer\ObjectManager2D.h"
#include "Application\Weapons\Bullet.h"
#include "System\Physics\PhysicsObject2D.h"
struct Player
{
	MouseCallbacks MouseCB;
	KeyboardCallbacks KeyboardCB;

	bool InputState[8] = {false};

	bool bIsLocalPlayer = false;
	Vector ForwardDir;

	f32 Rotation;
	f32 Speed = 150;
	Object2D* Model;
	PhysicsObject2D* PhysObj;
	NetworkObject* NetworkObj;

	f32 MousePos[2] = { 0 };
	u32 BulletCount = 0;
	Bullet BulletList[1024];
	void Init(Vector Location,u32 width, u32 height,bool IsLocalPlayer, bool DeferredAdd);
	void Update();
	void Draw();
	void Shoot(u64 UUID);
	void Shoot();
};