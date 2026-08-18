#include "Engine.h"
#include "../Platform/Windows/Win32Window.h"
#include "System\Network\UDPClient.h"
#include "System\Network\UDPServer.h"

Engine GEngine;
u32 DefaultFontSizes[eTotal]
{
	10,12,14,16,18,20,24,28,32
};
Font* Engine::GetClosestFont(u32 Size)
{
	for(u32 i = 0; i < eTotal;i++)
	{
		if (Size <= DefaultFontSizes[i])
		{
			return &DefaultFont[i];
		}
	}
	return NULL;
}
void Engine::Init(EngineSpec spec)
{
	Global.Create(1024 * 1024 * 1024); // allocate 1 GB
	pWindow = (Window*)Global.Allocate( sizeof(Window));
	
#if SERVER_MODE
	pServer = (UDPServer*)Global.Allocate(sizeof(UDPServer));
#else
	pClient = (UDPClient*)Global.Allocate(sizeof(UDPClient));
#endif

	pRendererInterface = (DirectX12*)Global.Allocate(sizeof(DirectX12));
	pPipelineManager = (DX12PipelineManager*)Global.Allocate(sizeof(DX12PipelineManager));
	pObjManager3D = (ObjectManager3D*)Global.Allocate(sizeof(ObjectManager3D));
	pObjManager2D = (ObjectManager2D*)Global.Allocate(sizeof(ObjectManager2D));
	pPhysicsManager2D = (PhysicsManager2D*)Global.Allocate(sizeof(PhysicsManager2D));
	pNetworkManager = (NetworkManager*)Global.Allocate(sizeof(NetworkManager));

	pCamManager = (CameraManager*)Global.Allocate(sizeof(CameraManager));

	WindowSpec winSpec;
	winSpec.fullscreen = spec.fullscreen;
	winSpec.height = spec.height;
	winSpec.width = spec.width;
	winSpec.windowName = spec.windowName;

	pWindow->Create(winSpec);
#if !SERVER_MODE
	pPipelineManager->Init();
	pRendererInterface->OnInit(((Win32_Window*)pWindow->platformWindow)->hwnd, pWindow->width, pWindow->height);
	
#endif
	pObjManager3D->Init(pRendererInterface);
	pObjManager2D->Init(pRendererInterface,pRendererInterface->PipelineManager);
	pPhysicsManager2D->Init();
	pNetworkManager->Init();
#if !SERVER_MODE //Client
	pClient->Init(spec.ServerIP, spec.Port, spec.ServerPort);
	pCamManager->Init(&pWindow->mouseManager, &pWindow->keyboardManager);
	//pCamManager->SetCamera(pCamManager->AddCamera(45.0f * (PI / 180.0f), pWindow->width, pWindow->height, { 0.0,0.0f,-3.0f },  eFreeCam),eFreeCam);

	for (u32 i = 0; i < eTotal; i++)
	{
		DefaultFont[i].Init(pRendererInterface, &Global);
		DefaultFont[i].LoadFont(pRendererInterface, "Textures/Roboto-Black.ttf", DefaultFontSizes[i]);
		//DefaultFont[i].LoadFont(pRendererInterface, "Textures/CascadiaMono-VariableFont_wght.ttf", DefaultFontSizes[i]);
		//DefaultFont[i].LoadFont(pRendererInterface, "Textures/JetBrainsMono_Bold.ttf", DefaultFontSizes[i]);

	}

#else
	pServer->Init(spec.ServerInternalIP, spec.ServerIP, spec.ServerPort);
#endif
}
void Engine::StartFrame()
{
	FrameTimer.Start();
#if !SERVER_MODE
	if (((Win32_Window*)pWindow->platformWindow)->Resized)
	{
		pCamManager->Resize(((Win32_Window*)pWindow->platformWindow)->width, ((Win32_Window*)pWindow->platformWindow)->height);
		pRendererInterface->Resize(((Win32_Window*)pWindow->platformWindow)->width, ((Win32_Window*)pWindow->platformWindow)->height);
		((Win32_Window*)pWindow->platformWindow)->Resized = false;
	}

	pRendererInterface->MainCommandQueue.Reset(pPipelineManager->GetPipeline("Main3D"));
	
#endif;
}



void Engine::EndFrame(DX12CommandQueue* Queue)
{
	if (FrameCounter % 100 == 0)
	{
		sprintf_s(FPSStr, "FPS: %f", FPSAverage / 100);
		FPSAverage = 0;
		FrameCounter = 0;
	}
	FPSAverage += 1.0f / FrameTimer.LastTime;
	FrameCounter++;
#if !SERVER_MODE
	sprintf_s(FrameTimeStr, "Frame Time: %f", FrameTimer.LastTime);
	sprintf_s(MemoryStr, "Total Memory Useage %u / %u", Global.writePtr - Global.Data , Global.TotalSize);
	pRendererInterface->Present( Queue);
#endif
	FrameTimer.Stop();
}
void Engine::ProcessInputs()
{
	pWindow->ProccessMessages();
}
void Engine::ExecuteCommandList()
{
#if !SERVER_MODE
	ID3D12CommandList* CommandLists[1]{ pRendererInterface->MainCommandQueue.List };
	pRendererInterface->MainCommandQueue.CloseCommandList();
	pRendererInterface->MainCommandQueue.Execute(CommandLists,1);
	pRendererInterface->WaitForPreviousFrame();
#endif
}
f32 Engine::GetFrameTime()
{
	return FrameTimer.LastTime;
}