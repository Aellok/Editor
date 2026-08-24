#pragma once
#include "System\Types.h"
#include "System\Renderer\Object2D.h"
#include "System\Renderer\Viewport.h"
#include "System\Renderer\ObjectManager.h"
#include "Application/UI/Button.h"
#include "Application/UI/TextEditor.h"
#include "Application/Editor/EditorLayout/TabPanel.h"
#include "Application/Editor/MaterialEditor/MaterialEditorLayout/MaterialOptionsPanel.h"
#include "Platform/Windows/Renderer/DirectX12/DX12PipelineManager.h"
#include "ShaderEditor\ShaderEditor.h"
#include "MaterialEditor\MaterialEditor.h"
#include "System/KeyboardManager.h"
struct Editor
{
	KeyboardCallbacks kCallbacks;
	ShaderEditor shaderEditor;
	MaterialEditor materialEditor;

	TabPanel tabPanel;

	Object2D* OptionsPanel;

	
	//Viewport Related
	Viewport SceneViewport;
	DX12Texture ViewportTexture;
	Object2D* ViewportObject;
	ObjectManager* objectManager;

	bool PipelineChanged;

	//TODO: pass whether control is down to the key up/down functions.
	bool ctrl;

	void Init();
	void Update();
	void Draw();

	void SaveAsset(const char* FilePath);
};