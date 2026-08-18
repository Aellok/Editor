#include "../Engine/System/Types.h"
#include "../Engine/System/Window.h"
#include "../Engine/System/Renderer/Font/FontRenderer.h"
#include "../Engine/Platform/Windows/Renderer/DirectX12/DX12Object2D.h"

#include "../Engine/Application/Engine.h"

#include "Engine\Platform\Windows\Renderer\DirectX12\ScreenCapture.h"
#include "GameState.h"
#include "Enemies\MeleeEnemy.h"
#include "System\Physics\AABB2D.h"
#include "Application\Enemies\EnemyManager.h"
#include "Application\Barriers\Barrier.h"
#include "Application\Network\Server.h"
#include "Application\Network\ClientEvents.h"
#include "Application\Network\ServerEvents.h"
#include "System\Utils\Hash.h"
#include "Platform\Windows\Renderer\DirectX12\DX12Compute.h"
#include "Application\Editor\Editor.h"
#include "System\Renderer\Font\String2D.h"
#include "Application\UI\Button.h"
#include "System\Renderer\Viewport.h"
#include "Platform\Windows\Utils\MonitorScale.h"
#define SE 1
void global_MouseMove(void* parent,Mouse mouse)
{
	Vector* Pos = (Vector*)parent;
	Pos->m128_f32[0] = min( mouse.x ,2560);
	Pos->m128_f32[1] = min( mouse.y ,1440);
}
struct MultiplayerGame {
	GameState gs;
	Barrier b;
	Barrier b2;
	DX12Compute comp;
	NE_ConnectToServer cts;
	Client c;
	AppServer server;
	void Setup();
	void Run();
};
void MultiplayerGame::Setup()
{
	gs.Initialize();
	b.Init({ 500, 500 }, { 100,50 });
	b2.Init({ 800, 800 }, { 100,50 });
#if SERVER_MODE
	
	server.Init(&gs, "192.168.0.10", "86.15.55.249", 53000);
#else
	//gs.SpawnPlayer(gs.SpawnLocations[0], -1, true);
	c.Init(&gs, "86.15.55.249", 53995, 53000, false);

	c.Send(&cts, sizeof(cts));
	
	comp.Initialize(GEngine.pRendererInterface, "Engine/Platform/Windows/Renderer/DirectX12/HLSL/ComputeShader.cso");
#endif
}
void MultiplayerGame::Run()
{
	c.gs->RunCommands();
	b.Update();
	b2.Update();
	gs.Update();

	GEngine.pPhysicsManager2D->Update(GEngine.GetFrameTime());
#if !SERVER_MODE

	b.Draw();
	b2.Draw();
	gs.Draw();

	GEngine.pRendererInterface->StartRender3D({ 0,0,0,0 });

	comp.Execute(&GEngine.pRendererInterface->MainCommandQueue);

	GEngine.pRendererInterface->StartRender2D();

	//GEngine.pObjManager2D->Draw();
#endif
}

void ApplicationMain()
{
	EngineSpec spec;
	spec.fullscreen = false;
	spec.windowName = "Tower Defence";
	spec.width = GetSystemMetrics(SM_CXSCREEN);
	spec.height = GetSystemMetrics(SM_CYSCREEN);
	spec.ServerIP = "192.168.0.10";
	spec.ServerInternalIP = "192.168.0.10";
	spec.Port = 53995;
	spec.ServerPort = 53000;

	GEngine.Init(spec);

	MouseCallbacks callbacks = { 0 };
	Vector MousePos;
	callbacks.Parent = &MousePos;
	callbacks.OnMove = global_MouseMove;
	GEngine.pWindow->mouseManager.Register(&callbacks);
#if !SE
	MultiplayerGame game;
	game.Setup();
#else
	Editor editor;
	editor.Init();
#endif	

	//String2D *FPSString = GEngine.pObjManager2D->AddString("Hello World         ", { 0,0 }, 32);
	//String2D *FrameTimeString = GEngine.pObjManager2D->AddString("Hello World       ", { 0,32 }, 32);
	//String2D *MemoryString = GEngine.pObjManager2D->AddString("Hello World         ", { 0,64 }, 32);
	String2D* TempString = GEngine.pObjManager2D->AddString("TEMP", { 0,100 }, 32,"Font");
	GEngine.ExecuteCommandList();

	while (GEngine.pWindow->Running())
	{
		GEngine.ProcessInputs();
		GEngine.StartFrame();
#if SE

		//GEngine.pPhysicsManager2D->Update(GEngine.GetFrameTime());
		GEngine.pRendererInterface->StartRender3D({ 0,0,0,0 });

		GEngine.pRendererInterface->StartRender2D();
		
		editor.Update();
		// Draws the Shader Editor. readt for the GEngine ObjectManager Draw call
		editor.Draw(); 
		
		//string->Visible = true;
		Object2DOnResizeBuffer OnResize;
		OnResize.Ortho = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0, GEngine.pWindow->width, GEngine.pWindow->height, 0, 0.0f, 100.0f));

		GEngine.pPipelineManager->GetPipeline("Ortho")->UpdateVSOnResize(&OnResize);
		GEngine.pPipelineManager->GetPipeline("Font")->UpdateVSOnResize(&OnResize);

		//GEngine.pObjManager2D->UpdateString(FPSString,GEngine.FPSStr,32);
		//GEngine.pObjManager2D->UpdateString(FrameTimeString, GEngine.FrameTimeStr, 32);
		//GEngine.pObjManager2D->UpdateString(MemoryString, GEngine.MemoryStr, 32);

		//FPSString->Draw();
		//FrameTimeString->Draw();
		//MemoryString->Draw();

		GEngine.pObjManager2D->Draw(&GEngine.pRendererInterface->MainCommandQueue);
#else
		game.Run();
#endif
		GEngine.EndFrame(&GEngine.pRendererInterface->MainCommandQueue);
	}
}