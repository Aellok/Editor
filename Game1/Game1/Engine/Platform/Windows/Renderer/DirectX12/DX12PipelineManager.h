#pragma once
#include "System\Memory\DynamicArray.h"

struct DX12CommandQueue;
struct DX12Pipeline;
struct ID3D12Device;
struct DX12PipelineManager
{
	//DX12Pipeline
	DynamicArray PipelineList;

	DX12Pipeline* CurrentPipeline;

	void Init();
	void SetPipeline(DX12CommandQueue* Queue, u32 FrameIndex, u32 Index,bool Reset);
	void AddPipeline(DX12Pipeline* pipeline);
	void AddPipeline(ID3D12Device* Device,const char* Name, const char* fileName, bool Depth, bool MSAA);
	void Update(u32 FrameIndex, void* OnResizeBuffer,u32 Index);

	u32 GetPipelineIndex(DX12Pipeline* pipeline);
	u32 GetPipelineIndex(const char* PipelineName);
	DX12Pipeline* GetPipeline(u32 Index);
	DX12Pipeline* GetPipeline(const char* Name);

};