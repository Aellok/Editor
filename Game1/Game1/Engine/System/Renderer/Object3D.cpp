#include "Object3D.h"

void Object3D::Init(Vector Position,Vector Dimension, Vector Rotation)
{
	Pos = Position;
	Dim = Dimension;
	Rot = Rotation;
	RotMatrix = GetRotationMatrix(Rot);
	Visible = false;
}

Matrix Object3D::GetModelMatrix()
{
	return GetScaleMatrix(Dim) * RotMatrix * GetTranslationMatrix(Pos);
}

void Object3D::Draw()
{
	Visible = true;
}
void Object3D::Update()
{
	RotMatrix = GetRotationMatrix(Rot);
}