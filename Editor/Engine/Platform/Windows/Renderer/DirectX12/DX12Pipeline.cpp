#include "DX12Pipeline.h"
#include "d3dx12.h"
#include "System/Memory/Arena.h"
#include "System/Utils/File.h"
#include "DX12CommandQueue.h"
#include "RenderDefines.h"


const char* RootSigParamTypesAscii[RSKTCount]
{
	"CBV",
	"UAV",
	"TEXTURE",
	"SAMPLER"
};

struct InputDescStruct
{
	s8 SemanticName[32];
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D12_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
};
void DX12Pipeline::CreateCompute(ID3D12Device* Device,const s8* fileName)
{
	Compute = true;
	File file;

	file.Load(fileName);

	device = Device;

	CD3DX12_DESCRIPTOR_RANGE backBufferRange[1];
	backBufferRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0);

	CD3DX12_ROOT_PARAMETER pipelineParameters[1];
	pipelineParameters[0].InitAsDescriptorTable(1, &backBufferRange[0]);

	D3D12_ROOT_SIGNATURE_DESC sigDesc;
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	sigDesc.NumParameters = 1;
	sigDesc.NumStaticSamplers = 0;
	sigDesc.pParameters = pipelineParameters;
	sigDesc.pStaticSamplers = nullptr;
	ID3DBlob* sigBlob;
	D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, nullptr);
	device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSig));
	
	D3D12_COMPUTE_PIPELINE_STATE_DESC pDesc = {};
	pDesc.pRootSignature = rootSig;
	pDesc.CS = {file.Data,file.FileSize};
	

	device->CreateComputePipelineState(&pDesc,IID_PPV_ARGS(&pipelineState));
}
void DX12Pipeline::Create(ID3D12Device* Device, D3D12_INPUT_ELEMENT_DESC* VSInputLayout,u32 VSInputLayoutCount, VertexShader& Vs,PixelShader& Ps,ID3D12RootSignature* Signature,u32 Width, u32 Height,bool AlphaEnabled)
{
	device = Device;

	CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
	if (AlphaEnabled)
	{
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	}
	
	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { VSInputLayout, VSInputLayoutCount };
	psoDesc.pRootSignature = Signature;
	psoDesc.VS = { Vs.file.Data, Vs.file.FileSize };
	psoDesc.PS = { Ps.file.Data, Ps.file.FileSize };
	
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;

	Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

	D3D12_DESCRIPTOR_HEAP_DESC DSHeapDesc = {};
	DSHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DSHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSHeapDesc.NumDescriptors = 1;
	
	Device->CreateDescriptorHeap(&DSHeapDesc, IID_PPV_ARGS(&dsDescriptorHeap));

	D3D12_DEPTH_STENCIL_VIEW_DESC DSViewDesc = {};
	DSViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;
	CD3DX12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Width, Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	Device->CreateCommittedResource(&props,D3D12_HEAP_FLAG_NONE,&Desc,D3D12_RESOURCE_STATE_DEPTH_WRITE,&depthOptimizedClearValue,IID_PPV_ARGS(&depthStencilBuffer));

	Device->CreateDepthStencilView(depthStencilBuffer, &DSViewDesc, dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}
#define ALIGN_256(size) (((size) + 255) & ~255)

void DX12Pipeline::Create(ID3D12Device* Device, const s8* Name,const s8* FileName, bool DepthTesting, bool MultiSample)
{
	memcpy(FilePath, FileName, strlen(FileName) + 1);
	memcpy(PipelineName, Name, strlen(Name) + 1);
	DX12PipelineDesc2 desc;
	File file;
	file.Load(FileName);
	if (!file.fp)
	{
		printf("Error: Failed to find shader %s\n", FileName);
		return;
	}
	desc.Deserialize(file.Data);
	Create(Device, desc, DepthTesting, MultiSample);
	file.Close();
}
void DX12Pipeline::Create(ID3D12Device* Device, DX12PipelineDesc2 desc, bool DepthTesting,bool MultiSample)
{
	FrameIndex = 0;
	VertexBufferElementSize = desc.header.InputSizes[0];
	InstanceBufferElementSize = desc.header.InputSizes[1];
	D3D12_BLEND_DESC blendDesc = { 0 };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	device = Device;

	RootSigParamCount = desc.header.RootSigParamCount;
	memcpy(RootSigParamTypes, desc.header.RootSigParamTypes, desc.header.RootSigParamCount * sizeof(D3D12_ROOT_PARAMETER_TYPE));

	//Generate CBV Structure
	for (u32 i = 0; i < desc.header.CBVCount; i++)
	{
		CD3DX12_HEAP_PROPERTIES props(D3D12_HEAP_TYPE_UPLOAD);
		//TODO: Why is this multiplied by 1024?
		CD3DX12_RESOURCE_DESC CBVDesc = CD3DX12_RESOURCE_DESC::Buffer(ALIGN_256(desc.header.CBVSizes[i] * 1024)); //TODO: this should be done in the AssetBuilder.
		device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &CBVDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&CBVs.constBufferUploadHeap[i]));
		device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &CBVDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&CBVs.constBufferUploadHeap[32 + i]));

		CD3DX12_RANGE readRange(0, 0);
		CBVs.constBufferUploadHeap[i]->Map(0, &readRange, (void**)&CBVs.constBufferAddress[i]);
		CBVs.constBufferUploadHeap[32 + i]->Map(0, &readRange, (void**)&CBVs.constBufferAddress[32 + i]);
		CBVs.BufferOffsetSizes[i] = ALIGN_256(desc.header.CBVSizes[i]);
	}
	memset(CBVs.BufferOffsets, 0, 64 * sizeof(u32));


	HRESULT hr = Device->CreateRootSignature(0, desc.rootSigBlob, desc.header.RootSigSize, IID_PPV_ARGS(&rootSig));

	if (hr != S_OK)
	{
		return;
	}

	if (true)
	{
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;

	}

	psoDesc.InputLayout = { desc.InputLayout, desc.header.InputCount };
	psoDesc.pRootSignature = rootSig;
	psoDesc.VS = { desc.CompiledVS, desc.header.CompiledVSSize };
	//psoDesc.HS = {};
	//psoDesc.DS = {};
	//psoDesc.GS = {};
	psoDesc.PS = { desc.CompiledPS, desc.header.CompiledPSSize };
	
	
	
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = blendDesc;
	if (DepthTesting)
	{
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
	}
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	if (MultiSample)
	{
		psoDesc.SampleDesc.Count = MULTI_SAMPLE_COUNT;
	}
	hr = Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
	if (hr != S_OK)
	{
		return;
	}

	D3D12_DESCRIPTOR_HEAP_DESC DSHeapDesc = {};
	DSHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DSHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSHeapDesc.NumDescriptors = 1;

	Device->CreateDescriptorHeap(&DSHeapDesc, IID_PPV_ARGS(&dsDescriptorHeap));

	D3D12_DEPTH_STENCIL_VIEW_DESC DSViewDesc = {};
	DSViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	if (MultiSample)
	{
		DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	}
	
	DSViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	
	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;
	
	CD3DX12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, 2560, 1440, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	if (MultiSample)
	{
		Desc.SampleDesc.Count = MULTI_SAMPLE_COUNT;
	}
	Device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &Desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, IID_PPV_ARGS(&depthStencilBuffer));

	Device->CreateDepthStencilView(depthStencilBuffer, &DSViewDesc, dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}
void DX12Pipeline::UpdateVSPerFrame(DX12CommandQueue* Queue,void* Data)
{
	// Assuming that CBVs.constBufferUploadHeap[1] is the PerFrameBuffer;
	memcpy(CBVs.constBufferAddress[(FrameIndex * 32) + 1] + CBVs.BufferOffsets[(FrameIndex * 32) + 1], Data, CBVs.BufferOffsetSizes[1]);
	Queue->SetConstantBuffer(1, CBVs.GetResourceGPUAddress(FrameIndex, 1) + CBVs.GetBufferOffset(FrameIndex, 1));
	CBVs.BufferOffsets[(FrameIndex * 32) + 1] += CBVs.BufferOffsetSizes[1];
	
}
void DX12Pipeline::UpdateVSOnResize(void* Data)
{
	//printf("UpdateVSOnResize: %s\n", PipelineName);
	// Assuming that CBVs.constBufferUploadHeap[0] is the OnResizeBuffer;
	memcpy(CBVs.constBufferAddress[(FrameIndex * 32)], Data, CBVs.BufferOffsetSizes[0]);
}

void DX12Pipeline::Resize(u32 Width, u32 Height)
{
	if (Width && Height)
	{
		if (!Compute)
		{
			dsDescriptorHeap->Release();
			depthStencilBuffer->Release();

			D3D12_DESCRIPTOR_HEAP_DESC DSHeapDesc = {};
			DSHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			DSHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			DSHeapDesc.NumDescriptors = 1;

			device->CreateDescriptorHeap(&DSHeapDesc, IID_PPV_ARGS(&dsDescriptorHeap));

			D3D12_DEPTH_STENCIL_VIEW_DESC DSViewDesc = {};
			DSViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			DSViewDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
			depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
			depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
			depthOptimizedClearValue.DepthStencil.Stencil = 0;
			CD3DX12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			CD3DX12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Width, Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
			device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &Desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, IID_PPV_ARGS(&depthStencilBuffer));

			device->CreateDepthStencilView(depthStencilBuffer, &DSViewDesc, dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		}
	}
}

void DX12Pipeline::Bind(DX12CommandQueue* Queue, u32 NewFrameIndex,bool Reset)
{
	FrameIndex = NewFrameIndex;

	Queue->SetPipeline(this);
	if (Reset)
	{
		Queue->ClearDepthStencil(this);
		//Reset the CBVs.
		CBVs.Reset(FrameIndex);
	}
	
	Queue->SetConstantBuffer(0, CBVs.GetResourceGPUAddress(FrameIndex, 0)); // Sets the OnResizeBuffer as this wont change.
}
void CBVInfo::Reset(u32 FrameIndex)
{
	memset(&BufferOffsets[32 * FrameIndex], 0, sizeof(u32) * 32);
}
D3D12_GPU_VIRTUAL_ADDRESS CBVInfo::GetResourceGPUAddress(u32 FrameIndex, u32 BufferIndex)
{
	return constBufferUploadHeap[(32 * FrameIndex) + BufferIndex]->GetGPUVirtualAddress();
}
u32 CBVInfo::GetBufferOffset(u32 FrameIndex, u32 BufferIndex)
{
	return BufferOffsets[(32 * FrameIndex) + BufferIndex];
}
#define COPY_ADVANCE_WRITE(dest,src,size) \
	memcpy(dest,src,size);\
	dest += size;\

#define COPY_ADVANCE_READ(dest,src,size) \
	memcpy(dest,src,size);\
	src += size;\

void* DX12PipelineDesc::Serialize(u32* TotalSize)
{
	*TotalSize = DX12_PIPELINE_HEADER_SIZE; // Static var sizes.
	//Variable Length Arrays.
	*TotalSize += RootSigSize;
	*TotalSize += InputCount * (sizeof(D3D12_INPUT_ELEMENT_DESC) - 8); // -8 due to the ptr in the struct
	for (u32 i = 0; i < InputCount; i++)
	{
		*TotalSize += (u32)strlen(InputLayout[i].SemanticName);
	}
	*TotalSize += InputCount * sizeof(u32);
	*TotalSize += CompiledVSSize + CompiledHSSize + CompiledDSSize + CompiledGSSize + CompiledPSSize;

	void* memory = calloc(1,*TotalSize);
	if (!memory)
	{
		printf("Error: ran out of memory?\n");
		return NULL;
	}
	char* writePtr = (char*)memory;

	COPY_ADVANCE_WRITE(writePtr, this, DX12_PIPELINE_HEADER_SIZE);
	for (u32 i = 0; i < InputCount; i++)
	{
		u32 StringLength = (u32)strlen(InputLayout[i].SemanticName);
		COPY_ADVANCE_WRITE(writePtr, &StringLength,sizeof(u32));
		COPY_ADVANCE_WRITE(writePtr, InputLayout[i].SemanticName, StringLength);
		COPY_ADVANCE_WRITE(writePtr, (char*)&InputLayout[i] + 8, sizeof(D3D12_INPUT_ELEMENT_DESC) - 8);
	}
	COPY_ADVANCE_WRITE(writePtr, rootSigBlob, RootSigSize);
	if (CompiledVS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledVS, CompiledVSSize);
	}
	if (CompiledHS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledHS, CompiledHSSize);
	}
	if (CompiledDS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledDS, CompiledDSSize);
	}
	if (CompiledGS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledGS, CompiledGSSize);
	}
	if (CompiledPS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledPS, CompiledPSSize);
	}
	return memory;
}
void DX12PipelineDesc::Deserialize(void* Data)
{
	char* readPtr = (char*)Data;
	COPY_ADVANCE_READ(this, readPtr, DX12_PIPELINE_HEADER_SIZE);
	
	InputLayout = (D3D12_INPUT_ELEMENT_DESC*)calloc(InputCount, sizeof(D3D12_INPUT_ELEMENT_DESC));

	for (u32 i = 0; i < InputCount; i++)
	{
		u32 StringLength = 0;
		COPY_ADVANCE_READ(&StringLength,readPtr,sizeof(u32));
		char* Buffer = (char*)calloc(1,StringLength + 1);
		InputLayout[i].SemanticName = Buffer;
		COPY_ADVANCE_READ(Buffer, readPtr,StringLength);
		COPY_ADVANCE_READ((char*)&InputLayout[i] + 8, readPtr, sizeof(D3D12_INPUT_ELEMENT_DESC) - 8);
	}
	rootSigBlob = malloc(RootSigSize);
	COPY_ADVANCE_READ(rootSigBlob, readPtr, RootSigSize);
	if (CompiledVSSize)
	{
		CompiledVS = malloc(CompiledVSSize);
		COPY_ADVANCE_READ(CompiledVS, readPtr, CompiledVSSize);
	}
	if (CompiledHSSize)
	{
		CompiledHS = malloc(CompiledHSSize);
		COPY_ADVANCE_READ(CompiledHS, readPtr, CompiledHSSize);
	}
	if (CompiledDSSize)
	{
		CompiledDS = malloc(CompiledDSSize);
		COPY_ADVANCE_READ(CompiledDS, readPtr, CompiledDSSize);
	}
	if (CompiledGSSize)
	{
		CompiledGS = malloc(CompiledGSSize);
		COPY_ADVANCE_READ(CompiledGS, readPtr, CompiledGSSize);
	}
	if (CompiledPSSize)
	{
		CompiledPS = malloc(CompiledPSSize);
		COPY_ADVANCE_READ(CompiledPS, readPtr, CompiledPSSize);
	}

}

void* DX12PipelineDesc2::Serialize(u32* TotalSize)
{
	*TotalSize = sizeof(header); // Static var sizes.
	//Variable Length Arrays.
	*TotalSize += header.RootSigSize;
	*TotalSize += header.InputCount * (sizeof(D3D12_INPUT_ELEMENT_DESC) - 8); // -8 due to the ptr in the struct
	for (u32 i = 0; i < header.InputCount; i++)
	{
		*TotalSize += (u32)strlen(InputLayout[i].SemanticName);
	}
	*TotalSize += header.InputCount * sizeof(u32);
	*TotalSize += header.CompiledVSSize + header.CompiledHSSize + header.CompiledDSSize + header.CompiledGSSize + header.CompiledPSSize;

	void* memory = calloc(1, *TotalSize);
	if (!memory)
	{
		printf("Error: ran out of memory?\n");
		return NULL;
	}
	char* writePtr = (char*)memory;

	COPY_ADVANCE_WRITE(writePtr, &header, sizeof(header));
	for (u32 i = 0; i < header.InputCount; i++)
	{
		u32 StringLength = (u32)strlen(InputLayout[i].SemanticName);
		COPY_ADVANCE_WRITE(writePtr, &StringLength, sizeof(u32));
		COPY_ADVANCE_WRITE(writePtr, InputLayout[i].SemanticName, StringLength);
		COPY_ADVANCE_WRITE(writePtr, (char*)&InputLayout[i] + 8, sizeof(D3D12_INPUT_ELEMENT_DESC) - 8);
	}
	COPY_ADVANCE_WRITE(writePtr, rootSigBlob, header.RootSigSize);
	if (CompiledVS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledVS, header.CompiledVSSize);
	}
	if (CompiledHS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledHS, header.CompiledHSSize);
	}
	if (CompiledDS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledDS, header.CompiledDSSize);
	}
	if (CompiledGS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledGS, header.CompiledGSSize);
	}
	if (CompiledPS)
	{
		COPY_ADVANCE_WRITE(writePtr, CompiledPS, header.CompiledPSSize);
	}
	return memory;
}
void DX12PipelineDesc2::Deserialize(void* Data)
{
	char* readPtr = (char*)Data;
	COPY_ADVANCE_READ(&header, readPtr, sizeof(header));

	InputLayout = (D3D12_INPUT_ELEMENT_DESC*)calloc(header.InputCount, sizeof(D3D12_INPUT_ELEMENT_DESC));

	for (u32 i = 0; i < header.InputCount; i++)
	{
		u32 StringLength = 0;
		COPY_ADVANCE_READ(&StringLength, readPtr, sizeof(u32));
		char* Buffer = (char*)calloc(1, StringLength + 1);
		InputLayout[i].SemanticName = Buffer;
		COPY_ADVANCE_READ(Buffer, readPtr, StringLength);
		COPY_ADVANCE_READ((char*)&InputLayout[i] + 8, readPtr, sizeof(D3D12_INPUT_ELEMENT_DESC) - 8);
	}
	rootSigBlob = malloc(header.RootSigSize);
	COPY_ADVANCE_READ(rootSigBlob, readPtr, header.RootSigSize);
	if (header.CompiledVSSize)
	{
		CompiledVS = malloc(header.CompiledVSSize);
		COPY_ADVANCE_READ(CompiledVS, readPtr, header.CompiledVSSize);
	}
	if (header.CompiledHSSize)
	{
		CompiledHS = malloc(header.CompiledHSSize);
		COPY_ADVANCE_READ(CompiledHS, readPtr, header.CompiledHSSize);
	}
	if (header.CompiledDSSize)
	{
		CompiledDS = malloc(header.CompiledDSSize);
		COPY_ADVANCE_READ(CompiledDS, readPtr, header.CompiledDSSize);
	}
	if (header.CompiledGSSize)
	{
		CompiledGS = malloc(header.CompiledGSSize);
		COPY_ADVANCE_READ(CompiledGS, readPtr, header.CompiledGSSize);
	}
	if (header.CompiledPSSize)
	{
		CompiledPS = malloc(header.CompiledPSSize);
		COPY_ADVANCE_READ(CompiledPS, readPtr, header.CompiledPSSize);
	}

}
bool DX12PipelineDesc2::IsValid()
{
	return (bool)header.InputCount;
}