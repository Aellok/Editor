#include "Lexer.h"
#include <string>

Token GetTokenFromWord(char* Word, u32 Length, Token& LastToken)
{
	Token token;
	Token* t = &token;
	token.Type = (LexerTokenTypes)0xffffffff;
	if (Length == 0)
	{
		return token;
	}
	memset(token.Buffer, 0, 32);
	
	memcpy(token.Buffer, Word, Length);
	token.Buffer[Length - 1] = Word[Length - 1] == ';' ? 0 : Word[Length - 1];
	
	if (LastToken.Type == eType)
	{
		token.Type = eName;
		return token;
	}
	
	s32 SubType = IsType(token.Buffer);
	if (SubType != -1)
	{
		token.Type = eType;
		token.Subtype = SubType;
		return token;
	}
	SubType = IsDefine(token.Buffer);
	if (SubType != -1)
	{
		token.Type = eDefine;
		token.Subtype = SubType;
		return token;
	}
	SubType = IsSemanticSeperator(token.Buffer);
	if (SubType != -1)
	{
		token.Subtype = 1;
		token.Type = eSemanticSeperator;
	}

	SubType = IsBeginEnd(token.Buffer);
	if (SubType != -1)
	{
		token.Type = eBeginEnd;
		token.Subtype = SubType;
		return token;
	}
	SubType = IsSlot(token.Buffer);
	if (SubType != -1)
	{
		token.Type = eSlot;
		token.Subtype = SubType;
		return token;
	}
	
	if (LastToken.Type == eSemanticSeperator)
	{
		token.Type = eSemantic;
		token.Subtype = IsInst(token.Buffer);
		return token;
	}

	return token;
}

LexerResult ParseHLSL(StringInfo& HLSL)
{
	LexerResult result;
	result.tokens.Init(4 * HLSL.LineCount,sizeof(Token)); // Guessing that there is atleast 4 tokens per line on average.
	
	Token LastToken = {(LexerTokenTypes)0xffffffff,0xffffffff};
	
	for (u32 i = 0; i < HLSL.LineCount; i++)
	{
		char* Start = HLSL.LineInfo[i].Contents.data;
		while (Start - HLSL.LineInfo[i].Contents.data < HLSL.LineInfo[i].Contents.elementCount)
		{	
			char* Space = strchr(Start, ' ');
			u32 Length = (Space ? Space - Start : HLSL.LineInfo[i].Contents.elementCount - (Start - HLSL.LineInfo[i].Contents.data));
			Token t = GetTokenFromWord(Start,Length  , LastToken);
			if (t.Type != 0xffffffff)
			{
				if (t.Type == eSemantic)
				{
					t.Subtype = t.Subtype << 31 | GetSemanticSlot(t.Buffer);
				}
				result.tokens.Add(&t);
				LastToken = t;
			}
			Start += Length + 1;
		} 
	}
	return result;
}