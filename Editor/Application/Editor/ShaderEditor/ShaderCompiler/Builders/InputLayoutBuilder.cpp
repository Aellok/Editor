#include "InputLayoutBuilder.h"
#include "..\Lexer\LexerDataTypes.h"
#include "Platform\Windows\Renderer\DirectX12\d3dx12.h"

DXGI_FORMAT GetInputFormat(u32 Type)
{
	switch (Type)
	{
		case eFLOAT4X4:
		{
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		case eFLOAT4:
		{
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		case eFLOAT3:
		{
			return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		case eFLOAT2:
		{
			return DXGI_FORMAT_R32G32_FLOAT;
		}
		case eFLOAT:
		{
			return  DXGI_FORMAT_R32_FLOAT;
		}
		default:
		{
			return DXGI_FORMAT_UNKNOWN;
		}
	}
}
u32 GetInputSizes(u32 Type)
{
	switch (Type)
	{		
		case eFLOAT4X4:
		{
			return 64;
		}
		case eFLOAT4:
		{
			return 16;
		}
		case eFLOAT3:
		{
			return 12;
		}
		case eFLOAT2:
		{
			return 8;
		}
		case eFLOAT:
		{
			return 4;
		}
		default:
		{
			return 0xffffffff;
		}
	}
}
D3D12_INPUT_ELEMENT_DESC CreateInput(DynamicArray& SemanticNames,u32 TypeIndex,DataString SemanticName,u32 SemanticFlags)
{
	D3D12_INPUT_ELEMENT_DESC res;
	D3D12_INPUT_ELEMENT_DESC* input = &res;
	input->InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	input->AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	input->InstanceDataStepRate = 0;
	input->InputSlot = 0;
	input->Format = GetInputFormat(TypeIndex);
	if (strlen(SemanticName.Buffer) >= 32)
	{
		printf("ERROR: Semantic Name is bigger than 32\n");
	}
	SemanticNames.Add(&SemanticName);

	DataString* SemanticPtr = DYNAMIC_ARR_GET_CAST_DATA(DataString, SemanticNames);
	input->SemanticName = SemanticPtr[SemanticNames.elementCount - 1].Buffer;
	input->SemanticIndex = SemanticFlags & ((1u << 31) - 1u);

	if (SemanticFlags & (1 << 31))
	{
		input->InstanceDataStepRate = 1;
		input->InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		input->InputSlot = 1; // TODO: user should choose which slot the instance data is in.
	}
	return res;
}
void DEBUG_PrintInputLayout(InputLayout inputLayout)
{
	D3D12_INPUT_ELEMENT_DESC* input = DYNAMIC_ARR_GET_CAST_DATA(D3D12_INPUT_ELEMENT_DESC, inputLayout.Inputs);
	for (u32 i = 0; i < inputLayout.Inputs.elementCount; i++)
	{
		const char* Format = "";
		switch (input[i].Format)
		{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		{
			Format = "DXGI_FORMAT_R32G32B32A32_FLOAT";
			break;
		}
		case DXGI_FORMAT_R32G32B32_FLOAT:
		{
			Format = "DXGI_FORMAT_R32G32B32_FLOAT";
			break;
		}
		case DXGI_FORMAT_R32G32_FLOAT:
		{
			Format = "DXGI_FORMAT_R32G32_FLOAT";
			break;
		}
		case DXGI_FORMAT_R32_FLOAT:
		{
			Format = "DXGI_FORMAT_R32_FLOAT";
			break;
		}
		}

		printf("{%s,%u,%s,%u,%s,%s,%u}\n", input[i].SemanticName, input[i].SemanticIndex, Format, input[i].InputSlot, "D3D12_APPEND_ALIGNED_ELEMENT",
			input[i].InputSlotClass == 0 ? "D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA" : "D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA", input[i].InstanceDataStepRate);
	}
}
InputLayout BuildInputLayout(ParsedVSInput VSInput)
{
	InputLayout res;
	InputLayout* inputLayout = &res;
	
	inputLayout->Inputs.Init(VSInput.DataTypesIndex.elementCount * 4, sizeof(D3D12_INPUT_ELEMENT_DESC));
	inputLayout->SemanticNames.Init(VSInput.DataTypesIndex.elementCount * 4, sizeof(DataString));

	DataString* Type = DYNAMIC_ARR_GET_CAST_DATA(DataString, VSInput.DataTypesString);
	DataString* Name = DYNAMIC_ARR_GET_CAST_DATA(DataString, VSInput.DataVariableNames);
	DataString* Semantic = DYNAMIC_ARR_GET_CAST_DATA(DataString, VSInput.DataSematicsString);
	u32* SemanticFlags = DYNAMIC_ARR_GET_CAST_DATA(u32, VSInput.SemanticFlags);
	u32* TypeIndex = DYNAMIC_ARR_GET_CAST_DATA(u32, VSInput.DataTypesIndex);

	inputLayout->BufferSizes[0] = 0;
	inputLayout->BufferSizes[1] = 0;


	for (u32 i = 0; i < VSInput.DataTypesIndex.elementCount;i++)
	{		
		bool IsInst = SemanticFlags[i] & (1 << 31);
		switch(TypeIndex[i])
		{
			case eFLOAT4X4:
			{
				D3D12_INPUT_ELEMENT_DESC input = CreateInput(inputLayout->SemanticNames,eFLOAT4, Semantic[i], SemanticFlags[i]);
				for (u32 j = 0; j < 4; j++)
				{
					inputLayout->BufferSizes[IsInst] += GetInputSizes(eFLOAT4);
					inputLayout->Inputs.Add(&input);
					//inputLayout->SemanticNames.Add(&Semantic[i]);
					input.SemanticIndex++;
				}
				break;
			}
			case eFLOAT4:
			case eFLOAT3:
			case eFLOAT2:
			case eFLOAT:
			{
				D3D12_INPUT_ELEMENT_DESC input = CreateInput(inputLayout->SemanticNames,TypeIndex[i], Semantic[i], SemanticFlags[i]);;
				inputLayout->Inputs.Add(&input);
				inputLayout->BufferSizes[IsInst] += GetInputSizes(TypeIndex[i]);
				//inputLayout->SemanticNames.Add(&Semantic[i]);
				break;
			}
		}

	}
	DEBUG_PrintInputLayout(res);

	return res;
}