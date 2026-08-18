#pragma once
#include "System\Types.h"
#include "System\Vector.h"
#include "DX12CommandQueue.h"
#include "System/KeyboardManager.h"
#include "System/MouseManager.h"
struct DX12Camera
{
	MouseCallbacks MCallbacks;
	KeyboardCallbacks KCallbacks;

	f32 fov;

	Matrix PMatrix;
	Matrix VMatrix;
	Vector Rotation;
	Vector Forward;
	Vector Left;

	f32 AngleX;
	f32 AngleY;
	f32 Speed;
	Vector Position;

	bool IsOrtho;
	bool IsActive;
	bool IsRotating;
	bool Directions[4];

	void Initialize(f32 fov,u32 ScreenWidth,u32 ScreenHeight, Vector Pos,bool IsOrthographic);

	void Update(f32 FrameTime);
	void Resize(u32 ScreenWidth, u32 ScreenHeight);
};