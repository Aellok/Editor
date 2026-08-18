#include "DirectX.h"
#include "VertexDef.h"
#include "DX12CommandQueue.h"
#include "Application\Engine.h"
#include "RenderDefines.h"
//TODO: Implement commandqueue class like in https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/CommandListManager.cpp
bool DirectX12::OnInit(HWND hwnd, u32 Width, u32 Height)
{
	vport.Init(0.0f, 0.0f, Width, Height);
	frameIndex = 0;
	rtvDescriptorSize = 0;

	if (!LoadPipeline(hwnd, Width, Height))
	{
		return false;
	}
	//TODO: need to convert the main screen to us viewport like shader editor.
	PipelineManager = GEngine.pPipelineManager;
	LoadPipelines();
	return true;
}
bool DirectX12::LoadPipeline(HWND hwnd, u32 Width, u32 Height)
{
	UINT dxgiFactoryFlags = 0;
	//Setup debugging
#if defined(_DEBUG)
	ID3D12Debug* debugController;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

#endif
	//Create device

	if (FAILED(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
	{
		return false;
	}

	
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&InfoQueue))))
	{
		InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		InfoQueue->SetMuteDebugOutput(FALSE);
	}

	//Create factory
	IDXGIFactory4* factory;
	if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags,IID_PPV_ARGS(&factory))))
	{
		return false;
	}
	//CommandQueue = (DX12CommandQueue*)GEngine.Global.Allocate(sizeof(DX12CommandQueue));
	MainCommandQueue.Initialize(device);
	LoadCommandQueue.Initialize(device);
	ScratchCommandQueue.Initialize(device);

	//Create swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Width = Width;
	swapChainDesc.Height = Height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	IDXGISwapChain1* sc;
	if (FAILED(factory->CreateSwapChainForHwnd(MainCommandQueue.Queue,hwnd, &swapChainDesc,nullptr,nullptr, &sc)))
	{
		return false;
	}
	if (FAILED(sc->QueryInterface(IID_PPV_ARGS(&swapChain))))
	{
		return false;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	//Create descriptor heaps
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap))))
	{
		return false;
	}

	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap2))))
	{
		return false;
	}
	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap3))))
	{
		return false;
	}
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC TextureHeapDesc = {};
	TextureHeapDesc.NumDescriptors = 1024;
	TextureHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	TextureHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	
	device->CreateDescriptorHeap(&TextureHeapDesc, IID_PPV_ARGS(&TextureHeap));
	
	TextureHeapDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	for (u32 i = 0; i < 2; i++)
	{
		ID3D12Resource* Resource;
		if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&Resource))))
		{
			return false;
		}
		renderTargets[i] = (DX12RenderTarget*)GEngine.Global.Allocate(sizeof(DX12RenderTarget));
		renderTargets[i]->Initialize(device, Resource, D3D12_RESOURCE_STATE_PRESENT,rtvHeap, i, {0,0,0,1});
	}
	
	return true;
}

void DirectX12::LoadPipelines()
{
	
	PipelineManager->AddPipeline(device,"Main3D", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/Main3DPipeline.desc",true,false);
	PipelineManager->AddPipeline(device, "NewMain3D", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/NewMain3D.desc", true, false);
	PipelineManager->AddPipeline(device, "BasicMain3D", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/BasicMain3DPipeline.desc", true, false);
	PipelineManager->AddPipeline(device,"Instance3D", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/InstancePipeline.desc", true,false);

	PipelineManager->AddPipeline(device,"InstnaceOrtho", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/InstanceOrthoPipeline.desc", true,false);
	PipelineManager->AddPipeline(device,"Ortho", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/OrthoPipeline.desc", true,false);
	PipelineManager->AddPipeline(device,"Font", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/FontPipeline.desc", false,false);
	PipelineManager->AddPipeline(device, "Font2", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/Font2Pipeline.desc", false, false);
}

void DirectX12::WaitForPreviousFrame()
{
	MainCommandQueue.WaitForGPU();

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineManager->PipelineList);
	for (u32 i = 0; i < PipelineManager->PipelineList.elementCount; i++)
	{
		Pipelines->FrameIndex = frameIndex;
	}
}

void DirectX12::StartRender2D()
{
	ID3D12DescriptorHeap* descHeaps[1] = { TextureHeap };
	MainCommandQueue.SetDescriptorHeaps(1, descHeaps);
	
	u32 Index = PipelineManager->GetPipelineIndex("Ortho");
	PipelineManager->SetPipeline(&MainCommandQueue, frameIndex, Index,false);
	
	renderTargets[frameIndex]->SetTarget(&MainCommandQueue, true);

}
void DirectX12::StartRender3D(Vector BackgroundColor)
{
	renderTargets[frameIndex]->Reset(&MainCommandQueue);

	u32 Index = PipelineManager->GetPipelineIndex("Main3D");
	PipelineManager->SetPipeline(&MainCommandQueue, frameIndex, Index,true);

	renderTargets[frameIndex]->SetTarget(&MainCommandQueue,true);
	//MainCommandQueue.SetRenderTarget(, CD3DX12_CPU_DESCRIPTOR_HANDLE(pipeline.dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart()), true);
	vport.SetViewport(&MainCommandQueue);

	MainCommandQueue.SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void DirectX12::Present(DX12CommandQueue* Queue)
{
	
	renderTargets[frameIndex]->Present(Queue);
	
	ID3D12CommandList* CommandLists[1]{ MainCommandQueue.List };
	MainCommandQueue.CloseCommandList();
	
	MainCommandQueue.Execute(CommandLists,1);
	
	// Present the frame.
	if (FAILED(swapChain->Present(1, 0)))
	{
		return;
	}

	WaitForPreviousFrame();

	PrintMessages();
}
void DirectX12::Resize( u32 width, u32 height)
{
	WaitForPreviousFrame();

	renderTargets[0]->Destroy();
	renderTargets[1]->Destroy();
	rtvHeap->Release();
	swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM,NULL );
	
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	//Create descriptor heaps
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap))))
	{
		assert(1);
	}

	for (u32 i = 0; i < 2; i++)
	{
		ID3D12Resource* Resource;
		if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&Resource))))
		{
			return;
		}
		renderTargets[i]->Initialize(device, Resource, D3D12_RESOURCE_STATE_PRESENT, rtvHeap, i,{ 0,0,0,1 });
	}
	
	vport.Resize(width, height);
}
void DirectX12::PrintMessages()
{
	UINT64 count = InfoQueue->GetNumStoredMessages();

	for (UINT64 i = 0; i < count; i++)
	{
		SIZE_T size = 0;
		InfoQueue->GetMessage(i, nullptr, &size);

		D3D12_MESSAGE* msg = (D3D12_MESSAGE*)malloc(size);
		InfoQueue->GetMessage(i, msg, &size);

		printf(msg->pDescription);
		printf("\n");

		free(msg);
	}

	InfoQueue->ClearStoredMessages();
}