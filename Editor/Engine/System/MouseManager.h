#pragma once
#include "Types.h"
#include "Vector.h"
#include "System/Memory/DynamicArray.h"
#define CREATE_MOUSE_CALLBACK(ParentPtr,MouseCBVarName,PrefixName) {\
	##MouseCBVarName##.OnLButtonDown	= ##PrefixName##_LButtonDown;\
	##MouseCBVarName##.OnLButtonUp		= ##PrefixName##_LButtonUp;\
	##MouseCBVarName##.OnRButtonDown	= ##PrefixName##_RButtonDown;\
	##MouseCBVarName##.OnRButtonUp		= ##PrefixName##_RButtonUp;\
	##MouseCBVarName##.OnMButtonDown	= ##PrefixName##_MButtonDown;\
	##MouseCBVarName##.OnMButtonUp		= ##PrefixName##_MButtonUp;\
	##MouseCBVarName##.OnMove			= ##PrefixName##_Move;\
	##MouseCBVarName##.OnDragDrop		= NULL;\
	##MouseCBVarName##.Parent			= ParentPtr;\
	##MouseCBVarName##.IsEnabled		= true;\
}

struct Mouse
{
	u32 x;
	u32 y;
	s32 dx;
	s32 dy;
};
struct MouseCallbacks
{
	bool IsEnabled;
	void* Parent;
	void (*OnLButtonDown)(void* Parent,Mouse mouse);
	void (*OnRButtonDown)(void* Parent, Mouse mouse);
	void (*OnMButtonDown)(void* Parent, Mouse mouse);
	void (*OnLButtonUp)(void* Parent, Mouse mouse);
	void (*OnRButtonUp)(void* Parent, Mouse mouse);
	void (*OnMButtonUp)(void* Parent, Mouse mouse);
	void (*OnMove)(void* Parent, Mouse mouse);
	void (*OnDragDrop)(void* Parent, Mouse mouse,char* fileName);
};
struct MouseManager
{
	DynamicArray CallbackList;
	void Init();
	void Register(MouseCallbacks* NewCallback);
	void OnLClick(Mouse mouse);
	void OnRClick(Mouse mouse);
	void OnMClick(Mouse mouse);
	void OnLRelease(Mouse mouse);
	void OnRRelease(Mouse mouse);
	void OnMRelease(Mouse mouse);
	void Move(Mouse mouse);
	void DragDrop(Mouse mouse,char* FileName);
};
bool ContainsPoint(Mouse mouse, Vector Pos, Vector Dim);