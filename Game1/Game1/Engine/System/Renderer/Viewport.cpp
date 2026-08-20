#include "Viewport.h"
#include "Application\Engine.h"
void Viewport_LButtonDown(void* Parent, Mouse mouse)
{
	Viewport* View = (Viewport*)Parent;
	if (ContainsPoint(mouse, View->Pos,View->Dim))
	{
		View->Focused = true;
		mouse.x -= View->Pos.m128_f32[0];
		mouse.y -= View->Pos.m128_f32[1];

		View->MManager.OnLClick(mouse);
	}
}
void Viewport_RButtonDown(void* Parent, Mouse mouse)
{
	Viewport* View = (Viewport*)Parent;
	if (ContainsPoint(mouse, View->Pos, View->Dim))
	{
		View->MManager.OnRClick(mouse);
	}
}
void Viewport_MButtonDown(void* Parent, Mouse mouse)
{
	Viewport* View = (Viewport*)Parent;
	if (ContainsPoint(mouse, View->Pos, View->Dim))
	{
		View->MManager.OnMClick(mouse);
	}
}
void Viewport_LButtonUp(void* Parent, Mouse mouse)
{
	Viewport* View = (Viewport*)Parent;
	if (ContainsPoint(mouse, View->Pos, View->Dim))
	{
		View->MManager.OnLRelease(mouse);
	}
}
void Viewport_RButtonUp(void* Parent, Mouse mouse)
{
	Viewport* View = (Viewport*)Parent;
	if (ContainsPoint(mouse, View->Pos, View->Dim))
	{
		View->MManager.OnRRelease(mouse);
	}
}
void Viewport_MButtonUp(void* Parent, Mouse mouse)
{
	Viewport* View = (Viewport*)Parent;
	if (ContainsPoint(mouse, View->Pos, View->Dim))
	{
		View->MManager.OnMRelease(mouse);
	}
}
void Viewport_Move(void* Parent, Mouse mouse)
{
	Viewport* View = (Viewport*)Parent;
	if (ContainsPoint(mouse, View->Pos, View->Dim))
	{
		mouse.x -= View->Pos.m128_f32[0];
		mouse.y -= View->Pos.m128_f32[1];
		View->MManager.Move(mouse);
	}
}
void Viewport_KeyUp(void* Parent, u32 Key)
{
	Viewport* View = (Viewport*)Parent;
	if (View->Focused)
	{
		View->KManager.OnKeyUp(Key);
	}
}
void Viewport_KeyDown(void* Parent,u32 Key)
{
	Viewport* View = (Viewport*)Parent;
	if (View->Focused)
	{
		View->KManager.OnKeyDown(Key);
	}
}
void Viewport_DragDrop(void* Parent,Mouse mouse,char* FileName)
{
	Viewport* View = (Viewport*)Parent;
	if (View->Focused)
	{
		View->MManager.DragDrop(mouse,FileName);
	}
}
void Viewport::Initialize(ID3D12Device* Device, u32 x, u32 y, u32 ScreenWidth, u32 ScreenHeight,const char* Name)
{
	PipelineManager.Init();
	//Engine specific pipelines.
	PipelineManager.AddPipeline3D(GEngine.pRendererInterface->device, "Main3D", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/Main3DPipeline.desc", true, false);
	PipelineManager.AddPipeline3D(GEngine.pRendererInterface->device, "NewMain3D", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/NewMain3D.desc", true, false);
	PipelineManager.AddPipeline3D(GEngine.pRendererInterface->device, "Instance3D", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/InstancePipeline.desc", true, false);
	PipelineManager.AddPipeline3D(GEngine.pRendererInterface->device, "InstanceOrtho", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/InstanceOrthoPipeline.desc", true, false);
	PipelineManager.AddPipeline3D(GEngine.pRendererInterface->device, "Default", "Engine/Application/Shaders/CompiledShaders/DefaultShader.desc", true, false);

	PipelineManager.AddPipeline2D(GEngine.pRendererInterface->device, "Ortho", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/OrthoPipeline.desc", true, false);
	PipelineManager.AddPipeline2D(GEngine.pRendererInterface->device, "Font", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/FontPipeline.desc", false, false);
	PipelineManager.AddPipeline2D(GEngine.pRendererInterface->device, "Font2", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/Font2Pipeline.desc", false, false);

	Focused = false;
	Pos = { (f32)x,(f32)y };
	Dim = { (f32)ScreenWidth,(f32)ScreenWidth };

	KManager.Init();
	MManager.Init();
	CREATE_KEYBOARD_CALLBACK(this, ViewportKeyboardCallbacks,Viewport);
	CREATE_MOUSE_CALLBACK(this, ViewportMouseCallbacks, Viewport);
	ViewportMouseCallbacks.OnDragDrop = Viewport_DragDrop;
	
	CManager.Init(&MManager, &KManager);

	GEngine.pWindow->mouseManager.Register(&ViewportMouseCallbacks);
	GEngine.pWindow->keyboardManager.Register(&ViewportKeyboardCallbacks);

	viewport.Init(0, 0, ScreenWidth, ScreenHeight  );

	Camera2D = CManager.AddCamera(45 * (PI / 180), ScreenWidth, ScreenHeight, { (float)0.0f,(float)0.0f,-6.0 }, eOrthoCam);
	Camera3D = CManager.AddCamera(45 * (PI / 180), ScreenWidth, ScreenHeight, { (float)0.0f,(float)0.0f,-6.0 }, eFreeCam);
	

	Queue.Initialize(Device);
	//Queue.CloseCommandList();
	Vector ClearColor = { 0,0,0,1 };
	Texture.Initialize(&Queue,GEngine.pRendererInterface,Name,ScreenWidth,ScreenHeight, DXGI_FORMAT_R8G8B8A8_UNORM,false,true, ClearColor);
	Texture.TextureBuffer.Transition(&Queue, D3D12_RESOURCE_STATE_RENDER_TARGET);
	RenderTarget.Initialize(Device, Texture.TextureBuffer.Resource, D3D12_RESOURCE_STATE_RENDER_TARGET, GEngine.pRendererInterface->rtvHeap3, 0, ClearColor);
	
	Queue.CloseCommandList();
	ID3D12CommandList* Lists[1] = { Queue.List };
	Queue.Execute(Lists, 1);
	Queue.WaitForGPU();

	CManager.Update(0.0f);
}
void Viewport::Bind()
{
	DirectX12* DX12 = GEngine.pRendererInterface;

	Queue.Reset(Pipeline);

	Texture.TextureBuffer.Transition(&Queue, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Queue.SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D12DescriptorHeap* descHeaps[1] = { DX12->TextureHeap };
	Queue.SetDescriptorHeaps(1, descHeaps);

	viewport.SetViewport(&Queue);
	
	Pipeline->Bind(&Queue,DX12->frameIndex,true);
}
void Viewport::SetPipeline(DX12Pipeline* pipeline)
{
	Pipeline = pipeline;
}
void Viewport::StartRender(bool RenderToTexture)
                                   {
	DirectX12* DX12 = GEngine.pRendererInterface;
	Bind();
	if (RenderToTexture)
	{
		RenderTarget.SetTarget(&Queue,true);
	}
	else
	{
		DX12->renderTargets[DX12->frameIndex]->SetTarget(&Queue,true);
	}
	
	CManager.Update(GEngine.GetFrameTime());

	Object2DOnResizeBuffer OnResize;
	DX12Camera* cam = CManager.GetCamera(Camera2D);
	OnResize.Ortho = cam->PMatrix;

	DX12Pipeline* pipeline2D = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineManager.PipelineList2D);
	DX12Pipeline* pipeline3D = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, PipelineManager.PipelineList3D);

	for (u32 i = 0; i < PipelineManager.PipelineList2D.elementCount; i++)
	{
		PipelineManager.GetPipeline2D(pipeline2D[i].PipelineName)->UpdateVSOnResize(&OnResize);
	}
	DX12Camera* Cam = CManager.GetCamera(Camera3D);
	DefaultOnResizeBuffer OnResize3D;
	OnResize3D.CamPos = Cam->Position;
	OnResize3D.Proj = DirectX::XMMatrixTranspose(Cam->PMatrix);
	OnResize3D.View = DirectX::XMMatrixTranspose(Cam->VMatrix);

	//3d
	for (u32 i = 0; i < PipelineManager.PipelineList3D.elementCount; i++)
	{
		PipelineManager.GetPipeline3D(pipeline3D[i].PipelineName)->UpdateVSOnResize(&OnResize3D);
	}
}
void Viewport::StopRender()
{
	Texture.TextureBuffer.Transition(&Queue, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	Queue.CloseCommandList();
	ID3D12CommandList* Lists[1] = { Queue.List };
	Queue.Execute(Lists, 1);
	Queue.WaitForGPU();
}