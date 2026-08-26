#pragma once
#include "MaterialEditorLayout\MaterialOptionsPanel.h"
#include "Application\Editor\ShaderEditor\ShaderEditorCallbackDefines.h"
#include "Application/Asset/Asset.h"
struct ShaderEditor;
struct Editor;
struct MaterialEditor
{
	MaterialOptionsPanel MaterialOptions;
	Asset* CurrentAsset;
	Object3D* Object;
	Editor* editor;


	//OnPropertyChanged
	DynamicArray OnPropertyChanged;

	//ObjectChanged
	DynamicArray OnObjectChanged;
	//void*
	DynamicArray OnObjectChangedParent;

	ObjectManager* ObjManager;

	MouseCallbacks Callbacks;

	bool RebuildPipeline;

	void AddOnObjectChangedCallback(void* Parent,ObjectChanged Callback);
	void Init(MouseManager* ViewportManager,ObjectManager* Manager,Editor* InEditor, Vector Pos, Vector Dim, Vector Color);
	void Update(bool Enabled);
	void Draw();
};

void MaterialEditor_OnPSRegistersChanged(OnRegisterChangedParams params);