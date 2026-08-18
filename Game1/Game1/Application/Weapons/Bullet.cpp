#include "Bullet.h"
#include "Application\Engine.h"
#include "Application\PhysicsObjectTypes.h"
void Bullet::Draw()
{
	Obj->Draw();
}
void Bullet::Update()
{
	//printf("Bullet Pos: (%f,%f)\n", PhysObj->Position.m128_f32[0], PhysObj->Position.m128_f32[1]);
}

void Bullet_OnCollide(void* Parent,PhysicsObject2D* Other)
{
	Bullet* bullet = (Bullet*)Parent;
	if (Other->ID == PhysBarrier)
	{
		//0: left 1: right 2: up: 3: down
		u32 Axis = 0;
		//sets the position to the last un collided pos.
		bullet->PhysObj->SeparateCollision(Other,&Axis);
		if (Axis == 0 || Axis == 1)
		{
			bullet->PhysObj->Velocity.x = -bullet->PhysObj->Velocity.x;
		}
		if (Axis == 2 || Axis == 3)
		{
			bullet->PhysObj->Velocity.y = -bullet->PhysObj->Velocity.y;
		}
	}
}

void Bullet::Init(Vector Pos, f32 Rotation, Vector Dim, f32 Speed)
{
	ObjectDesc desc = {this, Pos, Dim, { 0,0,Rotation }, { 1,1,1,1 }, "Centered", NULL, false, false, true, true};
	
	Obj = GEngine.pObjManager2D->AddObject(desc);
	
	Obj->SetRotation(Rotation);
	Matrix RotMat = DirectX::XMMatrixRotationZ(Rotation);
	PhysObj = &GEngine.pPhysicsManager2D->ObjectList[Obj->PhysicsID];
	PhysObj->OnCollide = Bullet_OnCollide;
	DirectX::XMStoreFloat4(&PhysObj->Velocity, DirectX::XMVectorScale(DirectX::XMVector2Normalize(DirectX::XMVector2Transform({ 0,1,0,0 }, RotMat)), Speed));
	PhysObj->ID = PhysBullet;
}

void Bullet::Destroy()
{
	GEngine.pObjManager2D->RemoveObject(Obj);
}