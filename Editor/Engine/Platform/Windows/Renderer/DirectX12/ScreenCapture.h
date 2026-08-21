#pragma once

#include "DX12.h"

#include "d3d11on12.h"
#include "d3d11.h"
struct ScreenCapture
{
	IDXGIAdapter1* Adapter;
	IDXGIOutput* DXGIOutput;
	ID3D12Device* DX12Device;

	IDXGIOutputDuplication* OutputDup;
	IDXGIFactory4* Factory;
	
	ID3D11Device* DX11Device;
	ID3D11DeviceContext* DX11Context;
	ID3D12CommandQueue* commandQueue;
	ID3D11Texture2D* stagingTexture;

	IDXGIOutput1* DXGIOutput1;
	IDXGIDevice* DXGIDevice;

	DXGI_OUTPUT_DESC DXGIOutputDesc;
	void* TextureData;
	void Init();
	bool GetFrame(void** Data,u32 &Size);
};