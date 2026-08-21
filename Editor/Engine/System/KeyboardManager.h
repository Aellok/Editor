#pragma once
#include "Types.h"
#include "System\Memory\DynamicArray.h"
#define CREATE_KEYBOARD_CALLBACK(Owner,CallbackName,typeName) {\
	##CallbackName##.OnKeyUp   = ##typeName##_KeyUp;\
	##CallbackName##.OnKeyDown = ##typeName##_KeyDown;\
	##CallbackName##.Parent	   = Owner;\
	##CallbackName##.IsEnabled = true;\
}

struct KeyboardCallbacks
{
	bool IsEnabled;
	void* Parent;
	void (*OnKeyDown)(void* Parent,u32 KeyCode);
	void (*OnKeyUp)(void* Parent, u32 KeyCode);

};

struct KeyboardManager
{
	//TODO: this is a hack because lparam isnt passed in
	u32 LastlParam;
	
	DynamicArray CallbackList;
	void Init();
	void Register(KeyboardCallbacks* Callbacks);
	void OnKeyUp(u32 KeyCode);
	void OnKeyDown(u32 KeyCode);
};