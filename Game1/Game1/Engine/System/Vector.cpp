#include "Vector.h"

Matrix GetTranslationMatrix(Vector Pos)
{
	return DirectX::XMMatrixTranslation(Pos.m128_f32[0], Pos.m128_f32[1], Pos.m128_f32[2]);
}
Matrix GetScaleMatrix(Vector Dim)
{
	return DirectX::XMMatrixScaling(Dim.m128_f32[0], Dim.m128_f32[1], Dim.m128_f32[2]);
}
Matrix GetRotationMatrix(Vector Rot)
{
	return DirectX::XMMatrixRotationX(Rot.m128_f32[0]) *
		DirectX::XMMatrixRotationY(Rot.m128_f32[1]) *
		DirectX::XMMatrixRotationZ(Rot.m128_f32[2]);
}
Matrix GetAlignmentRotation_3D(Vector target, Vector start)
{
	
	//for now the Target2 normal is 0,0,1;
	target = DirectX::XMVector3Normalize(target); // convert to unit vector
	start = DirectX::XMVector3Normalize(start);
	Vector w = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(target, start));
	if (w.m128_f32[0] == 0 && w.m128_f32[1] == 0 && w.m128_f32[2] == 0)
	{
		start.m128_f32[0] += FLT_EPSILON;
		start = DirectX::XMVector3Normalize(start);
		w = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(target, start));
	}
	Matrix K = { 
		    {0			   ,-w.m128_f32[2]  ,w.m128_f32[1] ,0},
			{w.m128_f32[2] ,0               ,-w.m128_f32[0],0},
			{-w.m128_f32[1],w.m128_f32[0]   ,0             ,0},
			{0,0,0,0} };
	f32 Angle = acos((DirectX::XMVector3Dot(target, start)).m128_f32[0]);

	return DirectX::XMMatrixIdentity() + (sin(Angle) * K) + ((1 - cos(Angle)) * (K * K));
}

f32 GetAlignmentRotation_2D(Vector target, Vector start)
{
	
	target = DirectX::XMVector2Normalize(target);
	start = DirectX::XMVector2Normalize(start);
	//f32 rot = DirectX::XMVector2Dot(target, start).m128_f32[0] / (DirectX::XMVector2Length(target).m128_f32[0] * DirectX::XMVector2Length(start).m128_f32[0] );
	f32 TargetAngle = atan2f(target.m128_f32[1], target.m128_f32[0]);
	f32 startAngle = atan2f(start.m128_f32[1], start.m128_f32[0]);

	return TargetAngle - startAngle;
}