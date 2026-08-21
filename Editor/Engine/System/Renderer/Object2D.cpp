#include "Object2D.h"
void Object2D::Init(Vector pos,Vector dim, Vector rot,Vector color)
{
	Pos = pos;
	Dim = dim;
	Rot = rot;
	RotOffset = { 0,0,0,0 };
	Color = color;
	Visible = false;
}
void Object2D::Update(bool isCentered)
{
	if (isCentered)
	{
		Center = Pos;
		return;
	}
	Center = { (Pos.m128_f32[0] + Dim.m128_f32[0]) / 2,
			   (Pos.m128_f32[1] + Dim.m128_f32[1]) / 2};
}
Matrix Object2D::GetModelMatrix()
{
	Matrix Rotation = GetTranslationMatrix(RotOffset) *
					  GetRotationMatrix(Rot) *
					  DirectX::XMMatrixInverse(NULL,GetTranslationMatrix(RotOffset));

	return GetScaleMatrix(Dim) * Rotation * GetTranslationMatrix(Pos);
}
void Object2D::Draw()
{
	Visible = true;
}
void Object2D::SetRotation(f32 NewRot)
{
	Rot.m128_f32[2] = NewRot;
}
void Object2D::Rotate(f32 Rotation)
{
	Rot.m128_f32[2] += Rotation;
}