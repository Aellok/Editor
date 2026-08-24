#include "FreeCamera.h"
#include "Platform/Windows/Renderer/DirectX12/DX12Camera.h"
void FreeCamera_LButtonDown(void* Parent, Mouse mouse)
{
	DX12Camera* cam = (DX12Camera*)Parent;
	if (cam->IsActive)
	{
		cam->IsRotating = true;
	}
}

void FreeCamera_LButtonUp(void* Parent, Mouse mouse)
{
	DX12Camera* cam = (DX12Camera*)Parent;
	if (cam->IsActive)
	{
		cam->IsRotating = false;
	}
}
void FreeCamera_Move(void* Parent, Mouse mouse) 
{
	DX12Camera* Cam = (DX12Camera*)Parent;
	if (Cam->IsActive && Cam->IsRotating)
	{
		Cam->AngleX += TO_RAD(mouse.dy) * 0.5;
		Cam->AngleY += TO_RAD(mouse.dx) * 0.5;
	}
}

void FreeCamera_RButtonDown(void* Parent, Mouse mouse) {}
void FreeCamera_RButtonUp(void* Parent, Mouse mouse) {}
void FreeCamera_MButtonDown(void* Parent, Mouse mouse) {}
void FreeCamera_MButtonUp(void* Parent, Mouse mouse) {}


void FreeCamera_KeyDown(void* Parent, u32 KeyCode)
{
	DX12Camera* cam = (DX12Camera*)Parent;
	if (cam->IsActive)
	{
		cam->Directions[0] = (KeyCode == 'A') ? true : cam->Directions[0];
		cam->Directions[1] = (KeyCode == 'D') ? true : cam->Directions[1];
		cam->Directions[2] = (KeyCode == 'W') ? true : cam->Directions[2];
		cam->Directions[3] = (KeyCode == 'S') ? true : cam->Directions[3];
	}
}
void FreeCamera_KeyUp(void* Parent, u32 KeyCode)
{
	DX12Camera* cam = (DX12Camera*)Parent;
	if (cam->IsActive)
	{
		cam->Directions[0] = (KeyCode == 'A') ? false : cam->Directions[0];
		cam->Directions[1] = (KeyCode == 'D') ? false : cam->Directions[1];
		cam->Directions[2] = (KeyCode == 'W') ? false : cam->Directions[2];
		cam->Directions[3] = (KeyCode == 'S') ? false : cam->Directions[3];
	}
}