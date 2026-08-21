#include "Asset.h"
#include "System/Utils/File.h"
#include "Platform\Windows\Renderer\DirectX12\VertexDef.h"
#define MAX_PATH 260
Asset* LoadAsset(const char* Path)
{
	u32 ShaderCount = 5;
	File file;
	file.Load(Path);

	//setup the ptrs.
	Asset* asset = (Asset*)calloc(1, file.FileSize + sizeof(Asset));
	file.Read(&asset->Header, sizeof(asset->Header));

	char* Cursor = (char*)asset + sizeof(Asset);

	for (u32 i = 0; i < ShaderCount; i++)
	{
		asset->ShaderName[i] = Cursor;
		Cursor += asset->Header.ShaderSize[i];
	}
	asset->PipelineName = Cursor;
	Cursor += MAX_PATH;
	
	asset->Verticies = Cursor;
	Cursor += asset->Header.VertexSize;

	asset->Indicies = Cursor;
	Cursor += asset->Header.IndexSize;

	asset->Textures = (char**)Cursor;
	Cursor += sizeof(char*) * asset->Header.TextureCount;

	for (u32 i = 0; i < asset->Header.TextureCount; i++)
	{
		asset->Textures[i] = Cursor;
		Cursor += MAX_PATH;
	}

	//init
	for (u32 i = 0; i < ShaderCount; i++)
	{
		file.Read(asset->ShaderName[i], asset->Header.ShaderSize[i]);
	}
	file.Read(asset->PipelineName, MAX_PATH);
	file.Read(asset->Verticies, asset->Header.VertexSize);
	
	printf("alignof(Vertex) = %zu\n", alignof(Vertex));
	printf("vertex address %% alignment = %llu\n",
		(unsigned long long)((uintptr_t)asset->Verticies % alignof(Vertex)));

	for (u32 i = 0; i < asset->Header.VertexCount; i++)
	{
		Vertex vert = ((Vertex*)asset->Verticies)[i];
		printf("%f%f%f\n",vert.position.x, vert.position.y, vert.position.z);
	}
	file.Read(asset->Indicies, asset->Header.IndexSize);
	for (u32 i = 0; i < asset->Header.TextureCount; i++)
	{
		file.Read(asset->Textures[i], MAX_PATH);
	}


	return asset;
}