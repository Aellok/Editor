#include "ShaderCompiler.h"
#include "Platform\Windows\Renderer\DirectX12\d3dx12.h"
#include "Platform\Windows\Renderer\DirectX12\d3dcompiler.h"

D3D_SHADER_MACRO defines[] =
{
    { "CBV", nullptr },
    { "SAMPLER", nullptr },
    { "TEXTURE", nullptr },
    { "UAV", nullptr },
    { "VS_OUTPUT", nullptr },
    { "VS_INPUT", nullptr },
    { "PS_INPUT", nullptr },
    { nullptr, nullptr }
};

CompiledShaderDesc CompileShader(const char* ShaderName,const char* Entry,const char* Version, DynamicArray& ShaderData)
{
	CompiledShaderDesc res;
	ID3DBlob* compiledShader;
	ID3DBlob* Error;

	//Compile the shaders.
	HRESULT hr = D3DCompile2(ShaderData.data, ShaderData.elementCount * ShaderData.elementSize, ShaderName, defines, NULL,
		Entry, Version, D3DCOMPILE_OPTIMIZATION_LEVEL0, NULL, 0, NULL, 0, &compiledShader, &Error);
	if (hr != S_OK)
	{
		printf("Failed to compile: %s\n", (char*)Error->GetBufferPointer());
		return {0,0};
	}
	res.Blob = compiledShader->GetBufferPointer();
	res.Size = (u32)compiledShader->GetBufferSize();
	return res;
}