#include "TextEditor.h"
#include "System\Utils\File.h"
#include "Application\Engine.h"
#include "Platform\Windows\Utils\WinSaveInterface.h"
void TextEditor_KeyDown(void* Parent, u32 Key)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
	switch (Key)
	{
		case VK_SHIFT:
		{
			Editor->IsShiftDown = true;
			break;
		}
		case VK_CONTROL:
		{
			Editor->IsCtrlDown = true;
			break;
		}
		case VK_DELETE:
		{
			if (Editor->CursorPosX == Editor->String->Info.LineInfo->CharCount && Editor->CursorPosY == Editor->String->Info.LineCount - 1)
			{
				return;
			}
			Editor->DeleteCharacter(Editor->CursorPosX,Editor->CursorPosY);

			break;
		}
		case VK_BACK:
		{
			if (Editor->CursorPosX == 0 && Editor->CursorPosY == 0)
			{
				return;
			}
			
			Editor->DeleteCharacter(Editor->CursorPosX - 1, Editor->CursorPosY);
			
			if (Editor->CursorPosX == 0 && Editor->CursorPosY > 0)
			{
				Editor->CursorPosY--;
				Editor->CursorPosX = Editor->String->Info.LineInfo[Editor->CursorPosY].CharCount;
				return;
			}

			Editor->CursorPosX--;
			break;
		}
		case VK_UP:
		{
			Editor->CursorPosY -= Editor->CursorPosY > 0;
			Editor->CursorPosX = Editor->CursorPosX > Editor->String->Info.LineInfo[Editor->CursorPosY].CharCount ? Editor->String->Info.LineInfo[Editor->CursorPosY].CharCount : Editor->CursorPosX;
			break;
		}
		case VK_DOWN:
		{
			Editor->CursorPosY += Editor->CursorPosY < Editor->String->Info.LineCount - 1;
			Editor->CursorPosX = Editor->CursorPosX > Editor->String->Info.LineInfo[Editor->CursorPosY].CharCount ? Editor->String->Info.LineInfo[Editor->CursorPosY].CharCount : Editor->CursorPosX;
			break;
		}
		case VK_LEFT:
		{
			if (Editor->CursorPosX == 0 && Editor->CursorPosY > 0)
			{
				Editor->CursorPosX = Editor->String->Info.LineInfo[Editor->CursorPosY - 1].CharCount;
				Editor->CursorPosY--;
			}
			else
			{
				Editor->CursorPosX -= Editor->CursorPosX > 0;
			}
			
			break;
		}
		case VK_RIGHT:
		{
			Editor->CursorPosX += Editor->CursorPosX < Editor->String->Info.LineInfo[Editor->CursorPosY].CharCount;
			break;
		}
	
	}
}
void TextEditor_KeyUp(void* Parent, u32 Key)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
	Editor->Changed = true;
	switch (Key)
	{
		case VK_SHIFT:
		{
			Editor->IsShiftDown = false;
			break;
		}
		case VK_CONTROL:
		{
			Editor->IsCtrlDown = false;
			break;
		}
		case 'S':
		{
			if (Editor->IsCtrlDown)
			{
				if (!WinDialog_Save(Editor->Contents.data, Editor->Contents.elementCount * Editor->Contents.elementSize))
				{
					printf("Error: Failed to save editor contents.\n");
				}
				
				return;
			}
			break;
		}
		case 'L':
		{
			if (Editor->IsCtrlDown)
			{
				DialogInfo FileData = WinDialog_Load();
				if (!FileData.data)
				{
					printf("Error: Failed to load file data.\n");
					return;
				}
				Editor->Contents.Copy(FileData.data, FileData.Size);
				free(FileData.data);
				return;
			}
			break;
		}
		case VK_RETURN:
		{
			Editor->Contents.Insert(Editor->GetStringIndexFromXY(Editor->CursorPosX, Editor->CursorPosY), (void*)"\n");
			Editor->CursorPosY++;
			Editor->CursorPosX = 0;
			break;
		}
	}
	u32 Value = GEngine.DefaultFont->GetAsciiFromVK(Key);
	if (Value >= 32 && Value <= 127)
	{
		if (GEngine.DefaultFont->Glyphs[Value].Renderable || Key == ' ')
		{
			Editor->Contents.Insert(Editor->GetStringIndexFromXY(Editor->CursorPosX, Editor->CursorPosY), &Value);
			Editor->CursorPosX++;
		}
	}
	if (Key == '\t')
	{
		Editor->Contents.Insert(Editor->GetStringIndexFromXY(Editor->CursorPosX, Editor->CursorPosY), &Value);
		Editor->CursorPosX++;
	}
}
void TextEditor_LButtonDown(void* Parent, Mouse pMouse)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
	if (ContainsPoint(pMouse,Editor->Pos,Editor->Dim))
	{
		Editor->Cursor->Pos = Editor->GetCursorPosition(pMouse.x, pMouse.y);
	}
	
}
void TextEditor_RButtonDown(void* Parent, Mouse pMouse)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
}
void TextEditor_MButtonDown(void* Parent, Mouse pMouse)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
}
void TextEditor_LButtonUp(void* Parent, Mouse pMouse)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
}
void TextEditor_RButtonUp(void* Parent, Mouse pMouse)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
}
void TextEditor_MButtonUp(void* Parent, Mouse pMouse)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
}
void TextEditor_Move(void* Parent, Mouse pMouse)
{
	TextEditor* Editor = (TextEditor*)Parent;
	if (!Editor->Focused)
	{
		return;
	}
}

void TextEditor_OnScrollUpdate(void* Parent,f32 Percentage)
{
	TextEditor* Editor = (TextEditor*)Parent;
	u32 TotalLines = Editor->String->Info.LineCount;
	Editor->FirstLineOffset = Percentage * TotalLines; // 0.5 / 1500; = 750
	Editor->Changed = true;
}
Vector TextEditor::GetCursorPosition(u32 X, u32 Y)
{
	Font* font = GEngine.GetClosestFont(Size);
	s32 ScaledAscent = ceilf(font->ascent * font->GetScale(Size));

	Vector Result = { 0 };
	u32 StartY = Pos.m128_f32[1];
	for (u32 i = 0; i < String->Info.LineCount; i++)
	{
		if (Y > StartY && Y <= String->Info.LineInfo[i].NewLineYPosition)
		{
			Result.m128_f32[1] = String->Info.LineInfo[i].NewLineYPosition - ScaledAscent;
			CursorPosY = i;
			break;
		}
		StartY = String->Info.LineInfo[i].NewLineYPosition;
	}

	//Clicking past the end of the string.
	StringLineInfo* Info = &String->Info.LineInfo[CursorPosY];
	f32 Width;
	CursorPosX = Info->CharCount;
	if (CursorPosX > 0)
	{
		Vector Position = Info->GetPosition(CursorPosX - 1);
		Width = Info->GetWidth(CursorPosX - 1);
		if (X > Position.m128_f32[0] + Width)
		{
			Result.m128_f32[0] = Position.m128_f32[0] + Width;
			return Result;
		}
	}
	//Clicking within the string
	for (u32 i = 0; i < Info->CharCount; i++)
	{
		u32 StartX = Info->GetPosition(i).m128_f32[0];
		u32 EndX = StartX + ceilf(Info->GetWidth(i));
		if (X >= StartX && X <= EndX)
		{
			u32 CenterX = (StartX + EndX) / 2;
			if (X < CenterX)
			{
				CursorPosX = i;
				Result.m128_f32[0] = StartX;
			}
			else
			{
				CursorPosX = i + 1;
				Result.m128_f32[0] = EndX;
			}
			return Result;
		}
	}
	return Result;
}
u32 TextEditor::GetStringIndexFromXY(u32 LineX, u32 LineY)
{
	u32 Result = 0;
	for (u32 i = 0; i < String->Info.LineCount && i < LineY;i++)
	{
		StringLineInfo LineInfo = String->Info.LineInfo[i];
		Result += LineInfo.Contents.elementCount + 1; // +1 for the trailing \n
	}
	return Result + LineX;
}
void TextEditor::Init(ObjectManager* ObjManager,MouseManager* MManager,KeyboardManager* KManager,Vector InPos,Vector InDim, u32 TextSize,const char* DEBUG_fileName,Vector BackgroundColor)
{
	FirstLineOffset = 0;
	Changed = false;
	IsShiftDown = false;
	Pos = InPos;
	Dim = InDim;
	Size = TextSize;
	ObjManager2D = ObjManager;

	Contents.Init(255, 1);
	File file;
	if (file.Load(DEBUG_fileName))
	{
		Contents.Copy(file.Data,file.FileSize);
		file.Close();
	}
	String = ObjManager2D->AddString(Contents.data, Pos, Size,"Font2");
	
	
	ObjectDesc desc = {0};
	desc.Color = { 1,1,1,1 };
	desc.Dim = { (TextSize * 0.10f),(f32)GEngine.GetClosestFont(Size)->GetCursorSize(TextSize) };
	desc.ModelFileName = "TopLeft";
	desc.PipelineName = "Ortho";
	Cursor = ObjManager2D->AddObject(desc);

	CREATE_MOUSE_CALLBACK(this,mCallbacks,TextEditor);
	CREATE_KEYBOARD_CALLBACK(this, kCallbacks, TextEditor);

	Vector ScrollBarDim = { 30,Dim.m128_f32[1] };
	f32 scrollBarX = Pos.m128_f32[0] + Dim.m128_f32[0] - (ScrollBarDim.m128_f32[0] / 2);
	f32 scrollBarY = Pos.m128_f32[1];

	scrollBar.Init(ObjManager, MManager, { scrollBarX, scrollBarY,1 }, ScrollBarDim, { 0,0,0,0 }, BackgroundColor, { 0.5f,0.5f,0.5,1.0f },TextEditor_OnScrollUpdate,this);
	
	MManager->Register(&mCallbacks);
	KManager->Register(&kCallbacks);

}
void TextEditor::Draw() 
{
	static f32 Timer = 0;
	static bool displayCursor = 0;
	if (Focused)
	{
		if (Timer > 0.6)
		{
			Timer = 0;
			displayCursor = !displayCursor;
		}
		Timer += GEngine.GetFrameTime();

		String->Draw();
		if (displayCursor)
		{
			Cursor->Draw();
		}
		Cursor->Draw();
		scrollBar.Draw();
	}
}
Vector TextEditor::UpdateCursor()
{
	Font* font = GEngine.GetClosestFont(Size);
	s32 ScaledAscent = ceilf(font->ascent * font->GetScale(Size));

	Vector result = {0};
	StringLineInfo* Info = &String->Info.LineInfo[CursorPosY];
	if(CursorPosX == 0 && CursorPosY == 0)
	{
		return result;
	}
	if (CursorPosX != 0)
	{
		result.m128_f32[0] = Info->GetPosition(CursorPosX - 1).m128_f32[0] + Info->GetWidth(CursorPosX - 1);
	}
	
	result.m128_f32[1] = Info->NewLineYPosition - ScaledAscent;
	return result;
}
void TextEditor::Update()
{
	if (Changed)
	{
		ObjManager2D->UpdateString(String, Contents.data, Size,FirstLineOffset);
		Changed = false;
	}
	Cursor->Pos = UpdateCursor();
}
void TextEditor::DeleteCharacter(u32 x, u32 y)
{
	Contents.Delete(GetStringIndexFromXY(x, y));
}
void TextEditor::SetReceiveInput(bool Enabled)
{
	kCallbacks.IsEnabled = Enabled;
	mCallbacks.IsEnabled = Enabled;
	scrollBar.MCallbacks.IsEnabled = Enabled;
}