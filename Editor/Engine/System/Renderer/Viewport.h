#pragma once

#include "Platform\Windows\Renderer\DirectX12\DX12Viewport.h"
#include "Platform\Windows\Renderer\DirectX12\DX12Camera.h"
#include "Platform\Windows\Renderer\DirectX12\DX12CommandQueue.h"
#include "Platform\Windows\Renderer\DirectX12\DX12Texture.h"
#include "System\MouseManager.h"
#include "System\KeyboardManager.h"
#include "System\Camera\CameraManager.h"
struct Viewport
{
	Vector Pos;
	Vector Dim;
	KeyboardManager KManager;
	MouseManager MManager;
	MouseCallbacks ViewportMouseCallbacks;
	KeyboardCallbacks ViewportKeyboardCallbacks;

	DX12Viewport viewport;
	CameraIndex Camera2D;
	CameraIndex Camera3D;

	DX12Camera* CurrentCamera;

	DX12CommandQueue Queue;
	DX12Pipeline* Pipeline;

	DX12RenderTarget RenderTarget;
	DX12Texture Texture;

	CameraManager CManager;

	DX12PipelineManager PipelineManager;

	bool Focused;
	void (*FillCommandQueue)();


	void Initialize(ID3D12Device* Device, f32 x, f32 y, f32 ScreenWidth, f32 ScreenHeight,const char* Name);
	void SetPipeline(DX12Pipeline* pipeline);
	void StartRender(bool RenderToTexture);
	void StopRender();
	void Bind();

};