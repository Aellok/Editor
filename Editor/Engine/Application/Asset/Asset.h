#pragma once

#include "System\Types.h"

struct Asset
{
	struct 
	{
		u32 ShaderSize[5];
		u32 VertexSize;
		u32 VertexCount;
		u32 IndexSize;
		u32 IndexCount;
		u32 TextureCount;
	} Header;

	char* FilePath;
	//Debug Only
	char* ShaderName[5];
	
	char* PipelineName;
	
	void* Verticies;
	void* Indicies;
	char** Textures;
};

Asset* LoadAsset(const char* Path);
