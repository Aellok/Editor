#include "Win32Window.h"
#include "time.h"
#include "System/String.h"
#include "Application\Engine.h"
bool keys[255] = {false};
bool GetKey(u32 Key)
{
	return keys[Key];
}
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Win32_Window* window = (Win32_Window*)GetWindowLongPtr(hWnd, 0);
	
	if (!window)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	window->keyboardManager->LastlParam = lParam;
	switch (msg)
	{
		case WM_CREATE:
		{

			break;
		}
		case WM_SIZE:
		{
			RECT r;
			GetWindowRect(hWnd, &r);
			GetClientRect(hWnd, &r);
			window->width = r.right - r.left;
			window->height = r.bottom - r.top;
			window->Resized = true;
			
			break;
		}
		case WM_LBUTTONUP:
		{
			window->mouseManager->OnLRelease(window->mouse);
			break;
		}
		case WM_RBUTTONUP:
		{
			window->mouseManager->OnRRelease(window->mouse);
			break;
		}
		case WM_MBUTTONUP:
		{
			window->mouseManager->OnMRelease(window->mouse);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			window->mouseManager->OnLClick(window->mouse);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			window->mouseManager->OnRClick(window->mouse);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			window->mouseManager->OnMClick(window->mouse);
			break;
		}
		case WM_MOUSEMOVE:
		{
			POINT mousePos;
			GetCursorPos(&mousePos);
			ScreenToClient(hWnd, &mousePos);

			window->mouse.dx = mousePos.x - window->mouse.x;
			window->mouse.dy = mousePos.y - window->mouse.y;

			window->mouse.x = mousePos.x;
			window->mouse.y = mousePos.y;

			window->mouseManager->Move(window->mouse);
			break;
		}
		case WM_KEYDOWN:
		{
			window->keyboardManager->OnKeyDown(wParam);
			break;
		}
		case WM_KEYUP:
		{
			
			window->keyboardManager->OnKeyUp(wParam);
			if (wParam != VK_ESCAPE)
			{
				break;
			}
			
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_QUIT:
		{
			break;
		}
		case WM_DROPFILES:
		{
			
			WCHAR* Buffer;
			u32 FileCount = DragQueryFileW((HDROP)wParam, 0xFFFFFFFF,NULL,NULL);
			u32 BufferSize = DragQueryFileW((HDROP)wParam, 0, NULL, NULL) + 1;
			Buffer = (WCHAR*)calloc(BufferSize, sizeof(wchar_t*));
			DragQueryFileW((HDROP)wParam, 0, Buffer, BufferSize);

			window->mouseManager->DragDrop(window->mouse, WCharToChar(Buffer));

			free(Buffer);
			DragFinish((HDROP)wParam);
			
			break;
		}
		default:
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
	return S_OK;
}
void Win32_Window::GetClientSize(u32* Width, u32* Height)
{
	RECT r;
	GetClientRect(hwnd, &r);
	*Width = r.right - r.left;
	*Height = r.bottom - r.top;
}
void Win32_Window::ProccessMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			close = true;
			return;
		}
	}
}
Win32_Window* Win32_CreateWindow(Win32_WindowSpec spec)
{
	
	Win32_Window* window = (Win32_Window*)calloc(1,sizeof(Win32_Window));
	window->mouseManager = spec.mouseManager;
	window->mouseManager->Init();
	window->keyboardManager = spec.keyboardManager;
	window->keyboardManager->Init();
	window->messageLoop = false;
	window->close = false;
	window->windowName = spec.windowName;
	window->width = spec.Width;
	window->height = spec.Height;
#if !SERVER_MODE
	POINT mousePos;
	GetCursorPos(&mousePos);
	window->mouse = { (u32)mousePos.x,(u32)mousePos.y,0,0 };
	
	wchar_t WindowName[128];
	STR_TO_WSTR(spec.windowName, WindowName);

	WNDCLASSEX WindClass = { sizeof(WNDCLASSEX),CS_CLASSDC,MsgProc,0,sizeof(Win32_Window),GetModuleHandle(NULL),NULL,LoadCursor(NULL,IDC_ARROW),NULL,NULL,WindowName,NULL};
	RegisterClassEx(&WindClass);

	window->CurrentWindowInfo = spec.fullscreen ? (WS_POPUP | WS_VISIBLE) : WS_OVERLAPPEDWINDOW;
	
	RECT ScreenDim;
	HWND desktopWindow = GetDesktopWindow();
	GetWindowRect(desktopWindow, &ScreenDim);
	
	window->hwnd = CreateWindowA(window->windowName, window->windowName, window->CurrentWindowInfo, 0, 0, spec.fullscreen ? (ScreenDim.right - ScreenDim.left) : spec.Width, spec.fullscreen ? (ScreenDim.bottom - ScreenDim.top) : spec.Height, NULL, NULL, WindClass.hInstance, NULL);
	SetWindowLongPtrW(window->hwnd, 0, (LONG_PTR)window);
	DragAcceptFiles(window->hwnd,true);
	ShowWindow(window->hwnd, SW_SHOWDEFAULT);
	UpdateWindow(window->hwnd);
#endif
	
#if 1//_DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif
	srand(time(NULL));
	return window;
}