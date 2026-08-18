
#include "Player.h"
#include "Application\Engine.h"
#include "Application\Weapons\Bullet.h"
#include "Application\PhysicsObjectTypes.h"
#include "System\Network\UDPClient.h"
#include "Application\Network\ClientEvents.h"
void Player_KeyDown(void* Parent, u32 Key)
{
	Player* player = (Player*)Parent;
	switch (Key)
	{
		case 'A':
		{
			player->InputState[0] = true;
			break;
		}
		case 'D':
		{
			player->InputState[1] = true;
			break;
		}
		case 'W':
		{
			player->InputState[2] = true;
			break;
		}
		case 'S':
		{
			player->InputState[3] = true;
			break;
		}
		case VK_SPACE:
		{
			static int a = 0;
			a++;
			NE_PlayerShoot shootEvent;
			shootEvent.PlayerUUID = player->Model->NetworkID;
			shootEvent.DEBUGNumber = a;

			GEngine.pClient->Send(&shootEvent, sizeof(NE_PlayerShoot));
		}
	}
}
void Player_KeyUp(void* Parent,u32 Key)
{
	Player* player = (Player*)Parent;
	switch(Key)
	{
		case 'A': //left
		{
			player->InputState[0] = false;
			break;
		}
		case 'D': // right
		{
			player->InputState[1] = false;
			break;
		}
		case 'W': // up
		{
			player->InputState[2] = false;
			break;
		}
		case 'S': // down
		{
			player->InputState[3] = false;
			break;
		}

	}
}
void Player_LButtonDown(void* Parent, Mouse pMouse)
{
	
}
void Player_RButtonDown(void* Parent, Mouse pMouse)
{

}
void Player_MButtonDown(void* Parent, Mouse pMouse)
{

}
void Player_LButtonUp(void* Parent, Mouse pMouse)
{

}
void Player_RButtonUp(void* Parent, Mouse pMouse)
{

}
void Player_MButtonUp(void* Parent, Mouse pMouse)
{

}
void Player_Move(void* Parent, Mouse pMouse)
{
	Player* player = (Player*)Parent;
	player->MousePos[0] = pMouse.x;
	player->MousePos[1] = pMouse.y;
}
void Player_OnCollide(void* Parent, PhysicsObject2D* Other)
{
	Player* p = (Player*)Parent;

}
void Player::Init(Vector Pos,u32 width,u32 height,bool IsLocalPlayer,bool DeferredAdd)
{
	bIsLocalPlayer = IsLocalPlayer;
	Window* pWindow = GEngine.pWindow;
	
	ObjectDesc desc = { this,Pos, { (f32)width,(f32)height }, { 0,0,0 }, {0,1,0,1},"Centered" ,NULL,"Ortho",false,false,true,true};

	Model = GEngine.pObjManager2D->AddObject(desc);
	
	PhysObj = &GEngine.pPhysicsManager2D->ObjectList[Model->PhysicsID];
	PhysObj->OnCollide = Player_OnCollide;
	PhysObj->ID = PhysPlayer;

	NetworkObj = GEngine.pNetworkManager->FindObject(Model->NetworkID);
#if !SERVER_MODE
	if (bIsLocalPlayer)
	{
		CREATE_KEYBOARD_CALLBACK(this, KeyboardCB, Player);
		CREATE_MOUSE_CALLBACK(this, MouseCB, Player);
		pWindow->keyboardManager.Register(&KeyboardCB);
		pWindow->mouseManager.Register(&MouseCB);
	}
#endif
	
}
void Player::Update()
{
	if (bIsLocalPlayer)
	{
		Vector Dir = { (InputState[0] * -1) + (InputState[1]),(InputState[2] * -1) + InputState[3],0,0 };
		ForwardDir = DirectX::XMVector2Normalize({ MousePos[0] - Model->Pos.m128_f32[0],MousePos[1] - Model->Pos.m128_f32[1],0,0 });


		DirectX::XMStoreFloat4(&PhysObj->Velocity, DirectX::XMVectorScale(Dir, Speed));

		Rotation = GetAlignmentRotation_2D(ForwardDir, { 0,1 });

		Model->SetRotation(Rotation);
	}
	
	for (u32 i = 0; i < BulletCount; i++)
	{
		BulletList[i].Update();
	}
}
void Player::Draw()
{
	Model->Draw();
	for (u32 i = 0; i < BulletCount; i++)
	{
		BulletList[i].Draw();
	}
}
void Player::Shoot(u64 UUID)
{
	BulletList[BulletCount].Init(Model->Pos, Rotation, { 2,4 },1000);
	BulletList[BulletCount].Obj->NetworkID = GEngine.pNetworkManager->AddObject(BulletList[BulletCount].PhysObj, UUID);
	BulletCount++;
}