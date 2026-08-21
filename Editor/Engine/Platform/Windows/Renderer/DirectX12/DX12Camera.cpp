#include "DX12Camera.h"
#include "System\Camera\FreeCamera.h"
void DX12Camera::Initialize(f32 FieldOfView, u32 ScreenWidth, u32 ScreenHeight, Vector Pos,bool IsOrthographic)
{
	memset(Directions, 0, 4);
	fov = FieldOfView;
	IsRotating = false;
	IsActive = false;
	IsOrtho = IsOrthographic;
	Position = Pos;
	AngleX = 0;
	AngleY = 0;
	Rotation = DirectX::XMQuaternionRotationRollPitchYaw(AngleX, AngleY, 0);
	Forward  = DirectX::XMVector3Rotate({0,0,1} , Rotation);
	Left     = DirectX::XMVector3Rotate({-1,0,0}, Rotation);
	Speed = 10;
	if (!IsOrtho)
	{
		VMatrix = DirectX::XMMatrixLookAtLH(Position, DirectX::XMVectorAdd(Position, Forward), { 0.0f, 1.0f, 0.0f, 0.0f });
		PMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, (f32)ScreenWidth / (f32)ScreenHeight, 0.1f, 1000);
		CREATE_MOUSE_CALLBACK(this, MCallbacks, FreeCamera);
		CREATE_KEYBOARD_CALLBACK(this, KCallbacks, FreeCamera);
	}
	else
	{
		PMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0, ScreenWidth, ScreenHeight, 0, 0.0f, 100.0f));
		memset(&MCallbacks, 0, sizeof(MCallbacks));
		memset(&KCallbacks, 0, sizeof(KCallbacks));
	}
}
void DX12Camera::Update(f32 FrameTime)
{
	if (!IsOrtho)
	{
		Rotation = DirectX::XMQuaternionRotationRollPitchYaw(AngleX, AngleY, 0);
		Forward = DirectX::XMVector3Rotate({ 0,0,1 }, Rotation);
		Left = DirectX::XMVector3Rotate({ -1,0,0 }, Rotation);

		Vector Vel = DirectX::XMVectorAdd(DirectX::XMVectorScale(Left, (float)(Directions[0] - Directions[1])),
			DirectX::XMVectorScale(Forward, (Directions[2] - Directions[3])));

		Vel = DirectX::XMVector3Normalize(Vel);
		Position = DirectX::XMVectorAdd(Position, DirectX::XMVectorScale(Vel, Speed * FrameTime));

		VMatrix = DirectX::XMMatrixLookAtLH(Position, DirectX::XMVectorAdd(Position, Forward), { 0.0f, 1.0f, 0.0f, 0.0f });
		// in the case where this is realloced due to dynamicArray this need to be corrected.
	}
	
	MCallbacks.Parent = this;
	KCallbacks.Parent = this;
}
void DX12Camera::Resize(u32 ScreenWidth, u32 ScreenHeight)
{
	if (!IsOrtho)
	{
		PMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, (f32)ScreenWidth / (f32)ScreenHeight, 0.1f, 1000);
	}
	else
	{
		PMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0, ScreenWidth, ScreenHeight, 0, 0.0f, 100.0f));
	}
}