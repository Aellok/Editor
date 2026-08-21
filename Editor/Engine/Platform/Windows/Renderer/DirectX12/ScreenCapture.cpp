#include "ScreenCapture.h"

void ScreenCapture::Init()
{
    if (CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory4), (void**)&Factory) != S_OK)
    {
        printf("ScreenCapture: Failed to create a IDXGIFactory4\n");
        return;
    }
    if (Factory->EnumAdapters1(0, &Adapter) != S_OK)
    {
        printf("ScreenCapture: Failed to get Adapter 0\n");
        return;
    }
    if (Adapter->EnumOutputs(0, &DXGIOutput) != S_OK)
    {
        printf("ScreenCapture: Failed to get Adapter output 0\n");
        return;
    }
    if (D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&DX12Device) != S_OK)
    {
        printf("ScreenCapture: Failed to create a ID3D12Device\n");
        return;
    }
    
    D3D_FEATURE_LEVEL FeatureLevels[2]
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_1
    };
    
    if (D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, FeatureLevels, 2, D3D11_SDK_VERSION, &DX11Device, NULL, &DX11Context) != S_OK)
    {
        printf("ScreenCapture: Failed to create a ID3D12Device\n");
        return;
    }
    
    if (DX11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&DXGIDevice) != S_OK)
    {
        printf("ScreenCapture: Failed to get DXGIDevice\n");
        return;
    }
    if (DXGIOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&DXGIOutput1) != S_OK)
    {
        printf("ScreenCapture: Failed to get DXGIOutput1\n");
        return;
    }
    if (DXGIOutput1->DuplicateOutput(DXGIDevice, &OutputDup) != S_OK)
    {
        printf("ScreenCapture: Failed to get DXGIOutputDuplication\n");
        return;
    }


    DXGIOutput1->GetDesc(&DXGIOutputDesc);
    
    D3D11_TEXTURE2D_DESC stagingDesc = {0};
    stagingDesc.MipLevels = 1;
    stagingDesc.ArraySize = 1;
    stagingDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    stagingDesc.Height = DXGIOutputDesc.DesktopCoordinates.bottom;
    stagingDesc.Width = DXGIOutputDesc.DesktopCoordinates.right;
    stagingDesc.MiscFlags = 0;
    stagingDesc.SampleDesc.Count = 1;
    stagingDesc.BindFlags = 0;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    TextureData = malloc(DXGIOutputDesc.DesktopCoordinates.right * DXGIOutputDesc.DesktopCoordinates.bottom * 4);
    D3D11_SUBRESOURCE_DATA d;
    d.pSysMem = TextureData;
    d.SysMemPitch = DXGIOutputDesc.DesktopCoordinates.right;
    d.SysMemSlicePitch = DXGIOutputDesc.DesktopCoordinates.bottom * DXGIOutputDesc.DesktopCoordinates.right * 4;
    
    if (DX11Device->CreateTexture2D(&stagingDesc, &d, &stagingTexture) != S_OK)
    {
        printf("ScreenCapture: Failed to create a D3D11Texture2D\n");
        return;
    }
}

bool ScreenCapture::GetFrame(void** Data,u32 &Size)
{
    ID3D11Texture2D* Image;

    IDXGIResource* DesktopResource = NULL;
    DXGI_OUTDUPL_FRAME_INFO FrameInfo;

    if (OutputDup->AcquireNextFrame(500, &FrameInfo, &DesktopResource) != S_OK)
    {
        printf("ScreenCapture: AquireNextFrame() failed\n");
        return false;
    }
    if (DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&Image)) != S_OK)
    {
        printf("ScreenCapture: Failed to get D3D11Texture2D\n");
        return false;
    }
    DesktopResource->Release();
    DesktopResource = NULL;
    /*
    // Get metadata
    if (FrameInfo.TotalMetadataBufferSize)
    {
        // Old buffer too small
        if (FrameInfo.TotalMetadataBufferSize > MetaDataSize)
        {
            if (MetaDataBuffer)
            {
                delete[] MetaDataBuffer;
                MetaDataBuffer = NULL;
            }
            MetaDataBuffer = new (std::nothrow) BYTE[FrameInfo.TotalMetadataBufferSize];
            if (!MetaDataBuffer)
            {
                DisplayErr(L"Failed to allocate memory for metadata in DUPLICATIONMANAGER", L"Error", E_OUTOFMEMORY);
                MetaDataSize = 0;
                Data->MoveCount = 0;
                Data->DirtyCount = 0;
                return E_OUTOFMEMORY;
            }
            MetaDataSize = FrameInfo.TotalMetadataBufferSize;
        }

        UINT BufSize = FrameInfo.TotalMetadataBufferSize;

        // Get move rectangles
        hr = DeskDupl->GetFrameMoveRects(BufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(MetaDataBuffer), &BufSize);
        if (FAILED(hr))
        {
            if (hr != DXGI_ERROR_ACCESS_LOST)
            {
                DisplayErr(L"Failed to get frame move rects in DUPLICATIONMANAGER", L"Error", hr);
            }
            Data->MoveCount = 0;
            Data->DirtyCount = 0;
            return hr;
        }
        Data->MoveCount = BufSize / sizeof(DXGI_OUTDUPL_MOVE_RECT);

        BYTE* DirtyRects = MetaDataBuffer + BufSize;
        BufSize = FrameInfo.TotalMetadataBufferSize - BufSize;

        // Get dirty rectangles
        hr = DeskDupl->GetFrameDirtyRects(BufSize, reinterpret_cast<RECT*>(DirtyRects), &BufSize);
        if (FAILED(hr))
        {
            if (hr != DXGI_ERROR_ACCESS_LOST)
            {
                DisplayErr(L"Failed to get frame dirty rects in DUPLICATIONMANAGER", L"Error", hr);
            }
            Data->MoveCount = 0;
            Data->DirtyCount = 0;
            return hr;
        }
        Data->DirtyCount = BufSize / sizeof(RECT);

        Data->MetaData = MetaDataBuffer;
    }
    */
 
    DX11Context->CopyResource(stagingTexture, Image);

    D3D11_MAPPED_SUBRESOURCE resource;
    if (DX11Context->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &resource) != S_OK)
    {
        printf("ScreenCapture: Failed to map texture\n");
        return false;
    }

    
    memcpy(TextureData, resource.pData, DXGIOutputDesc.DesktopCoordinates.right * DXGIOutputDesc.DesktopCoordinates.bottom * 4);
    
    DX11Context->Unmap(stagingTexture, 0);
    *Data = TextureData;

    Image->Release();
    OutputDup->ReleaseFrame();
    return true;
}