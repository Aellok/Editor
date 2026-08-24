#pragma once
#include "System\MouseManager.h"
#include "System\KeyboardManager.h"
#include "System\Renderer\Font\String2D.h"
#include "System\Renderer\ObjectManager2D.h"
#include "System\Memory\DynamicArray.h"
struct TextEditor
{
	KeyboardCallbacks kCallbacks;
	MouseCallbacks mCallbacks;
	ObjectManager* ObjManager2D;
	Vector Pos;
	Vector Dim;

	DynamicArray Contents;

	String2D* String;
	u32 Size;
	u32 CursorPosX;
	u32 CursorPosY;
	u32 CursorIndex;
	Object2D* Cursor;

	bool IsShiftDown;
	bool IsCtrlDown;

	bool Focused;

	bool Changed;

	void Init(ObjectManager* ObjManager,Vector InPos,Vector InDim,u32 Size, const char* DEBUG_fileName);
	
	void Update();
	void Draw();
	Vector GetCursorPosition(u32 X, u32 Y);
	Vector UpdateCursor();
	void AddChar(u32 Char, u32 ContentIndex);
	u32 GetStringIndexFromXY(u32 LineX, u32 LineY);
	void DeleteCharacter(u32 x, u32 y);
};