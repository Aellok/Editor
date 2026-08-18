#include "MaterialEditor.h"
#include "Application\Engine.h"
#include "System/Utils/PNG.h"
#include "Application\Editor\ShaderEditor\ShaderEditor.h"
#include "System/Utils/ObjLoader.h"
#include "System\Utils\File.h"
void MaterialEditor_OnPropertyChanged(PropertyChangeInfo Info)
{
	MaterialEditor* Editor = (MaterialEditor*)Info.Parent;

	switch(Info.PropertyType)
	{
		case 0: // File Selector
		{
			FileSelector* fs = (FileSelector*)Info.PropertyObject;
			char* Contents = (char*)Info.NewData;
			if (strlen(Contents) > 0)
			{
				Editor->ObjManager->AddTexture(Contents);
				Editor->Object->TextureID = Editor->ObjManager->TextureCount - 1;
			}			
			break;
		}
		case 1:
		{
			break;
		}
	}
}
enum MaterialEditor_DragDropTypes
{
	eME_Obj,
	eME_Asset,
	eME_Total
};

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
	MaterialEditor* Editor = (MaterialEditor*)Parent;
	switch(GetFileExtensionType(FileName))
	{
		case eME_Obj:
		{
			ObjectDesc desc = { 0 };
			desc.Dim = { 1,1,1 };
			desc.Pos = { 0,0,0 };
			desc.Color = { 1,1,1,1 };
			desc.TextureName = "Textures/BlankTextureSheet.png";
			desc.PipelineName = "Default";
			desc.ModelFileName = FileName;
			Editor->Object = Editor->ObjManager->AddObject3D(desc);
			break;
		}
		case eME_Asset:
		{
			break;
		}
	}
}

void MaterialEditor::Init(MouseManager* ViewportManager,ObjectManager2D* Manager, Vector Pos, Vector Dim, Vector Color)
{
	ObjManager = Manager;
	Object = Manager->AddObject3D({ 0,0,0 }, { 0.5f,0.5f,0.5f }, { 0,0,0 }, { 1,1,1,1 }, "Models/Sphere.obj", "Textures/BlankTextureSheet.png", "Default");
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
		Object->Rot.m128_f32[1] += 0.01f;
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
	Object->Draw();
};
void MaterialEditor_OnPSRegistersChanged(OnRegisterChangedParams params)
{
	MaterialEditor* Editor = (MaterialEditor*)params.Parent;
	DataString* Names = DYNAMIC_ARR_GET_CAST_DATA(DataString, params.RegisterNames);

	for (s32 i = Editor->MaterialOptions.Selectors.elementCount - 1; i >= (s32)params.RegisterCount;i--)
	{
		//delete the other ones.
		Editor->MaterialOptions.Selectors.Delete(i);
	}
	for (u32 i = Editor->MaterialOptions.Selectors.elementCount; i < params.RegisterCount; i++)
	{
		FileSelector* Selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Editor->MaterialOptions.Selectors);
		Vector BasePos = { 0,20,0.1 };
		if (i > 0)
		{
			BasePos = Selectors[i - 1].BasePosition;
			BasePos.m128_f32[1] += 60;
		}
		
		Editor->MaterialOptions.AddSelector(&GEngine.pWindow->mouseManager, BasePos, Names[i].Buffer, "");
		
		Selectors[i].AddPropertyChangeCallback(params.Parent, MaterialEditor_OnPropertyChanged);
	}

	
	for (u32 i = 0; i < params.RegisterCount; i++)
	{
		FileSelector* Selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Editor->MaterialOptions.Selectors);
		Selectors[i].UpdateLabel(Names[i].Buffer, strlen(Names[i].Buffer));
	}
	
}