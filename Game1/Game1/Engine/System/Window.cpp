#include "Window.h"
#include "../Platform/Windows/Win32Window.h"
void Window::Create(WindowSpec spec)
{
	width = spec.width;
	height = spec.height;
	//TODO: if WINDOWS needs to be added if other platforms are added
	Win32_WindowSpec Win32_spec;
	Win32_spec.fullscreen = spec.fullscreen;
	Win32_spec.windowName = spec.windowName;
	Win32_spec.Width = spec.width;
	Win32_spec.Height = spec.height;
	Win32_spec.mouseManager = &mouseManager;
	Win32_spec.keyboardManager = &keyboardManager;
	platformWindow = Win32_CreateWindow(Win32_spec);
	((Win32_Window*)platformWindow)->GetClientSize(&width,&height);
}
void Window::ProccessMessages()
{
	((Win32_Window*)platformWindow)->ProccessMessages();
}
bool Window::Running()
{
	return !((Win32_Window*)platformWindow)->close;
}
void Window::GetClientSize(u32* Width,u32* Height)
{
	((Win32_Window*)platformWindow)->GetClientSize(Width,Height);
}