#pragma once
#include "System/Renderer/Object2D.h"
#include "Application/UI/TextBox.h"
#include "Application\UI\FileSelector.h"
#include "../MaterialEditorCallbackDefines.h"
#include "Application\UI\Button.h"

struct MaterialOptionsPanel
{
	Object2D* Background;

	UIButton NewSelectorButton;

	//Textures and which one to slot to bind to	
	//FileSelector
	DynamicArray Selectors;
	
	ObjectManager* objectManager;

	//OnRegisterChanged
	DynamicArray OnPropertyAddedCallbacks;

	//OnRegisterChangedParams
	DynamicArray OnPropertyAddedCallbackParams;

	bool UpdateNeeded;
	bool DefferAdd;
	u32 Type;
	void Init(ObjectManager* Manager, Vector Pos, Vector Dim, Vector Color);
	void AddSelector(MouseManager* MManager, Vector Pos, const char* Label, const char* Contents);
	void Update();
	void Draw();
	void EnableInputs();
	void DisableInputs();
	void AddOnPropertyChangedCallback(void* Parent, PropertyChanged UserCallback);
	void AddOnPropertyAddedCallback(void* Parent,OnPropertyAddedCallback Callback);
	void OnPropertyAdded(u32 NewPropertyType, u32 NewPropertySubType);
};