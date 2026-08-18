#pragma once
#include "System\Renderer\Font\String2D.h"
#include "System\Renderer\Object2D.h"
#include "System\MouseManager.h"
#include "System\Renderer\ObjectManager2D.h"

struct ButtonDesc
{
	bool ShrinkToText;
	bool Center;
	Vector Pos;
	Vector Dim;
	const char* Text;
	u32 TextSize;
	ObjectManager2D* ObjectManager;
	Vector Color;
	Vector SelectedColor;
	//UserInput
	MouseManager* Manager; // what Manager to register to
	MouseCallbacks Callbacks;

};

struct UIButton
{
	Object2D* Base;
	String2D* Text;
	Vector Color;
	Vector SelectedColor;

	ButtonDesc desc;

	MouseCallbacks mCallbacks;
	MouseCallbacks UserCallbacks;

	void Initialize(ButtonDesc Desc);
	void SetPos(Vector Pos);
	void Draw();
	void SetAcceptInput(bool Value);
};