#pragma once
#include "../../Platform/Windows/Renderer/DirectX12/DX12Object2D.h"
#include "../Types.h"
struct Object2D
{
	Vector Pos;
	Vector Dim;
	bool IsCentered;
	Vector Center;

	Vector Rot;
	Vector RotOffset;

	Vector Color;
	s32 MeshID;
	s32 TextureID;
	s32 PhysicsID;
	u64 NetworkID;
	s32 PipelineID;
	bool Visible;

	void Init(Vector pos, Vector dim, Vector rot,Vector color);
	void Draw();
	void Update(bool isCentered);
	void Rotate(f32 Rotation);
	void SetRotation(f32 NewRot);

	Matrix GetModelMatrix();
	//DX12Object2D Object;

};