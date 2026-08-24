#include "CameraManager.h"
#include "FreeCamera.h"

void CameraManager::Init(MouseManager* pMouseManager, KeyboardManager* pKeyboardManager)
{
	mouseManager = pMouseManager;
	keyboardManager = pKeyboardManager;

	for (u32 i = 0; i < eCamCount; i++)
	{
		CameraList[i].Init(4, sizeof(DX12Camera));
	}
	
	CurrentCamIndex.Index = 0;
	CurrentCamIndex.Type = (CameraType)0;
}
void CameraManager::SetCamera(CameraIndex Index)
{
	DX12Camera* Cams = DYNAMIC_ARR_GET_CAST_DATA(DX12Camera, CameraList[Index.Type]);
	if (Index.Index < CameraList[Index.Type].elementCount)
	{
		CurrentCamIndex.Type = Index.Type;
		CurrentCamIndex.Index = Index.Index;
		Cams[Index.Index].IsActive = true;
		return;
	}
}
CameraIndex CameraManager::AddCamera(f32 FOV,u32 ScreenWidth,u32 ScreenHeight,Vector pos, CameraType Type)
{
	DX12Camera camera;

	camera.Initialize(FOV, ScreenWidth, ScreenHeight, pos,Type == eOrthoCam);
	
	CameraList[Type].Add(&camera);
	DX12Camera* Cam = &(DYNAMIC_ARR_GET_CAST_DATA(DX12Camera, CameraList[Type]))[CameraList[Type].elementCount - 1];
	for (u32 i = 0; i < CameraList[Type].elementCount; i++)
	{
		Cam[i].MCallbacks.Parent = &Cam[i];
		Cam[i].KCallbacks.Parent = &Cam[i];
	}
	
	mouseManager->Register(&Cam->MCallbacks);
	keyboardManager->Register(&Cam->KCallbacks);
	
	return { CameraList[Type].elementCount - 1,Type };
}
void CameraManager::Resize(u32 width, u32 height)
{
	if (width == 0 || height == 0)
	{
		return;
	}
	for (u32 Type = 0; Type < eCamCount;Type++)
	{
		DX12Camera* Cams = DYNAMIC_ARR_GET_CAST_DATA(DX12Camera, CameraList[Type]);
		for (u32 i = 0;i < CameraList[Type].elementCount; i++)
		{
			Cams[i].Resize(width, height);
			Cams[i].Update(0.0f);
		}
	}
}
void CameraManager::Update(f32 FrameTime)
{
	for (u32 Type = 0; Type < eCamCount; Type++)
	{
		DX12Camera* Cams = DYNAMIC_ARR_GET_CAST_DATA(DX12Camera, CameraList[Type]);
		for (u32 i = 0; i < CameraList[Type].elementCount; i++)
		{
			Cams[i].Update(FrameTime);
		}
	}
}
DX12Camera* CameraManager::GetCurrentCamera()
{
	DX12Camera* CamList = DYNAMIC_ARR_GET_CAST_DATA(DX12Camera, CameraList[CurrentCamIndex.Type]);
	return &CamList[CurrentCamIndex.Index];
}

DX12Camera* CameraManager::GetCamera(CameraIndex Index)
{
	DX12Camera* CamList = DYNAMIC_ARR_GET_CAST_DATA(DX12Camera, CameraList[Index.Type]);
	return &CamList[Index.Index];
}