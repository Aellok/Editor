#pragma once
#define WIN32_LEAN_AND_MEAN

//#include <d3d12.h>
#include "System/Types.h"
#include "DX12Pipeline.h"
#include "DX12.h"
#include "Windows.h"
#include "DX12CommandQueue.h"
#include "DX12Resource.h"
#include "DX12RenderTarget.h"
#include "DX12Viewport.h"
#include "DX12PipelineManager.h"
#include "DX12Camera.h"

#define D3DCOMPILE_DEBUG 1


struct DirectX12
{
	ID3D12Device* device;
	DX12CommandQueue MainCommandQueue;
	DX12CommandQueue LoadCommandQueue;

	DX12CommandQueue ScratchCommandQueue;

	IDXGISwapChain3* swapChain;
	ID3D12DescriptorHeap* rtvHeap;
	ID3D12DescriptorHeap* rtvHeap2;
	ID3D12DescriptorHeap* rtvHeap3;
	DX12RenderTarget* renderTargets[2];

	DX12Viewport vport;

	u32 frameIndex;
	u32 rtvDescriptorSize;
	ID3D12Resource* pixelShader;

	CD3DX12_RESOURCE_BARRIER barrier;
	u32 TextureHeapDescriptorCount = 0;
	u32 TextureHeapDescriptorSize;
	ID3D12DescriptorHeap* TextureHeap;

	ID3D12InfoQueue* InfoQueue = nullptr;

	DX12PipelineManager* PipelineManager;

	void PrintMessages();
	bool OnInit(HWND hwnd, u32 Width, u32 Height);
	bool LoadPipeline(HWND hwnd, u32 Width, u32 Height);
	void LoadPipelines();
	void WaitForPreviousFrame();
	void StartRender3D(Vector Background);
	void StartRender2D();
	void Resize(u32 width, u32 height);

	void Present( DX12CommandQueue* Queue);
};


