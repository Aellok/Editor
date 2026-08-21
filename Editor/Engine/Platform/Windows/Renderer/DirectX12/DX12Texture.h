#pragma once
#include "DirectX.h"
struct DX12Texture
{
	const char* TextureName;
	u32 TextureOffset;
	D3D12_CPU_DESCRIPTOR_HANDLE TextureHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE TextureHandleGPU;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* Layouts;
	
	DirectX12* DX12;

	DX12Resource TextureBuffer;
	
	ID3D12DescriptorHeap* mainDescriptorHeap;
	
	DXGI_FORMAT Format;

	bool hasAlpha;
	bool Initialized = false;

	void Initialize(DX12CommandQueue* Queue, DirectX12* dx12, const char* Name, u32 Width, u32 Height, DXGI_FORMAT DataFormat, bool HasAlpha,bool AllowRT, Vector ClearColor);
	
	void InitializeRenderTargetTexture();

	void InitFromFile(DX12CommandQueue* Queue,DirectX12* DX12, const char* Name);
	void InitFromData(DX12CommandQueue* Queue, DirectX12* DX12, const char* Name, void* Data, u32 Width, u32 Height, DXGI_FORMAT Size,bool HasAlpha);
	void SetTexture(DX12CommandQueue* Queue);
	void Update(DX12CommandQueue* Queue, void* TextureData,u32 Width,u32 Height);
	
	void CreateTextureArray(DX12CommandQueue* Queue,void* TextureList, u32 TextureCount,u32 Width,u32 Height);

};