#pragma once
#include "MaterialEditorLayout\MaterialOptionsPanel.h"
#include "Application\Editor\ShaderEditor\ShaderEditorCallbackDefines.h"
struct ShaderEditor;
struct MaterialEditor
{
	MaterialOptionsPanel MaterialOptions;
	Object3D* Object;
	
	//OnPropertyChanged
	DynamicArray OnPropertyChanged;

	ObjectManager2D* ObjManager;

	MouseCallbacks Callbacks;

	void Init(MouseManager* ViewportManager,ObjectManager2D* Manager, Vector Pos, Vector Dim, Vector Color);
	void Update(bool Enabled);
	void Draw();
};

void MaterialEditor_OnPSRegistersChanged(OnRegisterChangedParams params);