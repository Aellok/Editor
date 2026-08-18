#pragma once
#include "Types.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
struct WindowSpec
{
	const char* windowName;
	bool fullscreen;
	u32 width;
	u32 height;
};
struct Window
{
	const char* windowName;
	bool fullscreen;
	void* platformWindow;
	u32 width;
	u32 height;
	
	MouseManager mouseManager;
	KeyboardManager keyboardManager;

	bool Running();
	void Create(WindowSpec spec);
	void ProccessMessages();
	void GetClientSize(u32* Width, u32* Hieght);
};