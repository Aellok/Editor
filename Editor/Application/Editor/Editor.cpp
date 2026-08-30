#include "Editor.h"
#include "Application\Engine.h"
#include "Application\Asset\Asset.h"
void Editor_OnKeyDown(void* Parent, u64 Key)
{
	Editor* editor = (Editor*)Parent;
	if (Key == VK_CONTROL)
	{
		editor->ctrl = true;
	}
	switch(editor->tabPanel.CurrentTab)
	{
		case 0: // Shader Editor
		{
			if (Key == VK_F5)
			{	
				DX12Pipeline* pipeline = editor->shaderEditor.CreatePipeline();
				
				if (pipeline)
				{
					memcpy(pipeline->PipelineName, "MaterialEditorPipeline", strlen("MaterialEditorPipeline"));
					editor->objectManager->AddPipeline3D("MaterialEditorPipeline", pipeline);
				}
			}
			break;
		}
		case 1: // Material Editor
		{
			if (editor->ctrl && Key == 'S')
			{
				if (editor->materialEditor.CurrentAsset && editor->materialEditor.CurrentAsset->FilePath)
				{
					editor->SaveAsset(editor->materialEditor.CurrentAsset->FilePath);
					return;
				}
				char FilePath[MAX_PATH];
				bool res = WinDialog_SaveSelector(FilePath);
				if (res)
				{
					editor->SaveAsset(FilePath);
				}
				editor->ctrl = false;
			}

			break;
		}
	}
}
void Editor_OnKeyUp(void* Parent, u32 Key)
{
	Editor* editor = (Editor*)Parent;
	if (Key == VK_CONTROL)
	{
		editor->ctrl = false;
	}
}
void Editor::Init()
{
	ctrl = false;
	PipelineChanged = false;
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
	SceneViewport.SetPipeline(SceneViewport.PipelineManager.GetPipeline2D("Ortho"));
	SceneViewport.PipelineManager.AddPipeline3D(GEngine.pRendererInterface->device, "Default", "Engine/Platform/Windows/Renderer/DirectX12/HLSL/SavedPipelines/temp/BasicMain3DPipeline.desc", true, false);

	GEngine.pObjManager2D->AddTexture(&SceneViewport.Texture);
	ViewportObject = GEngine.pObjManager2D->AddObject(ViewportPosition, ViewportDim, { 0.0f,0.0f }, { 0,0,0,1 }, "TopLeft", "SceneViewport", "Ortho");
	objectManager = (ObjectManager*)GEngine.Global.Allocate(sizeof(ObjectManager));
	objectManager->Init(GEngine.pRendererInterface, &SceneViewport.PipelineManager);
	
	shaderEditor.Init(&SceneViewport.MManager, &SceneViewport.KManager, objectManager, PanelWidth, {0,HeaderHeight,1},ViewportDim,Color);
	materialEditor.Init(&SceneViewport.MManager, objectManager, this,{ 0.0f, HeaderHeight, 1.0f }, { PanelWidth,(float)GEngine.pWindow->height - HeaderHeight },Color);
	OnRegisterChangedParams params;
	params.Parent = &materialEditor;
	
	shaderEditor.RegisterOnPSRegisterChanged(MaterialEditor_OnPSRegistersChanged,params);
	materialEditor.MaterialOptions.AddOnPropertyAddedCallback(&shaderEditor,ShaderEditor_OnPropertyChanged);
	
	materialEditor.AddOnObjectChangedCallback(&shaderEditor,ShaderEditor_OnObjectChanged);
	kCallbacks.IsEnabled = true;
	kCallbacks.OnKeyDown = Editor_OnKeyDown;
	kCallbacks.OnKeyUp = NULL;
	kCallbacks.Parent = this;

	GEngine.pWindow->keyboardManager.Register(&kCallbacks);

}
void Editor::Update()
{
	shaderEditor.Update(tabPanel.CurrentTab == 0);
	materialEditor.Update(tabPanel.CurrentTab == 1);
	if (materialEditor.RebuildPipeline)
	{
		for (u32 i = 0; i < SPBCount; i++)
		{
			shaderEditor.Editors[i].Update();
		}
		

		DX12Pipeline* pipeline = shaderEditor.CreatePipeline();

		if (pipeline)
		{
			memcpy(pipeline->PipelineName, "MaterialEditorPipeline", strlen("MaterialEditorPipeline"));
			objectManager->AddPipeline3D("MaterialEditorPipeline", pipeline);
		}
		materialEditor.RebuildPipeline = false;
	}
	tabPanel.Update();
}
void Editor::Draw()
{
	SceneViewport.StartRender(true);
	switch(tabPanel.CurrentTab)
	{
		case 0: // Shader editor
		{
			DX12Camera* Cam = SceneViewport.CManager.GetCamera(SceneViewport.Camera3D);
			Cam->MCallbacks.IsEnabled = false;
			SceneViewport.CManager.SetCamera(SceneViewport.Camera2D);
			shaderEditor.Draw();
			break;
		}
		case 1: //MaterialEditor
		{
			SceneViewport.CManager.SetCamera(SceneViewport.Camera3D);

			DefaultOnResizeBuffer OnResize3D;
			DX12Camera* Cam = SceneViewport.CManager.GetCurrentCamera();
			Cam->MCallbacks.IsEnabled = true;
			OnResize3D.CamPos = Cam->Position;
			OnResize3D.Proj = Cam->PMatrix;
			OnResize3D.View = Cam->VMatrix;

			//SceneViewport.PipelineManager.GetPipeline("Default")->UpdateVSOnResize(&OnResize3D);
			
			materialEditor.Draw();
			break;
		}
	}
	objectManager->Draw(&SceneViewport.Queue);

	SceneViewport.StopRender();
	
	ViewportObject->Draw();
	OptionsPanel->Draw();
	tabPanel.Draw();
}

void Editor::SaveAsset(const char* FilePath)
{
	char* RelFilePath = GetEngineRelativePath((char*)FilePath);
	if (!RelFilePath)
	{
		printf("Error: File Path: %s wasn't under the working directory.\n", FilePath);
		return;
	}
	DX12Object3D* Obj = &objectManager->Meshes3D[materialEditor.Object->MeshID];
	File file;
	file.Open(RelFilePath, "wb");
	//write header.
	for (u32 i = 0; i < SPBCount; i++)
	{
		file.Write(&shaderEditor.Editors[i].Contents.elementCount, sizeof(u32));
	}

	u32 VertexSize = Obj->VertCount * sizeof(Vertex);
	file.Write(&VertexSize, sizeof(u32));
	file.Write(&Obj->VertCount, sizeof(u32));

	u32 IndexSize = Obj->IndexCount * sizeof(u32);
	file.Write(&IndexSize, sizeof(u32));
	file.Write(&Obj->IndexCount, sizeof(u32));

	u32 TextureCount = materialEditor.MaterialOptions.Selectors.elementCount;
	file.Write(&TextureCount, sizeof(u32));

	char Buffer[MAX_PATH] = { 0 };
	memcpy(Buffer, RelFilePath, strlen(RelFilePath));
	file.Write(Buffer, MAX_PATH);

	//shaders
	for (u32 i = 0; i < SPBCount; i++)
	{
		file.Write(shaderEditor.Editors[i].Contents.data, shaderEditor.Editors[i].Contents.elementCount);
	}
	DX12Pipeline* List = DYNAMIC_ARR_GET_CAST_DATA(DX12Pipeline, SceneViewport.PipelineManager.PipelineList3D);
	if (List[materialEditor.Object->PipelineID].FilePath && strlen(List[materialEditor.Object->PipelineID].FilePath) > 0)
	{
		file.Write(&List[materialEditor.Object->PipelineID].FilePath, MAX_PATH);
	}
	else
	{
		u32 Index = GetLastCharIndex(RelFilePath, '/') + 1;
		u32 ExtIndex = GetLastCharIndex(RelFilePath, '.') ;

		RelFilePath[ExtIndex] = 0;
		char buffer[260] = {0};
		sprintf_s(buffer, "Application/Shaders/CompiledShaders/%sPipeline.desc",&RelFilePath[Index]);
		File PipelineFile;
		PipelineFile.Open(buffer, "wb");
		DX12PipelineDesc2 desc = shaderEditor.GetPipelineDesc();
		u32 Size = 0;
		void* Data = desc.Serialize(&Size);
		PipelineFile.Write(Data, Size);
		PipelineFile.Close();

		file.Write(buffer,MAX_PATH);

	}

	file.Write(Obj->VertList, VertexSize);
	file.Write(Obj->IndexList,IndexSize);

	FileSelector* Selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, materialEditor.MaterialOptions.Selectors);
	for (u32 i = 0; i < TextureCount; i++)
	{
		file.Write(&Selectors[i].ContentFileInfo.FilePath, MAX_PATH);
	}
	file.Close();

	
}