#include "DX12PipelineManager.h"
#include "DX12Pipeline.h"

void DX12PipelineManager::Init()
{
	PipelineList2D.Init(32, sizeof(DX12Pipeline)); // dont want this reallocating because points will be invalid
	PipelineList3D.Init(32, sizeof(DX12Pipeline)); // dont want this reallocating because points will be invalid
}
void DX12PipelineManager::SetPipeline2D(DX12CommandQueue* Queue,u32 FrameIndex,u32 Index,bool Reset)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList2D);
	
	Pipelines[Index].Bind(Queue,FrameIndex,Reset);
	CurrentPipeline = &Pipelines[Index];
}
void DX12PipelineManager::AddPipeline2D(DX12Pipeline* pipeline)
{
	PipelineList2D.Add(pipeline);
}
void DX12PipelineManager::AddPipeline2D(ID3D12Device* Device,const char* Name, const char* fileName,bool Depth,bool MSAA)
{
	DX12Pipeline Pipeline;
	Pipeline.Create(Device,Name, fileName, Depth, MSAA);
	PipelineList2D.Add(&Pipeline);
}

s32 DX12PipelineManager::GetPipelineIndex2D(DX12Pipeline* pipeline)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList2D);

	for (u32 i = 0; i < PipelineList2D.elementCount; i++)
	{
		if (pipeline == &Pipelines[i])
		{
			return i;
		}
	}
	return -1;
}
s32 DX12PipelineManager::GetPipelineIndex2D(const char* PipelineName)
{
	//TODO: Hash map would be better
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList2D);
	for (u32 i = 0; i < PipelineList2D.elementCount; i++)
	{
		if (!strcmp(Pipelines[i].PipelineName,PipelineName))
		{
			return i;
		}
	}
	return -1;
}
DX12Pipeline* DX12PipelineManager::GetPipeline2D(u32 Index)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList2D);
	return &Pipelines[Index];
}
DX12Pipeline* DX12PipelineManager::GetPipeline2D(const char* Name)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList2D);
	return &Pipelines[GetPipelineIndex2D(Name)];
}
void DX12PipelineManager::Update2D(u32 FrameIndex,void* OnResizeBuffer, u32 Index)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList2D);
	Pipelines[Index].FrameIndex = FrameIndex;
	Pipelines[Index].UpdateVSOnResize(OnResizeBuffer);
}
s32 DX12PipelineManager::GetPipelineIndex3D(DX12Pipeline* pipeline)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList3D);

	for (u32 i = 0; i < PipelineList3D.elementCount; i++)
	{
		if (pipeline == &Pipelines[i])
		{
			return i;
		}
	}
	return -1;
}
s32 DX12PipelineManager::GetPipelineIndex3D(const char* PipelineName)
{
	//TODO: Hash map would be better
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList3D);
	for (u32 i = 0; i < PipelineList3D.elementCount; i++)
	{
		if (!strcmp(Pipelines[i].PipelineName, PipelineName))
		{
			return i;
		}
	}
	return -1;
}
DX12Pipeline* DX12PipelineManager::GetPipeline3D(u32 Index)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList3D);
	return &Pipelines[Index];
}
DX12Pipeline* DX12PipelineManager::GetPipeline3D(const char* Name)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList3D);
	return &Pipelines[GetPipelineIndex3D(Name)];
}
void DX12PipelineManager::Update3D(u32 FrameIndex, void* OnResizeBuffer, u32 Index)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList3D);
	Pipelines[Index].FrameIndex = FrameIndex;
	Pipelines[Index].UpdateVSOnResize(OnResizeBuffer);
}
void DX12PipelineManager::AddPipeline3D(DX12Pipeline* pipeline)
{
	PipelineList3D.Add(pipeline);
}
void DX12PipelineManager::AddPipeline3D(ID3D12Device* Device, const char* Name, const char* fileName, bool Depth, bool MSAA)
{
	DX12Pipeline Pipeline;
	Pipeline.Create(Device, Name, fileName, Depth, MSAA);
	PipelineList3D.Add(&Pipeline);
}
void DX12PipelineManager::SetPipeline3D(DX12CommandQueue* Queue, u32 FrameIndex, u32 Index, bool Reset)
{
	DX12Pipeline* Pipelines = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineList3D);

	Pipelines[Index].Bind(Queue, FrameIndex, Reset);
	CurrentPipeline = &Pipelines[Index];
}
void DX12PipelineManager::DeletePipeline2D(u32 Index)
{
	PipelineList2D.Delete(Index);
}
void DX12PipelineManager::DeletePipeline3D(u32 Index)
{
	PipelineList3D.Delete(Index);
}