#include "ScrollBar.h"

void ScrollBar_OnLButtonDown(void* Parent, Mouse mouse)
{
	ScrollBar* scrollBar =(ScrollBar*)Parent;

	scrollBar->Selected = ContainsPoint(mouse, scrollBar->Bar->Pos, scrollBar->Bar->Dim);
	scrollBar->Bar->Color = scrollBar->Selected ? scrollBar->SelectedColor : scrollBar->Color;
}
void ScrollBar_OnLButtonUp(void* Parent, Mouse mouse)
{
	ScrollBar* scrollBar = (ScrollBar*)Parent;
	scrollBar->Selected = false;
	scrollBar->Bar->Color = scrollBar->Color;
}
void ScrollBar_OnMove(void* Parent,Mouse mouse)
{
	ScrollBar* scrollBar = (ScrollBar*)Parent;
	if (scrollBar->Selected)
	{
		//TODO: this needs to change depending on if its a horizonatal or vertical
		scrollBar->Bar->Pos.m128_f32[1] = mouse.y - (scrollBar->Bar->Dim.m128_f32[1] / 2);

		f32 MinBaseY = scrollBar->Background->Pos.m128_f32[1];
		f32 MaxBaseY = MinBaseY + scrollBar->Background->Dim.m128_f32[1];
		
		//upper bound on the bar
		scrollBar->Bar->Pos.m128_f32[1] = (scrollBar->Bar->Pos.m128_f32[1] < MinBaseY) ? MinBaseY : scrollBar->Bar->Pos.m128_f32[1];
		//lower bond on the bar
		scrollBar->Bar->Pos.m128_f32[1] = (scrollBar->Bar->Pos.m128_f32[1] + scrollBar->Bar->Dim.m128_f32[1]) > MaxBaseY ? MaxBaseY : scrollBar->Bar->Pos.m128_f32[1];

		f32 RelCenter = scrollBar->Bar->Pos.m128_f32[1] + (scrollBar->Bar->Dim.m128_f32[1] / 2) - MinBaseY;
		scrollBar->Percentage = RelCenter / (MaxBaseY - MinBaseY);
		
		scrollBar->UpdateCallback(scrollBar->Parent,scrollBar->Percentage);

		printf("Percentage: %f\n", scrollBar->Percentage);
	}
}
/*
void ScrollBar_Scroll(void )
{

}
*/
void ScrollBar::Init(ObjectManager* Manager,MouseManager* mManager, Vector Pos, Vector Dim, Vector Rot, Vector BackgroundColor, Vector BarColor,OnScrollUpdate updateCallback,void* parent)
{
	Parent = parent;
	Color = BarColor;
	SelectedColor = { 0.25,0.25,0.25,1.0f };
	ObjectDesc desc = {0};
	desc.Color = BackgroundColor;
	desc.Dim = Dim;
	desc.Pos = Pos;
	desc.PipelineName = "Ortho";
	desc.Rot = Rot;
	desc.ModelFileName = "TopLeft";

	Background = Manager->AddObject(desc);
	desc.Pos.m128_f32[2] = 0.5f;
	desc.Dim.m128_f32[1] = desc.Dim.m128_f32[1] / 30;
	desc.Color = BarColor;
	Bar = Manager->AddObject(desc);
	MCallbacks = { 0 };
	MCallbacks.OnLButtonDown = ScrollBar_OnLButtonDown;
	MCallbacks.OnLButtonUp = ScrollBar_OnLButtonUp;
	MCallbacks.OnMove = ScrollBar_OnMove;
	MCallbacks.Parent = this;
	mManager->Register(&MCallbacks);
	UpdateCallback = updateCallback;
}
void ScrollBar::Update()
{

}
void ScrollBar::Draw()
{
	Background->Draw();
	Bar->Draw();
}