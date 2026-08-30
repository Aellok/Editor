#include "Button.h"
#include "Application\Engine.h"
void UIButton_LButtonDown(void* Parent, Mouse mouse)
{
	UIButton* Button = (UIButton*)Parent;
	if (ContainsPoint(mouse,Button->Base->Pos, Button->Base->Dim) && Button->UserCallbacks.OnLButtonDown)
	{
		Button->UserCallbacks.OnLButtonDown(Button->UserCallbacks.Parent, mouse);
	}
}
void UIButton_LButtonUp(void* Parent, Mouse mouse)
{
	UIButton* Button = (UIButton*)Parent;
	if (ContainsPoint(mouse, Button->Base->Pos, Button->Base->Dim) && Button->UserCallbacks.OnLButtonUp)
	{
		Button->UserCallbacks.OnLButtonUp(Button->UserCallbacks.Parent, mouse);
	}
}
void UIButton_RButtonDown(void* Parent, Mouse mouse)
{
	UIButton* Button = (UIButton*)Parent;
	if (ContainsPoint(mouse, Button->Base->Pos, Button->Base->Dim) && Button->UserCallbacks.OnRButtonDown)
	{
		Button->UserCallbacks.OnRButtonDown(Button->UserCallbacks.Parent, mouse);
	}
}
void UIButton_RButtonUp(void* Parent, Mouse mouse)
{
	UIButton* Button = (UIButton*)Parent;
	if (ContainsPoint(mouse, Button->Base->Pos, Button->Base->Dim) && Button->UserCallbacks.OnRButtonUp)
	{
		Button->UserCallbacks.OnRButtonUp(Button->UserCallbacks.Parent, mouse);
	}
}
void UIButton_MButtonDown(void* Parent, Mouse mouse)
{
	UIButton* Button = (UIButton*)Parent;
	if (ContainsPoint(mouse, Button->Base->Pos, Button->Base->Dim) && Button->UserCallbacks.OnMButtonDown)
	{
		Button->UserCallbacks.OnMButtonDown(Button->UserCallbacks.Parent, mouse);
	}
}
void UIButton_MButtonUp(void* Parent, Mouse mouse)
{
	UIButton* Button = (UIButton*)Parent;
	if (ContainsPoint(mouse, Button->Base->Pos, Button->Base->Dim) && Button->UserCallbacks.OnMButtonUp)
	{
		Button->UserCallbacks.OnMButtonUp(Button->UserCallbacks.Parent, mouse);
	}
}
void UIButton_Move(void* Parent, Mouse mouse)
{
	UIButton* Button = (UIButton*)Parent;
	if (ContainsPoint(mouse, Button->Base->Pos, Button->Base->Dim) && Button->UserCallbacks.OnMove)
	{
		Button->UserCallbacks.OnMove(Button->UserCallbacks.Parent, mouse);
	}
}

void UIButton::Initialize(ButtonDesc Desc)
{
	desc = Desc;
	Color = Desc.Color;
	SelectedColor = Desc.SelectedColor;
	UserCallbacks = Desc.Callbacks;
	CREATE_MOUSE_CALLBACK(this, mCallbacks, UIButton);
	Desc.Manager->Register(&mCallbacks);

	Text = Desc.ObjectManager->AddString(Desc.Text, Desc.Pos, Desc.TextSize,"Font");
	
	if (Desc.ShrinkToText)
	{
		Desc.Dim.m128_f32[0] = Text->PixelLength;
		Desc.Dim.m128_f32[1] = Text->PixelHeight;
	}
	if (Desc.Center)
	{
		Text->Pos.m128_f32[0] = Desc.Pos.m128_f32[0] + (Desc.Dim.m128_f32[0] / 2) - (Text->PixelLength / 2);
		Text->Pos.m128_f32[1] = Desc.Pos.m128_f32[1] + (Desc.Dim.m128_f32[1] / 2) - (Text->PixelHeight / 2);

		Desc.ObjectManager->UpdateString(Text, (char*)Desc.Text, Desc.TextSize ,0);
	}
	Base = Desc.ObjectManager->AddObject(Desc.Pos, Desc.Dim, { 0.0f,0.0f }, Desc.Color,"TopLeft",nullptr,"Ortho");
	
}
void UIButton::Draw()
{
	Base->Draw();
	Text->Draw();
}
void UIButton::SetPos(Vector Pos)
{
	Base->Pos = Pos;
	Text->Pos = Pos;
	desc.Pos = Pos;
	if (desc.Center)
	{
		Text->Pos.m128_f32[0] = desc.Pos.m128_f32[0] + (desc.Dim.m128_f32[0] / 2) - (Text->PixelLength / 2);
		Text->Pos.m128_f32[1] = desc.Pos.m128_f32[1] + (desc.Dim.m128_f32[1] / 2) - (Text->PixelHeight / 2);
	}
	desc.ObjectManager->UpdateString(Text, (char*)desc.Text, desc.TextSize, 0);
	
}
void UIButton::SetAcceptInput(bool Value)
{
	mCallbacks.IsEnabled = Value;
}