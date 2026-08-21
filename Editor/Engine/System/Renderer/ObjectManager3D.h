#pragma once
#include "Object3D.h"

#include "Platform/Windows/Renderer/DirectX12/DX12Object3D.h"
#include "Platform/Windows/Renderer/DirectX12/DX12Texture.h"
#define MAX_OBJECTS 255 * 255
#define MAX_MESHES  16

struct ObjectManager3D
{
	void* platformInterface;
	
	u32 ObjectCount;
	Object3D Objects[MAX_OBJECTS];
	
	u32 MeshCount;
	DX12Object3D Meshes[MAX_MESHES]; // TODO: figure out how to not use this directly (DX12Object3D)
	const char* LoadedMeshList[MAX_MESHES];

	u32 TextureCount;
	DX12Texture Textures[MAX_MESHES];
	const char* LoadedTextureList[MAX_MESHES];
	

	void Init(void* PlatformInterface) {};

	Object3D* AddNewObject(Vector Pos, Vector Dim, Vector Rot, const char* ModelFileName, const char* TextureName) {};
	void DeleteObject(Object3D Object) {};
	void Draw() {};
	void FinishedLoad() {};
};