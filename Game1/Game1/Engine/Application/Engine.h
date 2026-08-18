#pragma once


#include "Platform/Windows/Renderer/DirectX12/DirectX.h"
#include "System/Camera/CameraManager.h"
#include "System/Renderer/ObjectManager3D.h"
#include "System/Renderer/ObjectManager2D.h"
#include "System/Window.h"
#include "System/Types.h"
#include "System/Utils/Timer.h"
#include "System/Memory/Arena.h"
#include "System/Physics/PhysicsManager2D.h"
#include "System/Network/NetworkManager.h"
#include "Platform/Windows/Renderer/DirectX12/DX12PipelineManager.h"

#define SERVER_MODE 0

struct UDPServer;
struct UDPClient;

struct EngineSpec
{
	const char* windowName;
	bool fullscreen;
	u32 width;
	u32 height;
	
	
	u32 Port;
	u32 ServerPort;

	const char* ServerInternalIP;
	//External Server IP
	const char* ServerIP;
};


enum DefaultFontTypes
{
	eFont10,
	eFont12,
	eFont14,
	eFont16,
	eFont18,
	eFont20,
	eFont24,
	eFont28,
	eFont32,
	eTotal
};
struct Engine
{
	MemoryArena Global;
	
	Window* pWindow;
	
	DirectX12* pRendererInterface;
	DX12PipelineManager* pPipelineManager;
	ObjectManager3D* pObjManager3D;
	ObjectManager2D* pObjManager2D;
	PhysicsManager2D* pPhysicsManager2D;
	
	//Network
	NetworkManager* pNetworkManager;
	UDPClient* pClient = NULL;
	UDPServer* pServer = NULL;
	CameraManager* pCamManager;

	Timer FrameTimer;
	u32 FrameCounter = 0;

	f32 FPSAverage = 0;

	char FrameTimeStr[255] = { 0 };
	char FPSStr[255] = {0};
	char MemoryStr[255] = {0};

	Font DefaultFont[eTotal];

	void Init(EngineSpec spec);

	void ProcessInputs();
	void StartFrame();
	void EndFrame(DX12CommandQueue* Queue);
	void ExecuteCommandList();
	f32 GetFrameTime();
	Font* GetClosestFont(u32 Size);

};

#ifndef ENGINE_H
#define ENGINE_H
extern Engine GEngine;
#endif