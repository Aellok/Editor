#pragma once
#include "System\MouseManager.h"
#include "Application/UI/TextEditor.h"
#include "Application\UI\Button.h"
enum EShaderPanelButtons
{
	eVertexShader,
	eHullShader,
	eDomainShader,
	eGeometryShader,
	ePixelShader,
	SPBCount
};

struct ShaderEditorPanel
{
	MouseCallbacks mCallbacks;
	EShaderPanelButtons CurrentEditor;
	UIButton* ShaderPanelButtons;
	Object2D* Background;

	void Init(MouseManager* mManager, ObjectManager* Manager, Vector Pos, Vector Dim, Vector Color);
	void Update();
	void Draw();
	void SelectEditor(EShaderPanelButtons Editor);
};