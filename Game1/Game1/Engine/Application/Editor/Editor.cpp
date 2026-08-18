#include "Editor.h"
#include "Application\Engine.h"
void Editor::Init()
{
	
	f32 PanelWidth = (float)GEngine.pWindow->width * 0.2f;
	f32 TabsHeight = 20;
	f32 HeaderHeight = 20;

	Vector ViewportDim = { (GEngine.pWindow->width - PanelWidth)  , GEngine.pWindow->height - HeaderHeight };//{ GEngine.pWindow->width - ShaderPanelWidth - 100,GEngine.pWindow->height - HeaderHeight };
	Vector ViewportPosition = { PanelWidth,HeaderHeight + TabsHeight };
	Vector Color = { (43.0f / 255.0f),(55.0f / 255.0f) ,(61.0f / 255.0f),1.0f };

	tabPanel.Init(this, GEngine.pObjManager2D, { ViewportDim.m128_f32[0],TabsHeight }, { ViewportPosition.m128_f32[0],HeaderHeight,1.0f }, Color);

	tabPanel.AddTab(100, "Editor");
	tabPanel.AddTab(100, "Material");

	OptionsPanel = GEngine.pObjManager2D->AddObject({ 0.0f,0.0f,1.0f }, { (float)GEngine.pWindow->width ,HeaderHeight },
		{ 0.0f,0.0f }, Color, "TopLeft", NULL, "Ortho");


	SceneViewport.Initialize(GEngine.pRendererInterface->device, ViewportPosition.m128_f32[0], ViewportPosition.m128_f32[1], ViewportDim.m128_f32[0], ViewportDim.m128_f32[1], "SceneViewport");
	SceneViewport.SetPipeline(SceneViewport.PipelineManager.GetPipeline("Ortho"));
	SceneViewport.PipelineManager.AddPipeline(GEngine.pRendererInterface->device, "Default", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/BasicMain3DPipeline.desc", true, false);

	GEngine.pObjManager2D->AddTexture(&SceneViewport.Texture);
	ViewportObject = GEngine.pObjManager2D->AddObject(ViewportPosition, ViewportDim, { 0.0f,0.0f }, { 0,0,0,1 }, "TopLeft", "SceneViewport", "Ortho");
	ObjectManager = (ObjectManager2D*)GEngine.Global.Allocate(sizeof(ObjectManager2D));
	ObjectManager->Init(GEngine.pRendererInterface, &SceneViewport.PipelineManager);
	
	shaderEditor.Init(&SceneViewport.MManager, &SceneViewport.KManager, ObjectManager, PanelWidth, {0,HeaderHeight,1},ViewportDim,Color);
	materialEditor.Init(&SceneViewport.MManager,ObjectManager, { 0.0f, HeaderHeight, 1.0f }, { PanelWidth,(float)GEngine.pWindow->height - HeaderHeight },Color);
	OnRegisterChangedParams params;
	params.Parent = &materialEditor;
	
	shaderEditor.RegisterOnPSRegisterChanged(MaterialEditor_OnPSRegistersChanged,params);
	materialEditor.MaterialOptions.AddOnPropertyAddedCallback(&shaderEditor,ShaderEditor_OnPropertyChanged);
}
void Editor::Update()
{
	shaderEditor.Update(tabPanel.CurrentTab == 0);
	materialEditor.Update(tabPanel.CurrentTab == 1);
	tabPanel.Update();
}
void Editor::Draw()
{
	SceneViewport.StartRender(true);
	switch(tabPanel.CurrentTab)
	{
		case 0: // Shader editor
		{
			SceneViewport.CManager.SetCamera(SceneViewport.Camera2D);
			shaderEditor.Draw();
			break;
		}
		case 1: //MaterialEditor
		{
			SceneViewport.CManager.SetCamera(SceneViewport.Camera3D);

			DefaultOnResizeBuffer OnResize3D;
			DX12Camera* Cam = SceneViewport.CManager.GetCurrentCamera();

			OnResize3D.CamPos = Cam->Position;
			OnResize3D.Proj = Cam->PMatrix;
			OnResize3D.View = Cam->VMatrix;

			//SceneViewport.PipelineManager.GetPipeline("Default")->UpdateVSOnResize(&OnResize3D);
			
			materialEditor.Draw();
			break;
		}
	}
	ObjectManager->Draw(&SceneViewport.Queue);

	SceneViewport.StopRender();
	
	ViewportObject->Draw();
	OptionsPanel->Draw();
	tabPanel.Draw();
}
