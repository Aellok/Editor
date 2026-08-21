#include "ObjectManager3D.h"
#include <string.h>
#include "System/Utils/ObjLoader.h"
#include "Platform/Windows/Renderer/DirectX12/VertexDef.h"
#include "System/String.h"
#include "Application\Engine.h"
/*
void ObjectManager3D::Init(void* PlatformInterface)
{
	platformInterface = PlatformInterface;
	ObjectCount = 0;
	MeshCount = 0;
	TextureCount = 0;
}
Object3D* ObjectManager3D::AddNewObject(Vector Pos,Vector Dim, Vector Rot,const char* ModelFileName, const char* TextureName)
{
	Objects[ObjectCount].Init(Pos, Dim, Rot);
	
	Objects[ObjectCount].ModelID = GetStringIndex(LoadedMeshList,MeshCount,ModelFileName);
	Objects[ObjectCount].TextureID = GetStringIndex(LoadedTextureList, TextureCount, TextureName);

	if (Objects[ObjectCount].ModelID == -1)
	{
#if !SERVER_MODE
		//Load model
		Objects[ObjectCount].ModelID = MeshCount;

		LoadedMeshList[MeshCount] = ModelFileName;

		ObjData data = LoadObj(ModelFileName);

		Vertex* VertexList = (Vertex*)calloc(data.FaceCount, sizeof(Vertex));
		u32* IndexList = (u32*)calloc(data.FaceCount, sizeof(u32));

		for (u32 i = 0; i < data.FaceCount; i++)
		{
			VertexList[i].position = { data.VertexList[(int)data.FaceList[i].m128_f32[0] - 1].m128_f32[0],
									   data.VertexList[(int)data.FaceList[i].m128_f32[0] - 1].m128_f32[1],
									   data.VertexList[(int)data.FaceList[i].m128_f32[0] - 1].m128_f32[2] };
			VertexList[i].UV = { data.UVList[(int)data.FaceList[i].m128_f32[1] - 1].m128_f32[0],
								 (1.0f - data.UVList[(int)data.FaceList[i].m128_f32[1] - 1].m128_f32[1]) };
			
			VertexList[i].Normal = { data.NormalList[(int)data.FaceList[i].m128_f32[2] - 1].m128_f32[0],
									 data.NormalList[(int)data.FaceList[i].m128_f32[2] - 1].m128_f32[1],
									 data.NormalList[(int)data.FaceList[i].m128_f32[2] - 1].m128_f32[2] };
			IndexList[i] = i;
		}

		DX12VertexBufferDesc VBuffer = { data.FaceCount,VertexList,data.FaceCount * sizeof(Vertex) };
		DX12IndexBufferDesc IBuffer = { data.FaceCount,IndexList, data.FaceCount * sizeof(u32) };

		Meshes[MeshCount++].Init((DirectX12*)platformInterface, &GEngine.Global, VBuffer, IBuffer, {0});

		Objects[ObjectCount].BoundingBox = data.BoundingBox;

		free(data.FaceList);
		free(data.VertexList);
		free(data.UVList);
#endif
	}

	if (Objects[ObjectCount].TextureID == -1 )
	{
#if !SERVER_MODE
		LoadedTextureList[TextureCount] = TextureName;
		Objects[ObjectCount].TextureID = TextureCount;
		//NOTE this will fail if its networked.
		Textures[TextureCount++].InitFromFile(&GEngine.pRendererInterface->MainCommandQueue,(DirectX12*)platformInterface, TextureName);
#endif
	}

	
	return &Objects[ObjectCount++];
	
}
void ObjectManager3D::DeleteObject(Object3D Object)
{

}



void ObjectManager3D::Draw()
{
	for (u32 i = 0; i < ObjectCount;i++)
	{
		if (Objects[i].Visible)
		{
			//Meshes[Objects[i].ModelID].Model = Objects[i].GetModelMatrix();
			//Textures[Objects[i].TextureID].SetTexture(&GEngine.pRendererInterface->MainCommandQueue);
			//Meshes[Objects[i].ModelID].Draw(nullptr, {0});
			//Objects[i].Visible = false;
		}
	}
}
void ObjectManager3D::FinishedLoad()
{
	//((DirectX12*)platformInterface)->ExecuteCmdList();
}
*/
