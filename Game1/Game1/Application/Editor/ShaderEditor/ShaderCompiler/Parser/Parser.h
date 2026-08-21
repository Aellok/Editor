#pragma once
#include "ParserDataTypes.h"
#include "..\Lexer\LexerDataTypes.h"

VSParserResult ParseVSShader(LexerResult& VSLexerOurput);

PSParserResult ParsePSShader(LexerResult& PSLexerOutput);