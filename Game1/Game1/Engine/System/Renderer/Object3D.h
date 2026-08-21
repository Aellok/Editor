#pragma once
#include "System/Vector.h"
#include "System/Types.h"
#include "System/Physics/AABB.h"
struct Object3D
{
	Vector Pos;
	Vector Rot;
	Vector Dim;
	Matrix RotMatrix;
	Vector Color;

	s32 MeshID;
	s32 TextureID;
	s32 PhysicsID;
	u64 NetworkID;
	s32 PipelineID;

	AABB BoundingBox;

	bool Visible;

	void Init(Vector Position,Vector Dimension, Vector Rotation);
	void Draw();
	void Update();
	Matrix GetModelMatrix();
};