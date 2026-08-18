#pragma once
#include "System\Types.h"
#include "System\Vector.h"
#include "System\Renderer\Object2D.h"
#include "System\Physics\AABB2D.h"
#include "System\Renderer\ObjectManager2D.h"
#include "Platform\Windows\Renderer\DirectX12\DX12Object2D.h"
#pragma pack(push, 16) // or simply reset to default before the definition

__declspec(align(16))
struct PhysicsObject2D
{
	
	Vector Position;
	Vector OldPos;
	AABB2D box;
	AABB2D oldBox;
	DirectX::XMFLOAT4 Velocity;
	Vector* VertList;

	f32 Rotation;
	Object2D* Object;
	u32 VertCount;
	bool Enabled;
	MemoryArena* Arena;
	void* ParentObject;

	u32 ID;

	void(*OnCollide)(void* Parent, PhysicsObject2D* Other);

	void Init( MemoryArena* Arena, void* Parent,Object2D* RenderObject,Vector* VertexList,u32 VertexCount);
	void Update(f32 TimeStep);
	Vector SeparateCollision(PhysicsObject2D* Other, u32* Axis);
};
#pragma pack(pop)