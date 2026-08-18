#include "Parser.h"
#include <stdio.h>
#include <string>
//ParsedCBV
u32 GetSlotFromString(char* Word)
{
	char* rptr = Word;
	char wbuf[32] = {0};
	u32 Index = 0;
	for (u32 i = 0; i < strlen(Word);i++)
	{
		if (rptr[i] != ' ')
		{
			wbuf[Index++] = rptr[i];
		}
	}
	//register(b0)
	char* startPtr = strchr(wbuf, '(') + 1;
	char* comma = strchr(startPtr, ',');
	char* closeBracketPtr = strchr(startPtr, ')');
	
	if (!closeBracketPtr)
	{
		printf("Error: Malformed line when parsing slot.\n");
		return -1;
	}
	if (wbuf - startPtr == strlen(wbuf))
	{
		printf("Error: Syntax error: there is expected register(t#) but was given: %s.\n", Word);
		return -1;
	}
	//startPtr -> endPtr = b0
	if (startPtr[0] == 'b' || startPtr[0] == 't' || startPtr[0] == 's')
	{
		startPtr++;
		char Buffer[32] = {0};
		if (comma)
		{
			memcpy(Buffer, startPtr, comma - startPtr);
		}
		else
		{
			memcpy(Buffer, startPtr, closeBracketPtr - startPtr);
		}
		return atoi(Buffer);
	}
	else
	{
		printf("Format issue");
	}

	return -1;
}
ParsedCBV ParseCBV(LexerResult& Input)
{
	ParsedCBV res = {0};
	ParsedCBV* cbv = &res;
	u32 StartIndex[8] = {0};
	u32 EndIndex[8] = { 0 };
	bool foundCBV = false;
	for (u32 i = 0; i < Input.tokens.elementCount; i++)
	{
		if (cbv->Count == 8)
		{
			printf("Error: to many cbv's\n");
		}
		Token& token = ((Token*)Input.tokens.data)[i];
		if (token.Type == eDefine && token.Subtype == eCBV)
		{
			StartIndex[cbv->Count] = i;

			cbv->DataVariableNames[cbv->Count].Init(4, 32);
			cbv->DataTypesString[cbv->Count].Init(4, 32);
			cbv->DataTypesIndex[cbv->Count].Init(4,sizeof(u32));

			foundCBV = true;
			continue;
		}
		if (foundCBV && token.Type == eBeginEnd && token.Subtype == 1)
		{
			EndIndex[cbv->Count] = i;
			cbv->Count++;
			foundCBV = false;
		}
	}
	for (u32 cbvIndex = 0; cbvIndex < cbv->Count; cbvIndex++)
	{
		for (u32 i = StartIndex[cbvIndex]; i < EndIndex[cbvIndex]; i++)
		{
			Token& t = ((Token*)Input.tokens.data)[i];
			switch (t.Type)
			{
				case eName:
				{
					cbv->DataVariableNames[cbvIndex].Add(&t.Buffer);
					break;
				}
				case eType:
				{
					cbv->DataTypesString[cbvIndex].Add(&t.Buffer);
					cbv->DataTypesIndex[cbvIndex].Add(&t.Subtype);
					cbv->Size[cbvIndex] += TypeSizes[t.Subtype];
					//printf("%s\n", ((DataString*)cbv->DataTypes[cbvIndex].data)[cbv->DataTypes[cbvIndex].elementCount - 1]);
					break;
				}
				case eSlot:
				{
					cbv->Slot[cbvIndex] = GetSlotFromString(t.Buffer);
					break;
				}
			}
		}
	}

	/*
	for (u32 c = 0; c < cbv->Count; c++)
	{
		for (u32 i = StartIndex[c]; i < EndIndex[c]; i++)
		{
			Input.tokens.Delete(StartIndex[c]);
		}
	}
	*/
	
	return res;
}
//ParsedVSInput
ParsedVSInput ParseVSInput(LexerResult& Input)
{
	ParsedVSInput res;
	ParsedVSInput* VSInput = &res;

	res.DataSematicsString.Init(8, sizeof(DataString));
	res.DataVariableNames.Init(8, sizeof(DataString)); 
	res.DataTypesString.Init(8, sizeof(DataString));
	res.DataTypesIndex.Init(8, sizeof(u32));
	res.SemanticFlags.Init(8, sizeof(u32));
	
	Token* t = DYNAMIC_ARR_GET_CAST_DATA(Token, Input.tokens);
	while (!(t->Type == eDefine && t->Subtype == eVS_INPUT))
	{
		t++;
	}

	while (!(t->Type == eBeginEnd && t->Subtype == 1))
	{
		switch (t->Type)
		{
			case eType:
			{
				VSInput->DataTypesString.Add(&t->Buffer);
				VSInput->DataTypesIndex.Add(&t->Subtype);
				break;
			}
			case eName:
			{
				VSInput->DataVariableNames.Add(&t->Buffer);
				break;
			}
			case eSemantic:
			{
				VSInput->DataSematicsString.Add(&t->Buffer);
				VSInput->SemanticFlags.Add(&t->Subtype);
				break;
			}
		}
		t++;
	}
	return res;
}
//ParsedVSOutput
ParsedVSOutput ParseVSOutput(LexerResult& Input)
{
	ParsedVSOutput res;
	ParsedVSOutput* VSOutput = &res;

	res.DataSematicsString.Init(8, sizeof(DataString));
	res.DataVariableNames.Init(8, sizeof(DataString));
	res.DataTypesString.Init(8, sizeof(DataString));
	res.DataTypesIndex.Init(8, sizeof(u32));
	res.SemanticFlags.Init(8, sizeof(u32));

	Token* t = DYNAMIC_ARR_GET_CAST_DATA(Token, Input.tokens);
	while (!(t->Type == eDefine && t->Subtype == eVS_OUTPUT))
	{
		t++;
	}

	while (!(t->Type == eBeginEnd && t->Subtype == 1))
	{
		switch (t->Type)
		{
		case eType:
		{
			VSOutput->DataTypesString.Add(&t->Buffer);
			VSOutput->DataTypesIndex.Add(&t->Subtype);
			break;
		}
		case eName:
		{
			VSOutput->DataVariableNames.Add(&t->Buffer);
			break;
		}
		case eSemantic:
		{
			VSOutput->DataSematicsString.Add(&t->Buffer);
			VSOutput->SemanticFlags.Add(&t->Subtype);
			break;
		}
		}
		t++;
	}
	return res;
}
void DEBUG_PrintCBV(ParsedCBV& cbvs)
{
	for (u32 i = 0; i < cbvs.Count; i++)
	{
		printf("Slot%d:\n", cbvs.Slot[i]);
		for (u32 j = 0; j < cbvs.DataVariableNames[i].elementCount; j++)
		{
			printf("	%s %s\n", ((DataString*)cbvs.DataTypesString[i].data)[j].Buffer, ((DataString*)cbvs.DataVariableNames[i].data)[j].Buffer);
		}
	}
}
void DEBUG_PrintVSInput(ParsedVSInput& input)
{
	printf("\nVS_INPUT\n");
	for (u32 i = 0; i < input.DataVariableNames.elementCount; i++)
	{
		printf("%s %s : %s\n", ((DataString*)input.DataTypesString.data)[i].Buffer, ((DataString*)input.DataVariableNames.data)[i].Buffer, ((DataString*)input.DataSematicsString.data)[i].Buffer);
	}
}
void DEBUG_PrintVSOutput(ParsedVSOutput& input)
{
	printf("\nVS_OUTPUT\n");
	for (u32 i = 0; i < input.DataVariableNames.elementCount; i++)
	{
		printf("%s %s : %s\n", ((DataString*)input.DataTypesString.data)[i].Buffer, ((DataString*)input.DataVariableNames.data)[i].Buffer, ((DataString*)input.DataSematicsString.data)[i].Buffer);
	}
}
VSParserResult ParseVSShader(LexerResult& VSLexerOurput)
{
	VSParserResult res;

	res.cbvs = ParseCBV(VSLexerOurput);
	DEBUG_PrintCBV(res.cbvs);
	res.vsInput = ParseVSInput(VSLexerOurput);
	DEBUG_PrintVSInput(res.vsInput);
	res.vsOutput = ParseVSOutput(VSLexerOurput);
	DEBUG_PrintVSOutput(res.vsOutput);
	res.IsValid = true;
	return res;
}
//--------------------------------------------
//			   Pixel Shader
//--------------------------------------------



ParsedRegisters ParseRegisters(LexerResult& Input)
{
	ParsedRegisters res;
	ParsedRegisters* Registers = &res;
	Registers->Types.Init(8, sizeof(u32));
	Registers->Slots.Init(8, sizeof(u32));
	Registers->DataTypes.Init(8, sizeof(DataString));
	Registers->VarNames.Init(8, sizeof(DataString));

	u32 RegisterCount = 0;
	u32 TokenIndicies[64];

	Token* t = DYNAMIC_ARR_GET_CAST_DATA(Token, Input.tokens);
	if (Input.tokens.elementCount < 5)
	{
		printf("Warning: Not enough tokens to parse a register\n");
		return { 0 };
	}
	//format
	u32 Mask = eDefine | eType | eName | eSemanticSeperator | eSlot;
	//check to see if there are any registers.
	for (u32 i = 0; i < Input.tokens.elementCount - 4; i++)
	{
		u32 TypeMask = t[i].Type | t[i + 1].Type | t[i + 2].Type | t[i + 3].Type | t[i + 4].Type;
		if (Mask == TypeMask)
		{
			s32 Slot = GetSlotFromString(t[i + 4].Buffer);
			if (Slot != -1)
			{
				Registers->Types.Add(&t[i].Subtype);
				Registers->DataTypes.Add(&t[i + 1].Buffer);
				Registers->VarNames.Add(&t[i + 2].Buffer);
				Registers->Slots.Add(&Slot);
			}
			i += 4;
		}
	}
	return res;
}

ParsedPSInput ParsePSInput(LexerResult& Input)
{
	ParsedPSInput res;
	ParsedPSInput* PSInput = &res;

	res.DataSematicsString.Init(8, sizeof(DataString));
	res.DataVariableNames.Init(8, sizeof(DataString));
	res.DataTypesString.Init(8, sizeof(DataString));
	res.DataTypesIndex.Init(8, sizeof(u32));
	res.SemanticFlags.Init(8, sizeof(u32));

	
	s32 Start = 0;
	s32 End = 0;

	Token* t = DYNAMIC_ARR_GET_CAST_DATA(Token, Input.tokens);

	for (Start = 0; Start < Input.tokens.elementCount && t[Start].Subtype != ePS_INPUT; Start++);
	for (End = Start; End < Input.tokens.elementCount && !(t[End].Type == eBeginEnd && t[End].Subtype == 1); End++);


	//format
	u32 InputMask = eType | eName | eSemanticSeperator | eSemantic;
	for (s32 i = Start; i < End - 3; i++)
	{
		u32 TypeMask = t[i].Type | t[i + 1].Type | t[i + 2].Type | t[i + 3].Type;
		if (TypeMask == InputMask)
		{
			//Type
			PSInput->DataTypesString.Add(&t[i].Buffer);
			PSInput->DataTypesIndex.Add(&t[i].Subtype);
			//Name
			PSInput->DataVariableNames.Add(&t[i + 1].Buffer);
			//Semantic
			PSInput->DataSematicsString.Add(&t[i + 3].Buffer);
			PSInput->SemanticFlags.Add(&t[i + 3].Subtype);
			i += 3;
		}
	}
	return res;
}

void DEBUG_PrintRegisters(ParsedRegisters& Registers)
{
	printf("\nPS Registers\n");
	for (u32 i = 0; i < Registers.Types.elementCount; i++)
	{
		u32* Type = DYNAMIC_ARR_GET_CAST_DATA(u32, Registers.Types);
		DataString* DataTypeName = DYNAMIC_ARR_GET_CAST_DATA(DataString, Registers.DataTypes);
		DataString* VarName = DYNAMIC_ARR_GET_CAST_DATA(DataString, Registers.VarNames);
		u32* Slots = DYNAMIC_ARR_GET_CAST_DATA(u32, Registers.Slots);
		printf("	%s %s %s : register(%d)\n", DefineStrings[Type[i]], DataTypeName[i], VarName[i], Slots[i]);
	}
}
void DEBUG_PrintPSInput(ParsedPSInput& Input)
{
	printf("\nPS_INPUT\n");
	for (u32 i = 0; i < Input.DataVariableNames.elementCount; i++)
	{
		printf("	%s %s : %s\n", ((DataString*)Input.DataTypesString.data)[i].Buffer, ((DataString*)Input.DataVariableNames.data)[i].Buffer, ((DataString*)Input.DataSematicsString.data)[i].Buffer);
	}
}
PSParserResult ParsePSShader(LexerResult& PSLexerOutput)
{
	PSParserResult res;
	res.registers = ParseRegisters(PSLexerOutput);
	if (res.registers.Slots.elementCount != 0)
	{
		//DEBUG_PrintRegisters(res.registers);
	}	
	res.psInput = ParsePSInput(PSLexerOutput);
	//DEBUG_PrintPSInput(res.psInput);
	res.IsValid = true;
	return res;
}