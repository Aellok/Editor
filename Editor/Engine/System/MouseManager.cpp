#include "MouseManager.h"
#include <memory>
void MouseManager::Init()
{
	CallbackList.Init(8, sizeof(MouseCallbacks*));
}
void MouseManager::Register(MouseCallbacks* NewCallback)
{
	CallbackList.Add(&NewCallback);
}
void MouseManager::OnLClick(Mouse mouse)
{
	MouseCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(MouseCallbacks*,CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i]->OnLButtonDown && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnLButtonDown(Callbacks[i]->Parent,mouse);
		}
	}
}
void MouseManager::OnRClick(Mouse mouse)
{
	MouseCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(MouseCallbacks*, CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i]->OnRButtonDown && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnRButtonDown(Callbacks[i]->Parent, mouse);
		}
	}
}
void MouseManager::OnMClick(Mouse mouse)
{
	MouseCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(MouseCallbacks*, CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i]->OnMButtonDown && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnMButtonDown(Callbacks[i]->Parent, mouse);
		}
	}
}
void MouseManager::OnLRelease(Mouse mouse)
{
	MouseCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(MouseCallbacks*, CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i] && Callbacks[i]->OnLButtonUp && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnLButtonUp(Callbacks[i]->Parent, mouse);
		}
	}
}
void MouseManager::OnRRelease(Mouse mouse)
{
	MouseCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(MouseCallbacks*, CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i]->OnRButtonUp && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnRButtonUp(Callbacks[i]->Parent, mouse);
		}
	}
}
void MouseManager::OnMRelease(Mouse mouse)
{
	MouseCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(MouseCallbacks*, CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i]->OnMButtonUp && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnMButtonUp(Callbacks[i]->Parent, mouse);
		}
	}
}
void MouseManager::Move(Mouse mouse)
{
	MouseCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(MouseCallbacks*, CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i]->OnMove && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnMove(Callbacks[i]->Parent, mouse);
		}
	}
}
void MouseManager::DragDrop(Mouse mouse,char* FileName)
{
	MouseCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(MouseCallbacks*, CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i] && Callbacks[i]->OnDragDrop && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnDragDrop(Callbacks[i]->Parent, mouse,FileName);
		}
	}
}
bool ContainsPoint(Mouse mouse, Vector Pos, Vector Dim)
{
	if (Pos.m128_f32[0] <= mouse.x && (Pos.m128_f32[0] + Dim.m128_f32[0]) >= mouse.x)
	{
		if (Pos.m128_f32[1] <= mouse.y && (Pos.m128_f32[1] + Dim.m128_f32[1]) >= mouse.y)
		{
			return true;
		}
	}
	return false;
}