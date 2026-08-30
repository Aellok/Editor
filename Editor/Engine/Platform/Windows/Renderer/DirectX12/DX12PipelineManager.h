#pragma once
#include "System\Memory\DynamicArray.h"

struct DX12CommandQueue;
struct DX12Pipeline;
struct ID3D12Device;
struct DX12PipelineManager
{
	//DX12Pipeline
	DynamicArray PipelineList3D;
	//DX12Pipeline
	DynamicArray PipelineList2D;

	DX12Pipeline* CurrentPipeline;

	void Init();
	void Update2D(u32 FrameIndex, void* OnResizeBuffer, u32 Index);

	void SetPipeline2D(DX12CommandQueue* Queue, u32 FrameIndex, u32 Index,bool Reset);
	void AddPipeline2D(DX12Pipeline* pipeline);
	void AddPipeline2D(ID3D12Device* Device,const char* Name, const char* fileName, bool Depth, bool MSAA);
	
	s32 GetPipelineIndex2D(DX12Pipeline* pipeline);
	s32 GetPipelineIndex2D(const char* PipelineName);
	DX12Pipeline* GetPipeline2D(u32 Index);
	DX12Pipeline* GetPipeline2D(const char* Name);

	void Update3D(u32 FrameIndex, void* OnResizeBuffer, u32 Index);
	void SetPipeline3D(DX12CommandQueue* Queue, u32 FrameIndex, u32 Index, bool Reset);
	s32 GetPipelineIndex3D(DX12Pipeline* pipeline);
	s32 GetPipelineIndex3D(const char* PipelineName);
	DX12Pipeline* GetPipeline3D(u32 Index);
	DX12Pipeline* GetPipeline3D(const char* Name);
	void AddPipeline3D(DX12Pipeline* pipeline);
	void AddPipeline3D(ID3D12Device* Device, const char* Name, const char* fileName, bool Depth, bool MSAA);
	void DeletePipeline2D(u32 Index);
	void DeletePipeline3D(u32 Index);

};