#include "Barrier.h"
#include "Application\Engine.h"
#include "Application\PhysicsObjectTypes.h"
void Barrier_OnCollide(void* parent, PhysicsObject2D* Other)
{
	Barrier* barrier = (Barrier*)parent;

}

void Barrier::Init(Vector Pos, Vector Dim)
{
	//Obj = GEngine.pObjManager2D->AddObject(this, Pos, Dim, { 0,0,0,0 }, { 1,1,1,1 }, "Centered", NULL, true, false, true,true);
	PhysObject = &GEngine.pPhysicsManager2D->ObjectList[Obj->PhysicsID];
	PhysObject->OnCollide = Barrier_OnCollide;
	PhysObject->ID = PhysBarrier;
}
void Barrier::Update()
{
	
}
void Barrier::Draw()
{
	Obj->Draw();
}