#pragma once
#include "System\Memory\DynamicArray.h"
#include "System\Vector.h"
struct Object2D;
struct ObjectManager2D;
struct TabPanel
{
	void* Parent;
	
	ObjectManager2D* Manager2D;
	//UIButton
	DynamicArray Tabs;
	
	u32 CurrentTab;
	f32 TabOffset;

	Object2D* Background;

	void Init(void* Parent, ObjectManager2D* InManager, Vector PanelDim, Vector PanelPos,Vector Color);

	void Draw();
	void Update();
	void AddTab(f32 TabWidth,  const char* Name);
	void CloseTab(u32 Index);

};