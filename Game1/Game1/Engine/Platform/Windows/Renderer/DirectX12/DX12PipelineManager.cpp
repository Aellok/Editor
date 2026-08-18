#include "DX12PipelineManager.h"
#include "DX12Pipeline.h"

void DX12PipelineManager::Init()
{
	PipelineList.Init(32, sizeof(DX12Pipeline)); // dont want this reallocating because points will be invalid
}
void DX12PipelineManager::SetPipeline(DX12CommandQueue* Queue,u32 FrameIndex,u32 Index,bool Reset)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList);
	
	Pipelines[Index].Bind(Queue,FrameIndex,Reset);
	CurrentPipeline = &Pipelines[Index];
}
void DX12PipelineManager::AddPipeline(DX12Pipeline* pipeline)
{
	PipelineList.Add(pipeline);
}
void DX12PipelineManager::AddPipeline(ID3D12Device* Device,const char* Name, const char* fileName,bool Depth,bool MSAA)
{
	DX12Pipeline Pipeline;
	Pipeline.Create(Device,Name, fileName, Depth, MSAA);
	PipelineList.Add(&Pipeline);
}
u32 DX12PipelineManager::GetPipelineIndex(DX12Pipeline* pipeline)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList);

	for (u32 i = 0; i < PipelineList.elementCount; i++)
	{
		if (pipeline == &Pipelines[i])
		{
			return i;
		}
	}
}
u32 DX12PipelineManager::GetPipelineIndex(const char* PipelineName)
{
	//TODO: Hash map would be better
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList);
	for (u32 i = 0; i < PipelineList.elementCount; i++)
	{
		if (!strcmp(Pipelines[i].PipelineName,PipelineName))
		{
			return i;
		}
	}
	return -1;
}
DX12Pipeline* DX12PipelineManager::GetPipeline(u32 Index)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList);
	return &Pipelines[Index];
}
DX12Pipeline* DX12PipelineManager::GetPipeline(const char* Name)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList);
	return &Pipelines[GetPipelineIndex(Name)];
}
void DX12PipelineManager::Update(u32 FrameIndex,void* OnResizeBuffer, u32 Index)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList);
	Pipelines[Index].FrameIndex = FrameIndex;
	Pipelines[Index].UpdateVSOnResize(OnResizeBuffer);
}

