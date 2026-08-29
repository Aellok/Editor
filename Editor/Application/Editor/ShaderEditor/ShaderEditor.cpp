#include "ShaderEditor.h"
#include "Application\Engine.h"
#include "System\Renderer\ObjectManager.h"
#include "ShaderCompiler\Parser\Parser.h"
#include "ShaderCompiler\Lexer\Lexer.h"
#include "ShaderCompiler\Builders\InputLayoutBuilder.h"
#include "ShaderCompiler\Builders\RootSignatureBuilder.h"
#include "ShaderCompiler\ShaderCompiler.h"
#include "Platform\Windows\Utils\WinSaveInterface.h"


const char* ShaderNames[SPBCount]
{
	"Vertex_Shader",
	"Hull_Shader",
	"Domain_Shader",
	"Geometry_Shader"
	"Pixel_Shader"
};
const char* ShaderCompileVersions[SPBCount]
{
	"vs_5_0",
	"hs_5_0",
	"ds_5_0",
	"gs_5_0",
	"ps_5_0"
};


void ShaderEditorInterface_OnKeyUp(void* Parent,u32 Key)
{
	ShaderEditor* SEI = (ShaderEditor*)Parent;
	switch(Key)
	{
		case VK_F1:
		{
			DX12PipelineDesc2 desc = SEI->GetPipelineDesc();
			if (!desc.IsValid())
			{
				printf("Error: Failed to compile shaders into pipeline.\n");
				return;
			}
			u32 TotalSize = 0;
			void* data = desc.Serialize(&TotalSize);
			if (!data)
			{
				return;
			}
			WinDialog_Save(data, TotalSize);
			break;
		}
	}
}
void ShaderEditorInterface_DragDrop(void* Parent, Mouse mouse, char* FileName)
{
	ShaderEditor* SEI = (ShaderEditor*)Parent;

	File file;
	file.Load(FileName);
	TextEditor* CurrentEditor = &SEI->Editors[SEI->Panel.CurrentEditor];
	CurrentEditor->CursorPosX = 0;
	CurrentEditor->CursorPosY = 0;
	CurrentEditor->Contents.Copy(file.Data, file.FileSize);
	CurrentEditor->Changed = true;
	file.Close();
	SEI->UpdatePSRegisters(true);
}
void ShaderEditor::Init(MouseManager* mManager, KeyboardManager* kManager,ObjectManager* SceneObjManager,f32 PanelWidth,Vector Pos,Vector ViewportDim,Vector Color)
{
	ObjectManager* Manager = GEngine.pObjManager2D;

	RegisterCount = 0;
	RegisterNames.Init(8, sizeof(DataString));
	RegisterSlots.Init(8, sizeof(u32));
	PSRegistersChanged.Init(4, sizeof(OnRegisterChanged));
	PSRegistersChangedParams.Init(4, sizeof(OnRegisterChangedParams));
	ParseTimer = 0;


	f32 ShaderPanelWidth = PanelWidth;
	f32 ButtonStartY = Pos.m128_f32[1];
	f32 ButtonHeight = 100;
	f32 ButtonPadding = 10;

	//desc.Pos = { 0, ButtonStartY };
	//desc.Dim = ;
	Panel.Init(&GEngine.pWindow->mouseManager, GEngine.pObjManager2D, { 0, ButtonStartY }, { ShaderPanelWidth,ButtonHeight },Color);

	

	const char* DEBUG_Arr[SPBCount]
	{
		"",
		"",
		"",
		"",
		""
	};
	
	Editors = (TextEditor*)GEngine.Global.Allocate(sizeof(TextEditor) * SPBCount);
	for (u32 i = 0; i < SPBCount;i++)
	{
		Editors[i].Init(SceneObjManager,mManager,kManager, { 0,0 }, ViewportDim, 24, DEBUG_Arr[i],Color);
		
	}
	
	Editors[Panel.CurrentEditor].Focused = true;
	

	memset(&mcb, 0, sizeof(MouseCallbacks));
	memset(&kcb, 0, sizeof(KeyboardCallbacks));
	kcb.Parent = this;
	kcb.OnKeyUp = ShaderEditorInterface_OnKeyUp;
	kManager->Register(&kcb);

	mcb.OnDragDrop = ShaderEditorInterface_DragDrop;
	mcb.Parent = this;
	mcb.IsEnabled = true;
	mManager->Register(&mcb);

	

}

void ShaderEditor::Draw()
{
	if (Panel.CurrentEditor != SPBCount)
	{
		Editors[Panel.CurrentEditor].Draw();
	}
	Panel.Draw();
}
#define ABVS
struct ColorCode
{
	u32 Start;
	u32 End;
	DirectX::XMFLOAT4 Color;
};
ColorCode* GetColorCodePositions(char* Buffer,u32* CodeCount)
{
	ColorCode* Codes = (ColorCode*)calloc(128, sizeof(ColorCode));
	u32 BufferLength = strlen(Buffer);

	for (u32 i = 0; i < eDefineCount; i++)
	{
		char* ptr = Buffer;
		while (ptr - Buffer < BufferLength )
		{
			ptr = strstr(ptr, DefineStrings[i]);
			if (ptr == NULL)
			{
				break;
			}
			Codes[*CodeCount].Color = { 190.0f / 255.0f,183.0f / 255.0f,1,0 };
			Codes[*CodeCount].Start = ptr - Buffer;
			Codes[*CodeCount].End = ((ptr - Buffer) + strlen(DefineStrings[i]));
			*CodeCount += 1;
			ptr++;
		}
	}
	for (u32 i = 0; i < eTypeCount; i++)
	{
		char* ptr = Buffer;
		while (ptr - Buffer < BufferLength)
		{
			ptr = strstr(ptr, TypeStrings[i]);
			if (ptr == NULL || ((ptr != Buffer) && *(ptr - 1) != ' ') || *(ptr + strlen(TypeStrings[i])) != ' ')
			{
				break;
			}
			
			Codes[*CodeCount].Color = { 86.0f / 255.0f,156.0f / 255.0f,214.0f/255.0f,0 };
			Codes[*CodeCount].Start = ptr - Buffer;
			Codes[*CodeCount].End = ((ptr - Buffer) + strlen(TypeStrings[i]));
			*CodeCount += 1;
			ptr++;
		}
	}
	return Codes;
}
void ShaderEditor::Update(bool Enabled)
{
	bool UpdatePS = Editors[ePixelShader].Changed;
	Panel.Update();
	mcb.IsEnabled = Enabled;
	kcb.IsEnabled = Enabled;
	for (u32 i = 0; i < SPBCount; i++)
	{
		Editors[i].SetReceiveInput(Enabled);
		Editors[i].Focused = Panel.CurrentEditor == i;
	}

	Editors[Panel.CurrentEditor].Focused = Enabled;
	if (Enabled && Panel.CurrentEditor != SPBCount)
	{
		Editors[Panel.CurrentEditor].Update();
		DX12String2D* str = &Editors[Panel.CurrentEditor].ObjManager2D->StringMeshs[Editors[Panel.CurrentEditor].String->Index];
		Font2InstanceBuffer2D* Instances = DYNAMIC_ARR_GET_CAST_DATA(Font2InstanceBuffer2D, str->InstanceBufferData);


		for (u32 i = 0; i < str->InstanceBufferData.elementCount; i++)
		{
			Instances[i].Color = { 1,1,1,0 };
		}
		u32 CharOffset = 0;
		for (u32 LineIndex = 0; LineIndex < Editors[Panel.CurrentEditor].String->Info.LineCount; LineIndex++)
		{
			u32 CodeCount = 0;
			StringLineInfo* Info = &Editors[Panel.CurrentEditor].String->Info.LineInfo[LineIndex];
			ColorCode* Codes = GetColorCodePositions(Info->Contents.data, &CodeCount);

			for (u32 i = 0; i < CodeCount; i++)
			{
				for (u32 x = Codes[i].Start; x < Codes[i].End; x++)
				{
					Instances[CharOffset + x].Color = Codes[i].Color;
				}
			}
			free(Codes);
			CharOffset += Info->CharCount;
		}
	}
	//do this once a second.
	if (UpdatePS && ParseTimer > 1)
	{
		UpdatePSRegisters(false);
		ParseTimer = 0;
	}
	ParseTimer += GEngine.GetFrameTime();
}

DX12PipelineDesc2 ShaderEditor::GetPipelineDesc()
{
	VSParserResult ParsedVSShader = {0};
	PSParserResult ParsedPSShader = {0};
	InputLayout Layout = {0};
	RootSignature rs = {0};
	if (Editors[eVertexShader].Contents.elementCount > 0)
	{
		LexerResult VSLexerTokens = ParseHLSL(Editors[eVertexShader].String->Info);
		if (VSLexerTokens.tokens.elementCount == 0)
		{
			printf("Error: ParseHLSL failed to find any tokens.\n");
			return { 0 };
		}
		ParsedVSShader = ParseVSShader(VSLexerTokens);
		VSLexerTokens.tokens.Free();
	}
	if (Editors[ePixelShader].Contents.elementCount > 0)
	{
		LexerResult PSLexerTokens = ParseHLSL(Editors[ePixelShader].String->Info);
		if (PSLexerTokens.tokens.elementCount == 0)
		{
			printf("Error: ParseHLSL failed to find any tokens.\n");
			return { 0 };
		}
		ParsedPSShader = ParsePSShader(PSLexerTokens);
		PSLexerTokens.tokens.Free();
	}
	
	if (ParsedVSShader.IsValid)
	{
		Layout = BuildInputLayout(ParsedVSShader.vsInput);
		if (ParsedPSShader.IsValid)
		{
			rs = BuildRootSignature(ParsedVSShader, ParsedPSShader);
		}
		else
		{
			printf("Warning: PS Shader parser never ran or there was an error.  wont be built.\n");
			return { 0 };
		}
	}
	else
	{
		printf("Error: VS Shader parser never ran or there was an error. Layout and root signature wont be built.\n");
		return { 0 };
	}
	

	CompiledShaderDesc CompiledShaders[SPBCount];

	for (u32 i = 0; i < SPBCount; i++)
	{
		CompiledShaders[i] = CompileShader(ShaderNames[i], "main", ShaderCompileVersions[i], Editors[i].Contents);
		if (CompiledShaders[i].Blob == NULL && strlen(Editors[i].Contents.data) != 0)
		{
			printf("Error: %s failed to compile.\n", ShaderNames[i]);
			return {};
		}
	}
	//Create the pipeline;
	DX12PipelineDesc2 desc = { 0 };
	desc.header.CBVCount = ParsedVSShader.cbvs.Count;
	for (u32 i = 0; i < ParsedVSShader.cbvs.Count; i++)
	{
		desc.header.CBVSizes[i] = ParsedVSShader.cbvs.Size[i];
	}

	desc.header.InputCount = Layout.Inputs.elementCount;
	desc.InputLayout = DYNAMIC_ARR_GET_CAST_DATA(D3D12_INPUT_ELEMENT_DESC, Layout.Inputs);
	desc.header.InputSizes[0] = Layout.BufferSizes[0];
	desc.header.InputSizes[1] = Layout.BufferSizes[1];

	desc.rootSigBlob = rs.CompiledBlob->GetBufferPointer();
	desc.header.RootSigSize = rs.CompiledBlob->GetBufferSize();
	memcpy(desc.header.RootSigParamTypes, DYNAMIC_ARR_GET_CAST_DATA(u32, rs.RootParamTypes), rs.RootParamTypes.elementCount);
	desc.header.RootSigParamCount = rs.RootParams.elementCount;

	desc.CompiledVS = CompiledShaders[eVertexShader].Blob;
	desc.header.CompiledVSSize = CompiledShaders[eVertexShader].Size;
	desc.CompiledPS = CompiledShaders[ePixelShader].Blob;
	desc.header.CompiledPSSize = CompiledShaders[ePixelShader].Size;

	
	return desc;
}

DX12Pipeline* ShaderEditor::CreatePipeline()
{
	DX12Pipeline* res;
	res = (DX12Pipeline*)calloc(1,sizeof(DX12Pipeline));

	DX12PipelineDesc2 desc = GetPipelineDesc();
	if (!desc.IsValid())
	{
		return nullptr;
	}
	//TODO: MSAA and Depth shouldnt be hard coded.
	res->Create(GEngine.pRendererInterface->device,desc,true,false);
	return res;
}
void ShaderEditor::UpdatePSRegisters(bool reset)
{
	RegisterNames.Clear();
	RegisterSlots.Clear();
	RegisterCount = 0;
	PSParserResult ParsedPSShader = { 0 };
	
	Editors[ePixelShader].Update();

	if (Editors[ePixelShader].Contents.elementCount > 0)
	{
		LexerResult PSLexerTokens = ParseHLSL(Editors[ePixelShader].String->Info);
		if (PSLexerTokens.tokens.elementCount == 0)
		{
			printf("Error: ParseHLSL failed to find any tokens.\n");
			return;
		}
		ParsedPSShader = ParsePSShader(PSLexerTokens);
		PSLexerTokens.tokens.Free();
	}
	RegisterCount = ParsedPSShader.registers.Slots.elementCount;
	
	DataString* Names = DYNAMIC_ARR_GET_CAST_DATA(DataString, ParsedPSShader.registers.VarNames);
	u32* Types = DYNAMIC_ARR_GET_CAST_DATA(u32, ParsedPSShader.registers.Types);
	u32* Slots = DYNAMIC_ARR_GET_CAST_DATA(u32, ParsedPSShader.registers.Slots);
	for (u32 i = 0; i < RegisterCount; i++)
	{
		if (Types[i] == eTEXTURE)
		{
			RegisterNames.Add(&Names[i]);
			RegisterSlots.Add(&Slots[i]);
			printf("Texture Slot found: %s\n", Names[i]);
		}	
	}
	//this is being called on every key press if there is already a register. need keep track of the # of registers on the last call.
	OnRegisterChanged* Callbacks = DYNAMIC_ARR_GET_CAST_DATA(OnRegisterChanged, PSRegistersChanged);
	OnRegisterChangedParams* Params = DYNAMIC_ARR_GET_CAST_DATA(OnRegisterChangedParams, PSRegistersChangedParams);
	for (u32 i = 0; i < PSRegistersChanged.elementCount;i++)
	{
		Params[i].RegisterCount = RegisterNames.elementCount;
		Params[i].RegisterNames = RegisterNames;
		Params[i].Slots = RegisterSlots;
		Params[i].reset = reset;
		Callbacks[i](Params[i]);		
	}
}
void ShaderEditor::RegisterOnPSRegisterChanged(OnRegisterChanged Callbacks, OnRegisterChangedParams Params)
{
	PSRegistersChangedParams.Add(&Params);
	PSRegistersChanged.Add(&Callbacks);
}

void ShaderEditor_OnPropertyChanged(OnPropertyAddedParams Param)
{
	ShaderEditor* shaderEditor = (ShaderEditor*)Param.Parent;
	if (Param.PropertyType == eSelector)
	{
		TextEditor* Editor = &shaderEditor->Editors[ePixelShader];
		Editor->Changed = true;
		char Buffer[255] = {0};
		sprintf_s(Buffer, 255, "TEXTURE Texture2D Tex%d : register(t%d);\n", Param.NumberOfProperties, Param.NumberOfProperties);
		Editor->Contents.InsertArray(0,Buffer,strlen(Buffer));
	}
	shaderEditor->UpdatePSRegisters(false);
}

void ShaderEditor_OnObjectChanged(ObjectChangeInfo Info)
{
	ShaderEditor* shaderEditor = (ShaderEditor*)Info.Parent;
	for (u32 i = 0; i < 5; i++)
	{
		shaderEditor->Editors[i].Contents.Copy((void*)Info.ShaderFileData[i], Info.ShaderSize[i]);
		shaderEditor->Editors[i].Changed = true;
	}
	shaderEditor->UpdatePSRegisters(true);
}