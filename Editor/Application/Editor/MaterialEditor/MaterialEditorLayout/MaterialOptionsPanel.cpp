#include "MaterialOptionsPanel.h"
#include "Application\Engine.h"
void MaterialOptionsPanel_AddNewSelector(void* Parent,Mouse mouse)
{
	MaterialOptionsPanel* Options = (MaterialOptionsPanel*)Parent;
	Options->DefferAdd = true;
	Options->Type = eSelector;

	Options->OnPropertyAdded(eSelector, 0);
}

void MaterialOptionsPanel::Init(ObjectManager* Manager, Vector Pos, Vector Dim, Vector Color)
{
	UpdateNeeded = true;
	DefferAdd = false;
	objectManager = Manager;
	Selectors.Init(64, sizeof(FileSelector));
	Background = Manager->AddObject(Pos, Dim, { 0,0 }, Color, "TopLeft", NULL, "Ortho");
	Pos.m128_f32[2] = 0.1f; 
	TextureSelectorSlots = { 0 };
	ButtonDesc desc = {0};
	desc.Callbacks.OnLButtonUp = MaterialOptionsPanel_AddNewSelector;
	desc.Color = {0,0,0,1};
	desc.Dim = { 32,32 };
	desc.Pos = { ((Pos.m128_f32[0] + Dim.m128_f32[0]) / 2) - (desc.Dim.m128_f32[0] / 2),(Pos.m128_f32[1] + (desc.Dim.m128_f32[1] / 2))};
	desc.Manager = &GEngine.pWindow->mouseManager;
	desc.ObjectManager = Manager;
	desc.Text = "+";
	desc.TextSize = 24;
	desc.Center = true;
	desc.Callbacks.Parent = this;
	NewSelectorButton.Initialize(desc);

	OnPropertyAddedCallbacks.Init(8,sizeof(OnPropertyAddedCallback));
	OnPropertyAddedCallbackParams.Init(8,sizeof(OnPropertyAddedParams));

};
void MaterialOptionsPanel::AddSelector(MouseManager* MManager, Vector Pos, const char* Label, const char* Contents)
{
	FileSelector Selector;
	Selectors.Add(&Selector);
	FileSelector* selectors = &(DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Selectors)[Selectors.elementCount - 1]);
	selectors->Init(MManager, objectManager, Pos, 24, Label, Contents);
	NewSelectorButton.desc.Pos.m128_f32[1] = Pos.m128_f32[1] + 60;
	NewSelectorButton.SetPos(NewSelectorButton.desc.Pos);
}
void MaterialOptionsPanel::Update()
{

	FileSelector* selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Selectors);
	for (u32 i = 0; i < Selectors.elementCount; i++)
	{
		selectors[i].Update();
	}
	/*
	if (DefferAdd)
	{
		switch(Type)
		{
			case eSelector:
			{
				char Buffer[255];
				sprintf_s(Buffer, 255, "NewTexture%d", Selectors.elementCount);

				Vector Pos = { 0 };
				FileSelector* selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Selectors);
				for (u32 i = 0; i < Selectors.elementCount; i++)
				{
					Pos = selectors[i].BasePosition;
				}
				Pos.m128_f32[1] += 60;

				AddSelector(&GEngine.pWindow->mouseManager, Pos, Buffer, "");

				break;
			}
		}
		DefferAdd = false;
		Type = 0;
	}
	*/
};
void MaterialOptionsPanel::Draw()
{
	Background->Draw();

	FileSelector* selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Selectors);
	for (u32 i = 0; i < Selectors.elementCount; i++)
	{
		selectors[i].Draw();
	}
	NewSelectorButton.Draw();
};
void MaterialOptionsPanel::EnableInputs()
{
	FileSelector* selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Selectors);
	for (u32 i = 0; i < Selectors.elementCount; i++)
	{
		selectors[i].button.SetAcceptInput(true);
	}
	NewSelectorButton.mCallbacks.IsEnabled = true;
}
void MaterialOptionsPanel::DisableInputs()
{
	FileSelector* selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Selectors);
	for (u32 i = 0; i < Selectors.elementCount; i++)
	{
		selectors[i].button.SetAcceptInput(false);
	}
	NewSelectorButton.mCallbacks.IsEnabled = false;
}
void MaterialOptionsPanel::AddOnPropertyChangedCallback(void* Parent, PropertyChanged UserCallback)
{
	FileSelector* selectors = DYNAMIC_ARR_GET_CAST_DATA(FileSelector, Selectors);
	for (u32 i = 0; i < Selectors.elementCount; i++)
	{
		selectors[i].AddPropertyChangeCallback(Parent, UserCallback);
	}
}
void MaterialOptionsPanel::AddOnPropertyAddedCallback(void* Parent,OnPropertyAddedCallback Callback)
{
	OnPropertyAddedParams Params;
	Params.Parent = Parent;
	OnPropertyAddedCallbacks.Add(&Callback);
	OnPropertyAddedCallbackParams.Add(&Params);
}
void MaterialOptionsPanel::OnPropertyAdded(u32 NewPropertyType, u32 NewPropertySubType)
{
	OnPropertyAddedCallback* Callbacks = DYNAMIC_ARR_GET_CAST_DATA(OnPropertyAddedCallback, OnPropertyAddedCallbacks);
	OnPropertyAddedParams* Params = DYNAMIC_ARR_GET_CAST_DATA(OnPropertyAddedParams, OnPropertyAddedCallbackParams);

	if (NewPropertyType == eSelector)
	{
		for (u32 i = 0; i < OnPropertyAddedCallbacks.elementCount; i++)
		{
			Params[i].PropertyType = NewPropertyType;
			Params[i].NumberOfProperties = Selectors.elementCount;
			Callbacks[i](Params[i]);
		}
	}
			
}