#include "ShaderEditorPanel.h"
#include "Application/Engine.h"
#include "Application\UI\Button.h"
void VertexShader_OnLButtonUp(void* Parent, Mouse mouse)
{
	ShaderEditorPanel* SEI = (ShaderEditorPanel*)Parent;
	SEI->SelectEditor(eVertexShader);
}
void HullShader_OnLButtonUp(void* Parent, Mouse mouse)
{
	ShaderEditorPanel* SEI = (ShaderEditorPanel*)Parent;
	SEI->SelectEditor(eHullShader);
}
void DomainShader_OnLButtonUp(void* Parent, Mouse mouse)
{
	ShaderEditorPanel* SEI = (ShaderEditorPanel*)Parent;
	SEI->SelectEditor(eDomainShader);
}
void GeometryShader_OnLButtonUp(void* Parent, Mouse mouse)
{
	ShaderEditorPanel* SEI = (ShaderEditorPanel*)Parent;
	SEI->SelectEditor(eGeometryShader);
}
void PixelShader_OnLButtonUp(void* Parent, Mouse mouse)
{
	ShaderEditorPanel* SEI = (ShaderEditorPanel*)Parent;
	SEI->SelectEditor(ePixelShader);

}

typedef void (*OnButtonUpCallback)(void* Parent, Mouse mouse);
OnButtonUpCallback OnLButtonUpCallbacks[SPBCount]
{
	VertexShader_OnLButtonUp,
	HullShader_OnLButtonUp,
	DomainShader_OnLButtonUp,
	GeometryShader_OnLButtonUp,
	PixelShader_OnLButtonUp,
};

const char* ShaderPanelButtonNames[SPBCount]
{
	"Vertex Shader",
	"Hull Shader",
	"Domain Shader",
	"Geometry Shader",
	"Pixel Shader"
};
void ShaderEditorPanel::Init(MouseManager* mManager,ObjectManager* Manager,Vector Pos,Vector Dim,Vector Color)
{
	Background = Manager->AddObject({ 0.0f,Pos.m128_f32[1],1.0f}, {Dim.m128_f32[0],(float)GEngine.pWindow->height - Pos.m128_f32[1]},
		{ 0.0f,0.0f }, Color, "TopLeft", NULL, "Ortho");
	ShaderPanelButtons = (UIButton*)GEngine.Global.Allocate(sizeof(UIButton) * SPBCount);

	for (u32 i = 0; i < SPBCount; i++)
	{
		ButtonDesc desc = { 0 };
		desc.Center = true;
		desc.Pos = Pos;
		desc.Dim = Dim;
		desc.ObjectManager = Manager;
		desc.Text = ShaderPanelButtonNames[i];
		desc.TextSize = 32;
		desc.Manager = mManager;
		desc.Callbacks.OnLButtonUp = OnLButtonUpCallbacks[i];
		desc.Callbacks.Parent = this;
		desc.Color = { 0.5f,0.5f,0.5,1.0f };
		desc.SelectedColor = { 0.25f,0.25f,0.25,1.0f };
		ShaderPanelButtons[i].Initialize(desc);

		Pos.m128_f32[1] += Dim.m128_f32[1] + 10; // 10 for the padding.
	}
	CurrentEditor = SPBCount;
	SelectEditor(eVertexShader);
}
void ShaderEditorPanel::Update()
{

}
void ShaderEditorPanel::Draw()
{
	Background->Draw();
	for (u32 i = 0; i < SPBCount; i++)
	{
		ShaderPanelButtons[i].Draw();
	}
}
void ShaderEditorPanel::SelectEditor(EShaderPanelButtons Editor)
{
	if (CurrentEditor != SPBCount)
	{
		ShaderPanelButtons[CurrentEditor].Base->Color = ShaderPanelButtons[CurrentEditor].Color;
	}

	ShaderPanelButtons[Editor].Base->Color = ShaderPanelButtons[Editor].SelectedColor;
	CurrentEditor = Editor;
}