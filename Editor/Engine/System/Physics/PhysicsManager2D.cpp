#include "PhysicsManager2D.h"
#include "Application\Engine.h"


void PhysicsManager2D::Init()
{
	Arena.Create(1024 * 1024);
}
void PhysicsManager2D::Update(f32 TimeStep)
{
	GEngine.pNetworkManager->GetObjMutex().lock();
	for (u32 j = 0; j < ObjectCount;j++)
	{
		if (!ObjectList[j].Enabled)
		{
			continue;
		}
		for (u32 i = j + 1; i < ObjectCount; i++)
		{
			if (!ObjectList[i].Enabled)
			{
				continue;
			}
			if (ObjectList[j].box.Intersect(ObjectList[i].box))
			{
				if (ObjectList[j].OnCollide)
				{
					ObjectList[j].OnCollide(ObjectList[j].ParentObject, &ObjectList[i]);
				}
				if(ObjectList[i].OnCollide)
				{
					ObjectList[i].OnCollide(ObjectList[i].ParentObject, &ObjectList[j]);
				}
			}
		}
	}
	for (u32 i = 0; i < ObjectCount; i++)
	{
		if (!ObjectList[i].Enabled)
		{
			continue;
		}
		ObjectList[i].Update(TimeStep);
	}
	GEngine.pNetworkManager->GetObjMutex().unlock();
}
void PhysicsManager2D::AddObject(void* Parent,Object2D* Obj, Vector* VertexList, u32 Count)
{
	ObjectList[ObjectCount].Init(&Arena, Parent,Obj, VertexList, Count);
	ObjectList[ObjectCount++].Enabled = true;
}
void PhysicsManager2D::RemoveObject(PhysicsObject2D* Obj)
{
	
}
