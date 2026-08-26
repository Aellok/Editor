#include "Application\UI\FileSelector.h"
#include "Platform\Windows\Utils\WinSaveInterface.h"

void FileSelector_LButtonDown(void* Parent,Mouse mouse)
{
	FileSelector* fs = (FileSelector*)Parent;

	u32 FileSize = 0;
	fs->ContentFileInfo = WinDialog_Load();
	if (fs->ContentFileInfo.FilePath && strlen(fs->ContentFileInfo.FilePath))
	{
		u32 ExtIndex = GetLastCharIndex(fs->ContentFileInfo.FilePath, '.');
		u32 SlashIndex = GetLastCharIndex(fs->ContentFileInfo.FilePath, '/');
		u32 BackSlashIndex = GetLastCharIndex(fs->ContentFileInfo.FilePath, '\\');
		u32 Start = (SlashIndex == 0 ? BackSlashIndex : SlashIndex) + 1;

		fs->UpdateContent(&fs->ContentFileInfo.FilePath[Start], ExtIndex - Start);

		for (u32 i = 0; i < fs->Callbacks.elementCount; i++)
		{
			ParentCallbackPair* CallbackList = DYNAMIC_ARR_GET_CAST_DATA(ParentCallbackPair, fs->Callbacks);
			PropertyChangeInfo Info;
			Info.PropertyType = 0;
			Info.Size = fs->ContentFileInfo.Size; // char has size 1
			Info.Parent = CallbackList[i].Parent;
			Info.PropertyObject = fs;
			Info.NewData = fs->ContentFileInfo.FilePath;
			CallbackList[i].Callback(Info);
		}
	}
}

void FileSelector::Init(MouseManager* MManager,ObjectManager* Manager, Vector Pos, u32 TextSize, const char* LabelName,const char* FileName)
{
	textSize = TextSize;
	objectManager = Manager;
	BasePosition = Pos;
	Callbacks.Init(8, sizeof(ParentCallbackPair));
	Label = objectManager->AddString(LabelName, Pos, TextSize, "Font");
	
	Pos.m128_f32[0] += Label->PixelLength + 10;

	FileString = objectManager->AddString(FileName, Pos, TextSize, "Font");
	Contents.Init(32, sizeof(char*));
	UpdateContent((char*)FileName, strlen(FileName));

	LabelString.Init(8, sizeof(char*));
	UpdateLabel((char*)LabelName, strlen(LabelName));
	
	Pos.m128_f32[0] += FileString->PixelLength + 10;

	ButtonDesc desc = {0};
	desc.Text = "...";
	desc.Color = {0.25,0.25 ,0.25 ,1};
	desc.Pos = Pos;
	desc.ShrinkToText = true;
	desc.ObjectManager = Manager;
	desc.Manager = MManager;
	desc.Callbacks.Parent = this;
	desc.Callbacks.OnLButtonDown = FileSelector_LButtonDown;
	desc.TextSize = 24;
	
	button.Initialize(desc);
}
void FileSelector::UpdateLabel(char* NewName,u32 Length)
{
	LabelModified = true;
	LabelString.Copy((void*)NewName, strlen(NewName));
}
void FileSelector::UpdateContent(const char* NewFileName,u32 Length)
{
	ContentsModified = true;
	Contents.Copy((void*)NewFileName, Length);
}
void FileSelector::Update()
{
	if (LabelModified)
	{
		objectManager->UpdateString(Label, DYNAMIC_ARR_GET_CAST_DATA(char, LabelString), textSize);
		
		LabelModified = false;
	}
	if (ContentsModified)
	{
		FileString->Pos.m128_f32[0] = Label->PixelLength + 10;
		objectManager->UpdateString(FileString, DYNAMIC_ARR_GET_CAST_DATA(char, Contents), textSize);
		
		ContentsModified = false;
	}
	Vector Pos = BasePosition;
	Pos.m128_f32[0] += Label->PixelLength + 10;
	Pos.m128_f32[0] += FileString->PixelLength + 10;
	button.SetPos(Pos);
}
void FileSelector::Draw()
{
	Label->Draw();
	FileString->Draw();
	button.Draw();
}
void FileSelector::AddPropertyChangeCallback(void* Parent,PropertyChanged UserCallback)
{
	ParentCallbackPair Pair;
	Pair.Parent = Parent;
	Pair.Callback = UserCallback;
	Callbacks.Add(&Pair);
}