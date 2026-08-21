#pragma once
#include "System\Memory\DynamicArray.h"

struct CompiledShaderDesc
{
	u32 Size;
	void* Blob;
};

CompiledShaderDesc CompileShader(const char* ShaderName, const char* Entry, const char* Version, DynamicArray& ShaderData);
