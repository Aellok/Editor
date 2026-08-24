#include "Engine.h"
#include "Platform/Windows/Win32Window.h"
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
	
	if (spec.EnableNetworking)
	{
#if SERVER_MODE
		pServer = (UDPServer*)Global.Allocate(sizeof(UDPServer));
		pServer->Init(spec.ServerInternalIP, spec.ServerIP, spec.ServerPort);
#else
		pClient = (UDPClient*)Global.Allocate(sizeof(UDPClient));
		pClient->Init(spec.ServerIP, spec.Port, spec.ServerPort);
#endif
		pNetworkManager = (NetworkManager*)Global.Allocate(sizeof(NetworkManager));
		pNetworkManager->Init();
	}

	pRendererInterface = (DirectX12*)Global.Allocate(sizeof(DirectX12));
	pPipelineManager = (DX12PipelineManager*)Global.Allocate(sizeof(DX12PipelineManager));
	pObjManager2D = (ObjectManager*)Global.Allocate(sizeof(ObjectManager));
	pPhysicsManager2D = (PhysicsManager2D*)Global.Allocate(sizeof(PhysicsManager2D));
	
	pCamManager = (CameraManager*)Global.Allocate(sizeof(CameraManager));

	WindowSpec winSpec;
	winSpec.fullscreen = spec.fullscreen;
	winSpec.height = spec.height;
	winSpec.width = spec.width;
	winSpec.windowName = spec.windowName;

	pWindow->Create(winSpec);
	pPipelineManager->Init();
	pRendererInterface->OnInit(((Win32_Window*)pWindow->platformWindow)->hwnd, pWindow->width, pWindow->height);
	pObjManager2D->Init(pRendererInterface,pRendererInterface->PipelineManager);
	pPhysicsManager2D->Init();
	
	pCamManager->Init(&pWindow->mouseManager, &pWindow->keyboardManager);

	for (u32 i = 0; i < eTotal; i++)
	{
		DefaultFont[i].Init(pRendererInterface, &Global);
		DefaultFont[i].LoadFont(pRendererInterface, "Textures/Roboto-Black.ttf", DefaultFontSizes[i]);
	}

	Camera2D = pCamManager->AddCamera(45 * (PI / 180), pWindow->width, pWindow->height, { (float)0.0f,(float)0.0f,-6.0 }, eOrthoCam);
	Camera3D = pCamManager->AddCamera(45 * (PI / 180), pWindow->width, pWindow->height, { (float)0.0f,(float)0.0f,-6.0 }, eFreeCam);
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

	pRendererInterface->MainCommandQueue.Reset(pPipelineManager->GetPipeline3D("Main3D"));
	Object2DOnResizeBuffer OnResize;
	
	DX12Camera* Cam2D = pCamManager->GetCamera(Camera2D);
	DX12Camera* Cam3D = pCamManager->GetCamera(Camera2D);

	OnResize.Ortho = Cam2D->PMatrix;

	DX12Pipeline* pipeline2D = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, pPipelineManager->PipelineList2D);
	DX12Pipeline* pipeline3D = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, pPipelineManager->PipelineList3D);

	for (u32 i = 0; i < pPipelineManager->PipelineList2D.elementCount; i++)
	{
		pipeline2D[i].UpdateVSOnResize(&OnResize);
	}
	DefaultOnResizeBuffer OnResize3D;
	OnResize3D.CamPos = Cam3D->Position;
	OnResize3D.Proj = DirectX::XMMatrixTranspose(Cam3D->PMatrix);
	OnResize3D.View = DirectX::XMMatrixTranspose(Cam3D->VMatrix);

	//3d
	for (u32 i = 0; i < pPipelineManager->PipelineList3D.elementCount; i++)
	{
		pPipelineManager->GetPipeline3D(pipeline3D[i].PipelineName)->UpdateVSOnResize(&OnResize3D);
	}

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