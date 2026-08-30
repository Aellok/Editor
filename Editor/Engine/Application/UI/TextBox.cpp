#include "TextBox.h"

void TextBox_OnKeyDown(void* Parent,u32 Key)
{
	TextBox* textBox = (TextBox*)Parent;
	if (textBox->IsClicked)
	{
		
	}
}
void TextBox_OnKeyUp(void* Parent, u32 Key)
{
	TextBox* textBox = (TextBox*)Parent;
	if (textBox->IsClicked)
	{

	}
}
void TextBox_LButtonDown(void* Parent, Mouse mouse)
{
	TextBox* textBox = (TextBox*)Parent;
	if (textBox->IsClicked)
	{

	}
}

void TextBox::Init(ObjectManager* Manager2D,MouseManager* MManager,KeyboardManager* KManager,Vector Pos, Vector Dim,void(*OnChanged)())
{
	IsClicked = false;
	Content.Init(255, sizeof(char));
	const char* Default = "Hello World!";
	Content.Copy((void*)Default, strlen(Default));
	Manager = Manager2D;
	
	float PanelPadding = 20.0f;

	Vector BackgroundPos = Pos;
	BackgroundPos.m128_f32[0] += PanelPadding;
	BackgroundPos.m128_f32[1] += PanelPadding;
	Background = Manager->AddObject(BackgroundPos, Dim, { 0,0 }, { 0,0,0,1 }, "TopLeft", NULL, "Ortho");
	
	Vector TextPos = BackgroundPos;
	TextPos.m128_f32[0] += Dim.m128_f32[1] / 6;
	TextPos.m128_f32[1] += Dim.m128_f32[1] / 6;
	String = Manager->AddString("", TextPos, 32,"Font");

	mCallbacks.IsEnabled = true;
	mCallbacks.Parent = this;
	mCallbacks.OnLButtonDown = TextBox_LButtonDown;
	
	kCallbacks.Parent = this;
	kCallbacks.IsEnabled = true;
	kCallbacks.OnKeyUp = TextBox_OnKeyUp;
	kCallbacks.OnKeyDown = TextBox_OnKeyDown;


	MManager->Register(&mCallbacks);
	KManager->Register(&kCallbacks);

	
}
void TextBox::Update()
{
	Manager->UpdateString(String,Content.data,32, 0);
}
void TextBox::Draw()
{
	Background->Draw();
	String->Draw();
}