#pragma once
#include "System/MouseManager.h"
#include "System/Renderer/ObjectManager2D.h"
#include "Application\UI\Button.h"
#include "Platform/Windows/Utils/WinSaveInterface.h"

struct PropertyChangeInfo
{
	u32 PropertyType;
	u32 Size;
	void* NewData;
	void* PropertyObject;
	void* Parent;
};

typedef void(*PropertyChanged)(PropertyChangeInfo);

struct ParentCallbackPair
{
	void* Parent;
	PropertyChanged Callback;
};

struct FileSelector
{
	Object2D* Background;
	String2D* Label;
	String2D* FileString;
	UIButton button;

	DialogInfo ContentFileInfo;

	//Char
	DynamicArray LabelString;
	//Char
	DynamicArray Contents;
	
	char FilePath[MAX_PATH];

	u32 textSize;
	ObjectManager2D* ObjectManager;
	Vector BasePosition;

	//ParentCallbackPair
	DynamicArray Callbacks;

	bool LabelModified;
	bool ContentsModified;

	void Init(MouseManager* MManager, ObjectManager2D* Manager, Vector Pos, u32 TextSize, const char* LabelName, const char* FileName);
	void Update();
	void Draw();
	void AddPropertyChangeCallback(void* Parent,PropertyChanged UserCallback);
	void UpdateLabel(char* NewName,u32 Length);
	void UpdateContent(char* NewFileName,u32 Length);
};