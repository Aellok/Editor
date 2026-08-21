#include "DX12Texture.h"
#include "System/Utils/PNG.h"
#include "Application\Engine.h"
#include "RenderDefines.h"
void DX12Texture::InitializeRenderTargetTexture()
{
	
}
void DX12Texture::CreateTextureArray(DX12CommandQueue* Queue, void* TextureList,u32 TextureCount, u32 Width, u32 Height)
{
	DX12 = GEngine.pRendererInterface;
	D3D12_RESOURCE_DESC TextureDesc = {};
	TextureDesc.MipLevels = 1;
	TextureDesc.DepthOrArraySize = TextureCount;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	
	u64 size = 0;
	Layouts = (D3D12_PLACED_SUBRESOURCE_FOOTPRINT*)malloc(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) * TextureCount);
	DX12->device->GetCopyableFootprints(&TextureDesc, 0, TextureCount, 0, Layouts, nullptr, nullptr, &size);

	TextureBuffer.Initialize(GEngine.pRendererInterface->device, TextureDesc, size, D3D12_RESOURCE_STATE_COMMON, {});

	for (u32 i = 0; i < TextureCount; i++)
	{
		D3D12_SUBRESOURCE_DATA textureData;
		
		textureData.RowPitch = Width * 4;
		textureData.SlicePitch = Width * Height * 4;
		
		u64 offset = Layouts[i].Offset;
		textureData.pData = ((u8*)TextureList) + Width * Height * 4 * i;
		TextureBuffer.UpdateSubresource(Queue,offset, i, &textureData);
	}

	TextureBuffer.Transition(Queue, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	TextureOffset = (DX12->TextureHeapDescriptorCount * DX12->TextureHeapDescriptorSize);

	TextureHandleCPU.ptr = DX12->TextureHeap->GetCPUDescriptorHandleForHeapStart().ptr + TextureOffset;
	TextureHandleGPU.ptr = DX12->TextureHeap->GetGPUDescriptorHandleForHeapStart().ptr + TextureOffset;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = TextureCount;
	srvDesc.Texture2DArray.PlaneSlice = 0;
	srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;

	DX12->device->CreateShaderResourceView(TextureBuffer.Resource, &srvDesc, TextureHandleCPU);
	DX12->TextureHeapDescriptorCount++;
}
void DX12Texture::Initialize(DX12CommandQueue* Queue, DirectX12* dx12, const char* Name, u32 Width, u32 Height, DXGI_FORMAT DataFormat, bool HasAlpha,bool AllowRT,Vector ClearColor)
{
	Initialized = true;
	DX12 = dx12;
	TextureName = Name;
	Format = DataFormat;

	D3D12_RESOURCE_DESC TextureDesc = {};
	TextureDesc.MipLevels = 1;
	TextureDesc.DepthOrArraySize = 1;
	TextureDesc.Format = Format;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	if (AllowRT)
	{
		TextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		TextureDesc.SampleDesc.Count = MULTI_SAMPLE_COUNT;
		TextureDesc.SampleDesc.Quality = 0;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	}
	
	s32 textureHeapSize = ((((Width * 4) + 255) & ~255) * (Height - 1)) + (Width * 4);

	TextureBuffer.Initialize(DX12->device, TextureDesc, textureHeapSize, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, ClearColor);

	TextureOffset = (DX12->TextureHeapDescriptorCount * DX12->TextureHeapDescriptorSize);

	TextureHandleCPU.ptr = DX12->TextureHeap->GetCPUDescriptorHandleForHeapStart().ptr + TextureOffset;
	TextureHandleGPU.ptr = DX12->TextureHeap->GetGPUDescriptorHandleForHeapStart().ptr + TextureOffset;


	
	DX12->device->CreateShaderResourceView(TextureBuffer.Resource, &srvDesc, TextureHandleCPU);
	DX12->TextureHeapDescriptorCount++;
}
void DX12Texture::InitFromData(DX12CommandQueue* Queue, DirectX12* dx12, const char* Name,void* Data,u32 Width,u32 Height,DXGI_FORMAT DataFormat,bool HasAlpha)
{
	Initialized = true;
	DX12 = dx12;
	TextureName = Name;
	hasAlpha = HasAlpha;
	Format = DataFormat;
	D3D12_RESOURCE_DESC TextureDesc = {};
	TextureDesc.MipLevels = 1;
	TextureDesc.DepthOrArraySize = 1;
	TextureDesc.Format = Format;
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	s32 textureHeapSize = ((((Width * 4) + 255) & ~255) * (Height - 1)) + (Width * 4);

	TextureBuffer.Initialize(DX12->device, TextureDesc, textureHeapSize, D3D12_RESOURCE_STATE_COMMON, {});

	D3D12_SUBRESOURCE_DATA textureData;
	textureData.pData = Data;
	textureData.RowPitch = Width * 4;
	textureData.SlicePitch = Height * Width * 4;

	TextureBuffer.UpdateSubresource(Queue, 0, 0, &textureData);

	TextureBuffer.Transition(Queue, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	TextureOffset = (DX12->TextureHeapDescriptorCount * DX12->TextureHeapDescriptorSize);

	TextureHandleCPU.ptr = DX12->TextureHeap->GetCPUDescriptorHandleForHeapStart().ptr + TextureOffset;
	TextureHandleGPU.ptr = DX12->TextureHeap->GetGPUDescriptorHandleForHeapStart().ptr + TextureOffset;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	
	DX12->device->CreateShaderResourceView(TextureBuffer.Resource, &srvDesc, TextureHandleCPU);
	DX12->TextureHeapDescriptorCount++;
}
void DX12Texture::InitFromFile(DX12CommandQueue* Queue,DirectX12* dx12, const char* Name)
{
	DX12 = dx12;
	TextureName = Name;
	if (Name != NULL) 
	{
		PNG Image = LoadPNG(Name);
		InitFromData(Queue, dx12, Name, Image.Data, Image.Width, Image.Height, DXGI_FORMAT_R8G8B8A8_UNORM, Image.HasAlpha);
		return;
	}
	Vector c = { 0.0f,0.0f,0.0f,0.0f };
	InitFromData(Queue, dx12, "NULL", (void*)&c, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, false);
	
}
void DX12Texture::SetTexture(DX12CommandQueue* Queue)
{
	Queue->SetDescriptorTable(2, TextureHandleGPU);
}
void DX12Texture::Update(DX12CommandQueue* Queue,void* TextureData,u32 Width,u32 Height)
{
	D3D12_SUBRESOURCE_DATA textureData;
	textureData.pData = TextureData;
	textureData.RowPitch = Width * 4;
	textureData.SlicePitch = Height * Width * 4;

	TextureBuffer.Transition(Queue, D3D12_RESOURCE_STATE_COMMON);
	TextureBuffer.UpdateSubresource(Queue, 0, 0, &textureData);
	TextureBuffer.Transition(Queue, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}