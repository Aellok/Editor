#include "PhysicsObject2D.h"
#include "Application\Engine.h"
#include <math.h>
#include <limits>

#include <emmintrin.h>  // SSE2 base
#include <tmmintrin.h>  // SSSE3
#include <smmintrin.h>  // SSE4.1 (required for _mm_min_epi32)
#include "AABB2D.h"
struct Point
{
	union
	{
		__m128 IData;
		struct
		{
			f32 x;
			f32 y;
			f32 z;
			f32 w;
		};
	};
};

AABB2D GetAABBFromPoints(Vector* List,u32 Count,Matrix Model)
{
	AABB2D result;
	result.IData = { FLT_MAX, FLT_MAX, 0, 0 };
	Point min = {0};
	Point max = { 0 };
	Point abab = { 0 };
	Point result2 = {FLT_MAX,FLT_MAX,0,0};
	
	for (u32 i = 0; i < Count; i++)
	{
		Vector pos = DirectX::XMVector2Transform(List[i], Model);
		abab.IData = _mm_movelh_ps(pos, pos);
		min.IData = _mm_min_ps(result2.IData, abab.IData);
		max.IData = _mm_max_ps(result2.IData, abab.IData);
		result2.IData = _mm_shuffle_ps(min.IData, max.IData, _MM_SHUFFLE(3, 2, 1, 0));
	}
	_mm_storeu_ps(&result.IData.x, result2.IData);
	return result;
}


void PhysicsObject2D::Init(MemoryArena* pArena,void* Parent,Object2D* RenderObject,Vector* VertexList,u32 Count)
{
	ParentObject = Parent;
	Arena = pArena;
	Object = RenderObject;
	Position = Object->Pos;
	VertList = (Vector*)Arena->Allocate(sizeof(Vector) * Count);
	memcpy(VertList,VertexList,sizeof(Vector) * Count);
	VertCount = Count;
	box = GetAABBFromPoints(VertList,VertCount,Object->GetModelMatrix());
	oldBox.IData =  { 0.0f,0.0f,0.0f,0.0f };
}
void PhysicsObject2D::Update(f32 TimeStep)
{
	
	Position = Object->Pos;
	OldPos = Position;
	Position.m128_f32[0] = Position.m128_f32[0] + (Velocity.x * TimeStep);
	Position.m128_f32[1] = Position.m128_f32[1] + (Velocity.y * TimeStep);
	Object->Pos = Position;

	oldBox = box;
	box = GetAABBFromPoints(VertList, VertCount, Object->GetModelMatrix());
}

Vector PhysicsObject2D::SeparateCollision(PhysicsObject2D* Other,u32* Axis)
{
	//Move this into another collision file
	Vector result = {0,0,0,0};

	AABB2D RelativeDim = oldBox.GetRelativeDim(Other->oldBox);

	Vector V1 = DirectX::XMLoadFloat4(&Other->Velocity);;
	Vector V2 = DirectX::XMLoadFloat4(&Velocity);

	Vector RelativeVel = DirectX::XMVectorAdd(V1, V2);
	
	f32 tx1 = fabs(RelativeDim.x1 / RelativeVel.m128_f32[0]);
	f32 tx2 = fabs(RelativeDim.x2 / RelativeVel.m128_f32[0]);
	f32 ty1 = fabs(RelativeDim.y1 / RelativeVel.m128_f32[1]);
	f32 ty2 = fabs(RelativeDim.y2 / RelativeVel.m128_f32[1]);

	f32 minTx = fminf(tx1, tx2);
	f32 minTy = fminf(ty1, ty2);
	f32 t = fminf(minTx, minTy);
	
	f32 magRelativeV = DirectX::XMVector2Length(RelativeVel).m128_f32[0];

	f32 ScaledT = DirectX::XMVector2Length(V2).m128_f32[0] / magRelativeV;

	f32 ScaledOtherT = DirectX::XMVector2Length(V1).m128_f32[0] / magRelativeV;
	u32 axisX = (tx1 < tx2) ? 0 : 1;
	u32 axisY = (ty1 < ty2) ? 2 : 3;
	*Axis = (minTx < minTy) ? axisX : axisY;

	if (t == 0)
	{	
		Position = OldPos;
		Object->Pos = Position;
		box = GetAABBFromPoints(VertList, VertCount, Object->GetModelMatrix());
		return result;
	}
	Vector correctVel = DirectX::XMVectorScale(V2, t * ScaledT);
	Position = DirectX::XMVectorAdd(OldPos, correctVel);
	//Other->Position = DirectX::XMVectorAdd(Other->Position, DirectX::XMVectorScale(Other->Velocity, ScaledT));

	Object->Pos = Position;
	//Other->Object->Pos = Other->Position;
	box = GetAABBFromPoints(VertList, VertCount, Object->GetModelMatrix());

	return result;
}