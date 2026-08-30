#include "KeyboardManager.h"
#include <memory>
void KeyboardManager::Init()
{
	CallbackList.Init(8, sizeof(KeyboardCallbacks*));

}
void KeyboardManager::Register(KeyboardCallbacks* NewCallback)
{
	CallbackList.Add(&NewCallback);
}
void KeyboardManager::OnKeyUp(u64 KeyCode)
{
	KeyboardCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(KeyboardCallbacks*,CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i]->OnKeyUp && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnKeyUp(Callbacks[i]->Parent, KeyCode);
		}
	}
}
void KeyboardManager::OnKeyDown(u64 KeyCode)
{
	KeyboardCallbacks** Callbacks = DYNAMIC_ARR_GET_CAST_DATA(KeyboardCallbacks*, CallbackList);
	for (u32 i = 0; i < CallbackList.elementCount; i++)
	{
		if (Callbacks[i]->OnKeyDown && Callbacks[i]->IsEnabled)
		{
			Callbacks[i]->OnKeyDown(Callbacks[i]->Parent, KeyCode);
		}
	}
}