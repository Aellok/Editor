#pragma once
#include "System\Types.h"
#include "System\Renderer\Object2D.h"
#include "System\Renderer\Viewport.h"
#include "System\Renderer\ObjectManager2D.h"
#include "Application/UI/Button.h"
#include "Application/Editor/ShaderEditor/ShaderEditorCallbackDefines.h"

#include "Application/Editor/MaterialEditor/MaterialEditorLayout/MaterialOptionsPanel.h"
#include "Platform/Windows/Renderer/DirectX12/DX12PipelineManager.h"
#include "ShaderEditorLayout\ShaderEditorPanel.h"
#include "../MaterialEditor/MaterialEditorCallbackDefines.h"
struct ShaderEditor
{
	TextEditor* Editors;
	ShaderEditorPanel Panel;
	
	KeyboardCallbacks kcb;
	MouseCallbacks mcb;

	u32 RegisterCount;
	//DataString
	DynamicArray RegisterNames;

	f32 ParseTimer;

	//OnRegisterChanged
	DynamicArray PSRegistersChanged;
	
	//OnRegisterChangedParams
	DynamicArray PSRegistersChangedParams;

	void Init(MouseManager* mManager, KeyboardManager* kManager, ObjectManager* SceneObjManager, f32 PanelWidth, Vector Pos, Vector ViewportDim, Vector Color);
	void Draw();
	void Update(bool Enabled);
	DX12PipelineDesc2 GetPipelineDesc();
	DX12Pipeline* CreatePipeline();
	void UpdatePSRegisters();
	void RegisterOnPSRegisterChanged(OnRegisterChanged Callback,OnRegisterChangedParams PSRegistersChangedParams);

};

void ShaderEditor_OnPropertyChanged(OnPropertyAddedParams Param);
void ShaderEditor_OnObjectChanged(ObjectChangeInfo Info);
