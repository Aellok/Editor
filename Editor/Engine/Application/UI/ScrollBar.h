#pragma once
#include "System/Types.h"
#include "System/Renderer/Object2D.h"
#include "System\MouseManager.h"
#include "System\Renderer\ObjectManager.h"
typedef void(*OnScrollUpdate)(void* Parent,f32 Percentage);
struct ScrollBar
{
	MouseCallbacks MCallbacks;

	void* Parent;
	f32 Percentage;
	u32 StartPos;
	u32 EndPos;

	Vector Color;
	Vector SelectedColor;

	Object2D* Bar;
	Object2D* Background;

	bool Selected;

	OnScrollUpdate UpdateCallback;

	void Init(ObjectManager* Manager, MouseManager* mManager,Vector Pos, Vector Dim,Vector Rot,Vector BackgroundColor,Vector BarColor, OnScrollUpdate UpdateCallback, void* parent);
	void Update();
	void Draw();
};