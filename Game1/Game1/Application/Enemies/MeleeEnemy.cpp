#include "MeleeEnemy.h"
#include "Application\Engine.h"
void MeleeEnemy_OnCollide(void* Parent, PhysicsObject2D* Other)
{
	MeleeEnemy* Self = (MeleeEnemy*)Parent;
	Self->Die();
}
void MeleeEnemy::Init(Object2D* target)
{
	Window* pWindow = GEngine.pWindow;
	f32 Dim = (f32)10 + (rand() % 20);
	//Model = GEngine.pObjManager2D->AddObject(this,{ (f32)(rand() % pWindow->width),(f32)(rand() % pWindow->height) }, { Dim ,Dim }, {0,0,0}, {1,0,0,1}, "Centered", NULL, false, false, true,true);
	PhysObj = &GEngine.pPhysicsManager2D->ObjectList[Model->PhysicsID];
	Target = target;
}
void MeleeEnemy::Update()
{
	Vector Dir = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(Target->Pos , Model->Pos));
	DirectX::XMStoreFloat4(&PhysObj->Velocity , DirectX::XMVectorScale( Dir , Speed));
}
void MeleeEnemy::Draw()
{
	if (Alive)
	{
		Model->Draw();
	}
}
void MeleeEnemy::Spawn()
{
	Alive = true;
	
	PhysObj->Enabled = true;
	Model->Pos = { (f32)(rand() % GEngine.pWindow->width),(f32)(rand() % GEngine.pWindow->height) };
	PhysObj->OnCollide = MeleeEnemy_OnCollide;
	
}
void MeleeEnemy::Die()
{
	Alive = false;
	PhysObj->Enabled = false;
	Model->Visible = false;
	printf("Died\n");
}
