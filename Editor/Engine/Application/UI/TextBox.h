#pragma once
#include "System/Renderer/Object2D.h"
#include "System/Memory/DynamicArray.h"
#include "System/Renderer/Font/String2D.h"
#include "System\Renderer\ObjectManager2D.h"
#include "System/KeyboardManager.h"
#include "System/MouseManager.h"

struct TextBox
{
	KeyboardCallbacks kCallbacks;
	MouseCallbacks mCallbacks;

	ObjectManager* Manager;
	DynamicArray Content;
	Object2D* Background;
	String2D* String;

	bool IsClicked;
	void Init(ObjectManager* Manager2D, MouseManager* MManager, KeyboardManager* KManager, Vector Pos, Vector Dim, void(*OnChanged)());
	void Update();
	void Draw();
};