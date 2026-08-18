#pragma once
#include "windows.h"
#include "../../System/Types.h"
#include "../../System/MouseManager.h"
#include "../../System/KeyboardManager.h"

struct Win32_WindowSpec
{
	bool fullscreen;
	const char* windowName;
	u32 Width;
	u32 Height;
	MouseManager* mouseManager;
	KeyboardManager* keyboardManager;
};
struct Win32_Window
{
	const char* windowName;
	HWND hwnd;
	s32 CurrentWindowInfo;
	u32 height;
	u32 width;
	bool close;
	bool messageLoop;

	bool Resized = false;

	MouseManager* mouseManager;
	KeyboardManager* keyboardManager;

	Mouse mouse;
	void ProccessMessages();
	void GetClientSize(u32* Width, u32* Height);
};

Win32_Window* Win32_CreateWindow(Win32_WindowSpec spec);

bool GetKey(u32 Key);