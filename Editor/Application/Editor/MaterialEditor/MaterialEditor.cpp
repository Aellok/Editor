#include "MaterialEditor.h"
#include "Application\Engine.h"
#include "System/Utils/PNG.h"
#include "Application\Editor\Editor.h"
#include "System/Utils/ObjLoader.h"
#include "System\Utils\File.h"


enum MaterialEditor_DragDropTypes
{
	eME_Obj,
	eME_Asset,
	eME_Total
};
void MaterialEditor_OnPropertyChanged(PropertyChangeInfo Info)
{
	MaterialEditor* Editor = (MaterialEditor*)Info.Parent;

	switch(Info.PropertyType)
	{
		case 0: // File Selector
		{
			FileSelector* fs = (FileSelector*)Info.PropertyObject;
			FileSelector* Selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Editor->MaterialOptions.Selectors);

			u32 TextureIndex = 0;
			for (u32 i = 0; i < Editor->MaterialOptions.Selectors.elementCount; i++)
			{
				if (&Selectors[i] == fs)
				{
					TextureIndex = i;
				}
			}

			char* Contents = (char*)Info.NewData;
			if (strlen(Contents) > 0)
			{
				Editor->ObjManager->AddTexture(Contents);
				Editor->Object->TextureID[TextureIndex] = Editor->ObjManager->TextureCount - 1;
				Editor->Object->TextureCount = Editor->MaterialOptions.Selectors.elementCount;
				Editor->RebuildPipeline = true;
			}			
			break;
		}
		case 1:
		{
			break;
		}
	}
}
void MaterialEditor_OnPSRegistersChanged(OnRegisterChangedParams params)
{
	MaterialEditor* Editor = (MaterialEditor*)params.Parent;
	DataString* Names = DYNAMIC_ARR_GET_CAST_DATA(DataString, params.RegisterNames);
	DynamicArray* CurrentTextureSlots = &Editor->MaterialOptions.TextureSelectorSlots;
	FileSelector* Selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Editor->MaterialOptions.Selectors);
	u32* Slots = DYNAMIC_ARR_GET_CAST_DATA(u32, params.Slots);

	if (Editor->MaterialOptions.Selectors.elementCount > 0)
	{
		for (s32 i = Editor->MaterialOptions.Selectors.elementCount - 1; i > params.RegisterCount; i--)
		{
			//delete the other ones.
			Editor->MaterialOptions.Selectors.Delete(i);
		}
	}
	
	for (u32 i = Editor->MaterialOptions.Selectors.elementCount; i < params.RegisterCount; i++)
	{
			
		Vector BasePos = { 0,20,0.1 };
		if (i > 0)
		{
			BasePos = Selectors[i - 1].BasePosition;
			BasePos.m128_f32[1] += 60;
		}

		Editor->MaterialOptions.AddSelector(&GEngine.pWindow->mouseManager, BasePos, "", "");

		Selectors[i].AddPropertyChangeCallback(params.Parent, MaterialEditor_OnPropertyChanged);
	}

	for (u32 i = 0; i < params.RegisterCount; i++)
	{
		FileSelector* Selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Editor->MaterialOptions.Selectors);
		Selectors[i].UpdateLabel(Names[Slots[i]].Buffer, strlen(Names[Slots[i]].Buffer));
	}
	
}


const char* FileExtensions[eTotal] =
{
	"obj",
	"asset",
};

u32 GetFileExtensionType(const char* FilePath)
{
	char Buffer[32] = {0};
	GetFileExtension(FilePath, Buffer,32);
	for (u32 i = 0; i < eME_Total; i++)
	{
		if (!strcmp(Buffer,FileExtensions[i]))
		{
			return i;
		}
	}
	return eME_Total;
}
void MaterialEditor_OnDragDrop(void* Parent, Mouse mouse,char* FileName)
{
	MaterialEditor* MatEditor = (MaterialEditor*)Parent;
	switch(GetFileExtensionType(FileName))
	{
		case eME_Obj:
		{
			ObjectDesc desc = { 0 };
			desc.Dim = { 1,1,1 };
			desc.Pos = { 0,0,0 };
			desc.Color = { 1,1,1,1 };
			memset(desc.TextureName, 0, sizeof(const char*) * 8);
			desc.PipelineName = "MaterialEditorPipeline";
			desc.ModelFileName = FileName;

			MatEditor->ObjManager->AddPipeline3D("MaterialEditorPipeline", "Application/Shaders/CompiledShaders/DefaultShader.desc", true, false);
			

			MatEditor->Object = MatEditor->ObjManager->AddObject3D(desc);

			File VSShader;
			File PSShader;
			
			VSShader.Load("Application/Shaders/HLSLShaders/DefaultVS.hlsl");
			PSShader.Load("Application/Shaders/HLSLShaders/DefaultPS.hlsl");

			ObjectChangeInfo Params = {0};
			Params.PipelineName = "MaterialEditorPipeline";

			Params.ShaderSize[eVertexShader] = VSShader.FileSize;
			Params.ShaderSize[ePixelShader] = PSShader.FileSize;

			Params.ShaderFileData[eVertexShader] = (char*)VSShader.Data;
			Params.ShaderFileData[ePixelShader] = (char*)PSShader.Data;

			ObjectChanged* Callbacks = DYNAMIC_ARR_GET_CAST_DATA(ObjectChanged, MatEditor->OnObjectChanged);
			void** Parents = DYNAMIC_ARR_GET_CAST_DATA(void*, MatEditor->OnObjectChangedParent);
			for (u32 i = 0; i < MatEditor->OnObjectChanged.elementCount; i++)
			{
				Params.Parent = Parents[i];
				Callbacks[i](Params);
			}
			VSShader.Close();
			PSShader.Close();

			break;
		}
		case eME_Asset:
		{
			
			MatEditor->CurrentAsset = LoadAsset(FileName);
			
			ObjectDesc desc = { 0 };
			desc.Dim = { 1,1,1 };
			desc.Pos = { 0,0,0 };
			desc.Color = { 1,1,1,1 };
			
			desc.PipelineName = "MaterialEditorPipeline";
			desc.ModelFileName = FileName;

			MatEditor->ObjManager->AddMesh3D(FileName, MatEditor->CurrentAsset->Verticies, MatEditor->CurrentAsset->Header.VertexCount, 
													   MatEditor->CurrentAsset->Header.VertexSize, MatEditor->CurrentAsset->Indicies, 
													   MatEditor->CurrentAsset->Header.IndexCount, MatEditor->CurrentAsset->Header.IndexSize);
			
			for (u32 i = 0; i < MatEditor->CurrentAsset->Header.TextureCount; i++)
			{
				MatEditor->ObjManager->AddTexture(MatEditor->CurrentAsset->Textures[i]);
				desc.TextureName[i] = MatEditor->CurrentAsset->Textures[i];
			}

			//add pipeline.
			MatEditor->ObjManager->AddPipeline3D("MaterialEditorPipeline", MatEditor->CurrentAsset->PipelineName, true, false);
			MatEditor->Object = MatEditor->ObjManager->AddObject3D(desc);
			
			ObjectChangeInfo Params;
			Params.PipelineName = "MaterialEditorPipeline";
			memcpy(Params.ShaderSize, MatEditor->CurrentAsset->Header.ShaderSize,sizeof(u32) * SPBCount );
			
			for (u32 i = 0; i < SPBCount; i++)
			{
				Params.ShaderFileData[i] = Params.ShaderSize[i] > 0 ? MatEditor->CurrentAsset->ShaderName[i] : (char*)NULL;
			}

			ObjectChanged* Callbacks = DYNAMIC_ARR_GET_CAST_DATA(ObjectChanged, MatEditor->OnObjectChanged);
			void** Parents = DYNAMIC_ARR_GET_CAST_DATA(void*, MatEditor->OnObjectChangedParent);
			for (u32 i = 0; i < MatEditor->OnObjectChanged.elementCount; i++)
			{
				Params.Parent = Parents[i];
				Callbacks[i](Params);
			}
			
			FileSelector* Selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, MatEditor->MaterialOptions.Selectors);

			for (u32 i = 0; i < MatEditor->MaterialOptions.Selectors.elementCount; i++)
			{
				const char* Texture = MatEditor->CurrentAsset->Textures[i];
				u32 ExtIndex = GetLastCharIndex(Texture, '.');
				u32 SlashIndex = GetLastCharIndex(Texture, '/');
				u32 BackSlashIndex = GetLastCharIndex(Texture, '\\');
				u32 Start = (SlashIndex == 0 ? BackSlashIndex : SlashIndex) + 1;

				Selectors[i].UpdateContent(Texture + Start, ExtIndex - Start);
				strcpy_s(Selectors[i].ContentFileInfo.FilePath, Texture);
			}

			break;
		}
	}
}

void MaterialEditor::Init(MouseManager* ViewportManager,ObjectManager* Manager,Editor* InEditor, Vector Pos, Vector Dim, Vector Color)
{
	CurrentAsset = NULL;
	RebuildPipeline = false;
	editor = InEditor;
	ObjManager = Manager;
	Object = NULL;// Manager->AddObject3D({ 0,0,0 }, { 0.5f,0.5f,0.5f }, { 0,0,0 }, { 1,1,1,1 }, "Models/Sphere.obj", "Textures/BlankTextureSheet.png", "Default");

	OnObjectChanged.Init(4,sizeof(ObjectChanged));
	OnObjectChangedParent.Init(4, sizeof(void*));

	MaterialOptions.Init(GEngine.pObjManager2D, Pos, Dim, Color);
	MaterialOptions.AddOnPropertyChangedCallback(this,MaterialEditor_OnPropertyChanged);
	
	memset(&Callbacks, 0, sizeof(MouseCallbacks));
	Callbacks.Parent = this;
	Callbacks.OnDragDrop = MaterialEditor_OnDragDrop;
	ViewportManager->Register(&Callbacks);
};
void MaterialEditor::Update(bool Enabled)
{
	
	if (Enabled)
	{
		Callbacks.IsEnabled = true;
		MaterialOptions.EnableInputs();
		if (Object)
		{
			Object->Rot.m128_f32[1] += 0.01f;
		}
		
		MaterialOptions.Update();
	}
	else
	{
		Callbacks.IsEnabled = false;
		MaterialOptions.DisableInputs();
	}
};
void MaterialEditor::Draw()
{
	MaterialOptions.Draw(); 
	if (Object)
	{
		Object->Draw();
	}
	
};

void MaterialEditor::AddOnObjectChangedCallback(void* Parent, ObjectChanged Callback)
{
	OnObjectChanged.Add(&Callback);
	OnObjectChangedParent.Add(&Parent);
}