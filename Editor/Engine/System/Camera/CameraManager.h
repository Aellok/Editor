#pragma once
#include "System/Types.h"
#include "Platform/Windows/Renderer/DirectX12/DX12Camera.h"
#include "System/MouseManager.h"
#include "System/KeyboardManager.h"
#include "System\Memory\DynamicArray.h"
enum CameraType
{
	eOrthoCam,
	eFreeCam,
	eObjectCam,
	eCamCount
};
struct CameraIndex
{
	u32 Index;
	CameraType Type;
};
struct CameraManager
{
	CameraIndex CurrentCamIndex;
	//DX12Camera
	DynamicArray CameraList[eCamCount];

	MouseManager* mouseManager;
	KeyboardManager* keyboardManager;


	void Init(MouseManager* pMouseManager, KeyboardManager* pKeyboardManager);
	void SetCamera(CameraIndex Index);
	CameraIndex AddCamera(f32 FOV, u32 ScreenWidth, u32 ScreenHeight, Vector pos, CameraType Type);
	void Resize(u32 width, u32 height);
	void Update(f32 FrameTime);
	//Dont save this ptr.
	DX12Camera* GetCurrentCamera();
	DX12Camera* GetCamera(CameraIndex Index);
};