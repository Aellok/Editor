#pragma once
#include "Object2D.h"
#include "Object3D.h"
#include "Platform/Windows/Renderer/DirectX12/DX12Texture.h"
#include "Platform/Windows/Renderer/DirectX12/DX12Object2D.h"
#include "Platform/Windows/Renderer/DirectX12/DX12Object3D.h"
#include "Platform/Windows/Renderer/DirectX12/DX12VertexBuffer.h"
#include "Platform\Windows\Renderer\DirectX12\DX12String2D.h"
#include "Font\String2D.h"
#include "Font\FontRenderer.h"
#include "Engine\System\Memory\Arena.h"
#include "Engine\System\Network\NetworkObject.h"
#include "Platform/Windows/Renderer/DirectX12/DX12PipelineManager.h"
#define MAX_OBJECTS_2D	1024
#define MAX_TEXTURES	512
#define MAX_MESHES		16
#define MAX_STRINGS     255
struct StringObject;
struct ObjectDesc
{
	void* Parent;
	Vector Pos;
	Vector Dim;
	Vector Rot;
	Vector Color;
	const char* PipelineName;
	const char* ModelFileName;
	const char* TextureName;
	bool IsStatic;
	bool HasAlpha;
	bool PhysEnabled;
	bool Networked;
	
};
struct ObjectManager
{
	void* PlatformInterface;
	
	MemoryArena Arena;
	
	u32 ObjectCount;
	Object2D Objects[MAX_OBJECTS_2D];

	u32 MeshCount;
	DX12Object2D Meshes[MAX_OBJECTS_2D];
	const char* LoadedMeshList[MAX_OBJECTS_2D];

	u32 ObjectCount3D;
	Object3D Objects3D[MAX_OBJECTS_2D];

	u32 MeshCount3D;
	DX12Object3D Meshes3D[MAX_OBJECTS_2D];
	const char* LoadedMeshList3D[MAX_OBJECTS_2D];

	u32 TextureCount;
	DX12Texture Textures[MAX_TEXTURES];
	const char* LoadedTextureList[MAX_TEXTURES];

	u32 StringCount;
	DX12String2D StringMeshs[MAX_STRINGS];
	String2D Strings[MAX_STRINGS];

	u32 NetworkObjectCount;
	NetworkObject NetworkObjects[MAX_OBJECTS_2D];

	u32* StaticInstancedMeshCount;
	InstanceBuffer2D* StaticInstancedModelList;

	u32* DynamicInstancedMeshCount;
	InstanceBuffer2D* DynamicInstancedModelList;

	u32 FontCount;
	Font fonts[4];
	const char* LoadedFonts[4];

	u64 LastUUID;

	Vector* VecList;

	DX12PipelineManager* pipelineManager;
	
	u32 ObjPipelineNamesCount;
	u32 StrPipelineNamesCount;

	//DynamicArray
	DynamicArray PipelineObjects2D;
	//DynamicArray
	DynamicArray PipelineObjects3D;
	//DynamicArray
	DynamicArray PipelineStrings;

	void          Init(void* pPlatformRenderer, DX12PipelineManager* PipelineManager);
	void          Update();
	void          Draw(DX12CommandQueue* Queue);
	Object2D*     AddObject(ObjectDesc Desc);
	Object2D*	  AddObject(Vector Pos, Vector Dim, Vector Rot, Vector Color, const char* ModelFileName, const char* TextureName, const char* PipelineName);
	Object3D*	  AddObject3D(ObjectDesc Desc);
	Object3D*	  AddObject3D(Vector Pos, Vector Dim, Vector Rot, Vector Color, const char* ModelFileName, const char* TextureName, const char* PipelineName);
	Object2D*     AddObjectRaw(Vector Pos, Vector Dim, Vector Rot, Vector Color, const char* ModelFileName, const char* TextureName, DX12Texture* Texture);
	String2D*	  AddString(const char* String, Vector Pos, u32 Size, const char* PipelineName);
	void		  AddMesh3D(const char* MeshName,void* VertexData, u32 VertexCount, u32 VertexSize, void* IndexData, u32 IndexCount, u32 IndexSize);
	void		  AddPipeline2D( const char* Name, const char* fileName, bool Depth, bool MSAA);
	void		  AddPipeline3D(const char* Name, const char* fileName, bool Depth, bool MSAA);
	void		  AddPipeline3D(const char* Name, DX12Pipeline* Pipeline);
	void          UpdateString(String2D* StringObj, char* String, u32 Size);
	void		  RemoveObject(Object2D* Obj);
	u32           GetStringLength( const char* String, const char* FontName, u32 Size);
	void		  AddTexture(const char* TextureName);
	void		  AddTexture(DX12Texture* Texture);
	void		  AddTexture(void* Texture, const char* TextureName, u32 TextureWidth, u32 TextureHeight);
};