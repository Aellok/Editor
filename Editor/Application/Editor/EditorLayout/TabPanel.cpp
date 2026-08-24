#include "TabPanel.h"
#include "System/Renderer/ObjectManager.h"
#include "Application/UI/Button.h"
#include "Application/Engine.h"

void TabPanel_LButtonDown(void* Parent,Mouse mouse)
{
	TabPanel* TPanel = (TabPanel*)Parent;

	UIButton* Button = DYNAMIC_ARR_GET_CAST_DATA(UIButton, TPanel->Tabs);
	for (u32 i = 0; i < TPanel->Tabs.elementCount; i++)
	{
		if (ContainsPoint(mouse,Button[i].Base->Pos, Button[i].Base->Dim))
		{
			Button[TPanel->CurrentTab].Base->Color = Button[TPanel->CurrentTab].Color;
			Button[i].Base->Color = Button[i].SelectedColor;
			TPanel->CurrentTab = i;
		}
	}
	
}
void TabPanel::Init(void* Parent,ObjectManager* InManager,Vector PanelDim,Vector PanelPos,Vector Color)
{
	Manager2D = InManager;
	CurrentTab = 0;
	TabOffset = 0;
	Background = Manager2D->AddObject(PanelPos, PanelDim,{ 0.0f,0.0f }, Color, "TopLeft", NULL, "Ortho");

	Tabs.Init(4, sizeof(UIButton));
}
void TabPanel::AddTab(f32 TabWidth, const char* Name)
{
	UIButton* NewTab = (UIButton*)calloc(1,sizeof(UIButton));
	ButtonDesc desc = { 0 };
	desc.Center = true;
	desc.Pos = { Background->Pos.m128_f32[0] + TabOffset, Background->Pos.m128_f32[1],0.1f };
	desc.Dim = { TabWidth,Background->Dim.m128_f32[1] };

	desc.ObjectManager = Manager2D;
	desc.Text = Name;
	desc.TextSize = 16;
	desc.Manager = &GEngine.pWindow->mouseManager;
	desc.Callbacks.OnLButtonUp = TabPanel_LButtonDown;
	desc.Callbacks.Parent = this;
	desc.Color = { 0.5f,0.5f,0.5,1.0f };
	desc.SelectedColor = { 0.25f,0.25f,0.25,1.0f };

	NewTab->Initialize(desc);
	Tabs.Add(NewTab);
	if (Tabs.elementCount - 1 == CurrentTab)
	{
		NewTab->Base->Color = NewTab->SelectedColor;
	}

	TabOffset += TabWidth;
}
void TabPanel::Draw()
{
	Background->Draw();
	UIButton* Button = DYNAMIC_ARR_GET_CAST_DATA(UIButton, Tabs);
	for (u32 i = 0; i < Tabs.elementCount; i++)
	{
		Button[i].Draw();
	}
}
void TabPanel::Update()
{
	
}
void TabPanel::CloseTab(u32 Index)
{

}
