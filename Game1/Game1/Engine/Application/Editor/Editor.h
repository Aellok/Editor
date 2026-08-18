#pragma once
#include "System\Types.h"
#include "System\Renderer\Object2D.h"
#include "System\Renderer\Viewport.h"
#include "System\Renderer\ObjectManager2D.h"
#include "Application/UI/Button.h"
#include "Application/UI/TextEditor.h"
#include "Application/Editor/EditorLayout/TabPanel.h"
#include "Application/Editor/MaterialEditor/MaterialEditorLayout/MaterialOptionsPanel.h"
#include "Platform/Windows/Renderer/DirectX12/DX12PipelineManager.h"
#include "ShaderEditor\ShaderEditor.h"
#include "MaterialEditor\MaterialEditor.h"

struct Editor
{
	ShaderEditor shaderEditor;
	MaterialEditor materialEditor;

	TabPanel tabPanel;

	Object2D* OptionsPanel;

	
	//Viewport Related
	Viewport SceneViewport;
	DX12Texture ViewportTexture;
	Object2D* ViewportObject;
	ObjectManager2D* ObjectManager;

	bool PipelineChanged;

	void Init();
	void Update();
	void Draw();

};