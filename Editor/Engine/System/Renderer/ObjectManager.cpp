#include "ObjectManager.h"
#include "System/String.h"
#include "Platform/Windows/Renderer/DirectX12/VertexDef.h"
#include "Application\Engine.h"
#include "System\Utils\ObjLoader.h"
#define STATIC_INSTANCED_DATA_OFFSET MAX_TEXTURES * OBJECT2D_MAX_STATIC_INSTANCE
#define GET_S_INSTANCED_OFFSET(mID,tID) (((mID) * MAX_TEXTURES * OBJECT2D_MAX_STATIC_INSTANCE) + ((tID) * OBJECT2D_MAX_STATIC_INSTANCE))
#define GET_D_INSTANCED_OFFSET(mID,tID) (((mID) * MAX_TEXTURES * OBJECT2D_MAX_DYNAMIC_INSTANCE) + ((tID) * OBJECT2D_MAX_DYNAMIC_INSTANCE))

#define MAX_OBJECTS 1024

void CreateNewDefaultCenteredMesh(ObjectManager* ObjManager,void* pPlatformInterface, MemoryArena* Arena)
{
	Vertex2D VertexList[6] =
	{
		{{-1,-1,0,1},{0,0}},
		{{1,-1,0,1},{1,0}},
		{{-1,1,0,1},{0,1}},
		{{-1,1,0,1},{0,1}},
		{{1,-1,0,1},{1,0}},
		{{1,1,0,1},{1,1}}
	};
	u32 IndexList[6] = { 0,1,2,3,4,5 };

	DX12VertexBufferDesc VDesc = { 6, VertexList,6 * sizeof(Vertex2D),sizeof(Vertex2D) };
	VDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(VDesc.Size);
	DX12IndexBufferDesc IDesc = { 6, IndexList, 6 * sizeof(u32) };
	IDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(IDesc.Size);
	
	InstanceDataDesc IDDesc;
	IDDesc.InstanceData = nullptr;
	IDDesc.InstanceDataSize = 0;
	IDDesc.InstanceElementSize = 0;
	ObjManager->Meshes[ObjManager->MeshCount].Init((DirectX12*)pPlatformInterface, Arena, VDesc, IDesc,IDDesc);
	ObjManager->Meshes[ObjManager->MeshCount].IsCentered = true;
	ObjManager->LoadedMeshList[ObjManager->MeshCount++] = "Centered";
}
void CreateNewDefaultTopLeftMesh(ObjectManager* ObjManager, void* pPlatformInterface, MemoryArena* Arena)
{
	Vertex2D VertexList[6] =
	{
		{{0,0,0,1},{0,0}},
		{{1,0,0,1},{1,0}},
		{{0,1,0,1},{0,1}},
		{{0,1,0,1},{0,1}},
		{{1,0,0,1},{1,0}},
		{{1,1,0,1},{1,1}}
	};
	u32 IndexList[6] = { 0,1,2,3,4,5 };

	DX12VertexBufferDesc VDesc = { 6, VertexList,6 * sizeof(Vertex2D),sizeof(Vertex2D) };
	VDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(VDesc.Size);
	DX12IndexBufferDesc IDesc = { 6, IndexList, 6 * sizeof(u32) };
	IDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(IDesc.Size);

	InstanceDataDesc IDDesc;
	IDDesc.InstanceData = nullptr;
	IDDesc.InstanceDataSize = 0;
	IDDesc.InstanceElementSize = 0;
	ObjManager->Meshes[ObjManager->MeshCount].Init((DirectX12*)pPlatformInterface, Arena, VDesc, IDesc, IDDesc);
	ObjManager->Meshes[ObjManager->MeshCount].IsCentered = false;
	ObjManager->LoadedMeshList[ObjManager->MeshCount++] = "TopLeft";
}
void ObjectManager::Init(void* pPlatformInterface,DX12PipelineManager* PipelineManager)
{

	PlatformInterface = pPlatformInterface;
	ObjectCount = 0;
	MeshCount = 0;
	TextureCount = 0;
	StringCount = 0;
	pipelineManager = PipelineManager;
	PipelineObjects2D.Init(pipelineManager->PipelineList2D.elementCount, sizeof(DynamicArray));
	PipelineObjects3D.Init(pipelineManager->PipelineList3D.elementCount, sizeof(DynamicArray));
	PipelineStrings.Init(pipelineManager->PipelineList2D.elementCount, sizeof(DynamicArray));

	Arena.Create(64 * 1024 * 1024); // 32 mb

	for (u32 i = 0; i < pipelineManager->PipelineList2D.elementCount; i++)
	{
		DynamicArray arr = {0};
		PipelineObjects2D.Add(&arr);
		PipelineStrings.Add(&arr);
	}
	for (u32 i = 0; i < pipelineManager->PipelineList3D.elementCount; i++)
	{
		DynamicArray arr = { 0 };
		PipelineObjects3D.Add(&arr);
	}
	
#if !SERVER_MODE

	CreateNewDefaultCenteredMesh(this, pPlatformInterface, &Arena);
	CreateNewDefaultTopLeftMesh(this, pPlatformInterface, &Arena);
#endif
	VecList = (Vector*)GEngine.Global.Allocate(10240 * sizeof(Vector));

	LoadedTextureList[TextureCount] = "NULL";
	Textures[TextureCount++].InitFromFile(&GEngine.pRendererInterface->MainCommandQueue, (DirectX12*)PlatformInterface, NULL);
	
}
void ObjectManager::AddMesh3D(const char* MeshName,void* VertexData,u32 VertexCount,u32 VertexSize,void* IndexData,u32 IndexCount,u32 IndexSize)
{
	DirectX12* DX12 = (DirectX12*)PlatformInterface;
	DX12VertexBufferDesc VDesc = { VertexCount,VertexData,VertexSize,VertexSize / VertexCount };
	VDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(VDesc.Size);
	DX12IndexBufferDesc IDesc = { IndexCount,(u32*)IndexData, IndexSize };
	IDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(IDesc.Size);

	Instance3DDataDesc IDDesc;
	IDDesc.InstanceData = nullptr;
	IDDesc.InstanceDataSize = 0;
	IDDesc.InstanceElementSize = 0;

	Meshes3D[MeshCount3D].Init(DX12, &GEngine.Global, VDesc, IDesc, IDDesc);

	LoadedMeshList3D[MeshCount3D++] = MeshName;
}
void ObjectManager::AddPipeline3D(const char* Name, DX12Pipeline* Pipeline)
{
	u32 Index = pipelineManager->GetPipelineIndex3D(Name);
	if (Index != -1)
	{
		pipelineManager->DeletePipeline3D(Index);
	}
	pipelineManager->AddPipeline3D(Pipeline);
	DynamicArray arr = { 0 };
	PipelineObjects3D.Add(&arr);
}
void ObjectManager::AddPipeline3D(const char* Name, const char* fileName, bool Depth, bool MSAA)
{
	u32 Index = pipelineManager->GetPipelineIndex3D(Name);
	if(Index != -1)
	{
		pipelineManager->DeletePipeline3D(Index);
	}
	pipelineManager->AddPipeline3D(((DirectX12*)PlatformInterface)->device,Name,fileName,Depth,MSAA );
	DynamicArray arr = {0};
	PipelineObjects3D.Add(&arr);
}
void ObjectManager::AddPipeline2D(const char* Name, const char* fileName, bool Depth, bool MSAA)
{
	u32 Index = pipelineManager->GetPipelineIndex2D(Name);
	if (Index != -1)
	{
		pipelineManager->DeletePipeline2D(Index);
	}
	pipelineManager->AddPipeline3D(((DirectX12*)PlatformInterface)->device, Name, fileName, Depth, MSAA);
	DynamicArray arr = { 0 };
	PipelineObjects3D.Add(&arr);
}
void ObjectManager::AddTexture(void* Texture,const char* TextureName,u32 TextureWidth,u32 TextureHeight)
{
	DirectX12* DX12 = (DirectX12*)PlatformInterface;
	if (DX12->LoadCommandQueue.IsClosed)
	{
		DX12->LoadCommandQueue.Reset(nullptr);
	}
	Textures[TextureCount].InitFromData(&DX12->LoadCommandQueue, (DirectX12*)PlatformInterface, TextureName, Texture, TextureWidth, TextureHeight, DXGI_FORMAT_R8G8B8A8_UNORM, true);
	LoadedTextureList[TextureCount++] = TextureName;
	DX12->LoadCommandQueue.ExecuteAndWait();
}
void ObjectManager::AddTexture(const char* TextureName)
{
	DirectX12* DX12 = (DirectX12*)PlatformInterface;
	if (DX12->LoadCommandQueue.IsClosed)
	{
		DX12->LoadCommandQueue.Reset(nullptr);
	}
	LoadedTextureList[TextureCount] = TextureName;
	Textures[TextureCount++].InitFromFile(&DX12->LoadCommandQueue, (DirectX12*)PlatformInterface, TextureName);
	DX12->LoadCommandQueue.ExecuteAndWait();
}
void ObjectManager::AddTexture(DX12Texture* Texture)
{
	DirectX12* DX12 = (DirectX12*)PlatformInterface;
	LoadedTextureList[TextureCount] = Texture->TextureName;
	Textures[TextureCount++] = *Texture;
}
Object2D* ObjectManager::AddObject(Vector Pos, Vector Dim, Vector Rot, Vector Color, const char* ModelFileName, const char* TextureName,const char* PipelineName)
{
	DynamicArray* PipelineObjectList = DYNAMIC_ARR_GET_CAST_DATA(DynamicArray, PipelineObjects2D);
	s32 PipelineIndex = pipelineManager->GetPipelineIndex2D(PipelineName);
	if (PipelineIndex == -1)
	{
		printf("Error: AddObject: Failed to find pipeline %s\n", PipelineName);
		return nullptr;
	}
	DirectX12* DX12 = (DirectX12*)PlatformInterface;
	s32 MeshID = GetStringIndex(LoadedMeshList, MeshCount, ModelFileName);
	if (MeshID == -1) 
	{
		assert(1);
		printf("Cant load model");
		return nullptr;
	}
	s32 TextureID = GetStringIndex(LoadedTextureList, TextureCount, (TextureName == NULL) ? "NULL" : TextureName);
	if (TextureID == -1) 
	{
		LoadedTextureList[TextureCount] = TextureName;
		TextureID = TextureCount;
		Textures[TextureCount++].InitFromFile(&DX12->MainCommandQueue, (DirectX12*)PlatformInterface, TextureName);
	}
	Object2D* Obj = &Objects[ObjectCount];

	Obj->Init(Pos, Dim, Rot, Color);
	Obj->MeshID = MeshID;
	Obj->TextureID = TextureID;
	Obj->PipelineID = PipelineIndex;
	
	if (!PipelineObjectList[PipelineIndex].data)
	{
		u32 NameLength = strlen(PipelineName);
		if (NameLength >= 32)
		{
			printf("Error: Pipeline name longer than buffer\n");
		}
		PipelineObjectList[PipelineIndex].Init(MAX_OBJECTS, sizeof(u32));
	}

	PipelineObjectList[PipelineIndex].Add(&ObjectCount);

	u32* ObjectIDs = DYNAMIC_ARR_GET_CAST_DATA(u32, PipelineObjectList[1]);

	ObjectCount++;
	return Obj;
	
}
Object2D* ObjectManager::AddObjectRaw(Vector Pos, Vector Dim, Vector Rot, Vector Color, const char* ModelFileName, const char* TextureName, DX12Texture* Texture)
{
	DirectX12* DX12 = (DirectX12*)PlatformInterface;
	s32 MeshID = GetStringIndex(LoadedMeshList, MeshCount, ModelFileName);
	if (MeshID == -1)
	{
		assert(1);
		printf("Cant load model");
		return nullptr;
	}
	s32 TextureID = GetStringIndex(LoadedTextureList, TextureCount, (TextureName == NULL) ? "NULL" : TextureName);
	if (TextureID == -1)
	{
		LoadedTextureList[TextureCount] = TextureName;
		TextureID = TextureCount;
		Textures[TextureCount++] = *Texture;
	}
	else
	{
		printf("Texture already exists using that one instead.\n");
	}
	Object2D* Obj = &Objects[ObjectCount];

	Obj->Init(Pos, Dim, Rot, Color);
	Obj->MeshID = MeshID;
	Obj->TextureID = TextureID;

	ObjectCount++;
	return Obj;
}
Object2D* ObjectManager::AddObject(ObjectDesc Desc)
{
	return AddObject(Desc.Pos, Desc.Dim, Desc.Rot, Desc.Color, Desc.ModelFileName, Desc.TextureName[0], Desc.PipelineName);
}

Object3D* ObjectManager::AddObject3D(ObjectDesc Desc)
{
	return AddObject3D(Desc.Pos,Desc.Dim,Desc.Rot,Desc.Color,Desc.ModelFileName,Desc.TextureName, Desc.PipelineName);
}
Object3D* ObjectManager::AddObject3D(Vector Pos, Vector Dim, Vector Rot, Vector Color, const char* ModelFileName, const char** TextureName, const char* PipelineName)
{
	DynamicArray* PipelineObjectList = DYNAMIC_ARR_GET_CAST_DATA(DynamicArray, PipelineObjects3D);
	s32 PipelineIndex = pipelineManager->GetPipelineIndex3D(PipelineName);
	if (PipelineIndex == -1)
	{
		printf("Error: AddObject3D: Failed to find pipeline %s\n", PipelineName);
		return nullptr;
	}
	DirectX12* DX12 = (DirectX12*)PlatformInterface;
	s32 MeshID = GetStringIndex(LoadedMeshList3D, MeshCount3D, ModelFileName);
	if (MeshID == -1)
	{
		ObjData data = LoadObj(ModelFileName);
		
		DynamicArray VertexList;
		DynamicArray IndexList;
		VertexList.Init(data.VertexCount, sizeof(Vertex));
		IndexList.Init(data.FaceCount, sizeof(u32));

		u32* IndexData = DYNAMIC_ARR_GET_CAST_DATA(u32, IndexList);
		Vertex* VertexData = DYNAMIC_ARR_GET_CAST_DATA(Vertex, VertexList);

		for (u32 i = 0; i < data.FaceCount; i++)
		{
			Vertex vert;
			DirectX::XMStoreFloat4(&vert.position, data.VertexList[(s32)data.FaceList[i].m128_f32[0] - 1]);
			vert.position.w = 1;
			DirectX::XMStoreFloat2(&vert.UV, data.UVList[(s32)data.FaceList[i].m128_f32[1] - 1]);
			vert.UV.y = 1 - vert.UV.y;
			DirectX::XMStoreFloat3(&vert.Normal, data.NormalList[(s32)data.FaceList[i].m128_f32[2] - 1]);
			
			VertexList.Add(&vert);
			IndexList.Add(&i);
		}

		DX12VertexBufferDesc VDesc = { data.FaceCount,VertexList.data,data.FaceCount * sizeof(Vertex),sizeof(Vertex)};
		VDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(VDesc.Size);
		DX12IndexBufferDesc IDesc = { data.FaceCount,(u32*)IndexList.data, data.FaceCount * sizeof(u32) };
		IDesc.ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(IDesc.Size);
		
		Instance3DDataDesc IDDesc;
		IDDesc.InstanceData = nullptr;
		IDDesc.InstanceDataSize = 0;
		IDDesc.InstanceElementSize = 0;
		Meshes3D[MeshCount3D].Init(DX12, &GEngine.Global, VDesc, IDesc, IDDesc);

		LoadedMeshList3D[MeshCount3D++] = ModelFileName;
		MeshID = MeshCount3D - 1;
	}
	Object3D* Obj = &Objects3D[ObjectCount3D];
	Obj->Init(Pos, Dim, Rot);

	for (u32 i = 0; TextureName[i] != nullptr; i++)
	{
		s32 TextureID = GetStringIndex(LoadedTextureList, TextureCount, (TextureName[i] == NULL) ? "NULL" : TextureName[i]);
		if (TextureID == -1)
		{
			LoadedTextureList[TextureCount] = TextureName[i];
			TextureID = TextureCount;
			Textures[TextureCount++].InitFromFile(&DX12->MainCommandQueue, (DirectX12*)PlatformInterface, TextureName[i]);
		}
		Obj->TextureID[i] = TextureID;
		Obj->TextureCount++;
	}
	
	
	Obj->MeshID = MeshID;
	Obj->PipelineID = PipelineIndex;
	Obj->Color = Color;
	if (!PipelineObjectList[PipelineIndex].data)
	{
		u32 NameLength = strlen(PipelineName);
		if (NameLength >= 32)
		{
			printf("Error: Pipeline name longer than buffer\n");
		}
		PipelineObjectList[PipelineIndex].Init(MAX_OBJECTS, sizeof(u32));
	}

	PipelineObjectList[PipelineIndex].Add(&ObjectCount3D);

	ObjectCount3D++;
	return Obj;
}
void ObjectManager::RemoveObject(Object2D* Obj)
{
	
}
void RemoveControlChars(char* String)
{
	u32 TotalLength = strlen(String);
	char* ptr = String;
	u32 i = 0;
	for (i = 0; ptr = strchr(ptr, '\r'); i++)
	{
		u32 relative = ptr - String;
		memcpy(ptr, ptr + 1, --TotalLength - relative);
		String[TotalLength] = 0;
	}
	
}


void ObjectManager::UpdateString(String2D* StringObj, char* String, u32 Size)
{
	Font* font = GEngine.GetClosestFont(Size);

	DX12String2D* str = &StringMeshs[StringObj->Index];
	str->InstanceBufferData.Clear();
	str->data.Copy(String, strlen(String));
	StringObj->Info.Clear();

	str->Length = strlen(String);

	u32 NLCount = GetNewLineCount(String) + 1;

	RemoveControlChars(String);

	if (str->Length > 0)
	{
		font->GetStringPositions(String, StringObj->Pos, Size, &StringObj->Info);

		StringObj->PixelLength = StringObj->Info.PixelLength;
		StringObj->PixelHeight = StringObj->Info.PixelHeight;
		StringObj->YPadding = StringObj->Info.YPadding;

		u32 StringIndex = 0;
		for (u32 LineIndex = 0; LineIndex < StringObj->Info.LineCount; LineIndex++)
		{
			StringLineInfo* Info = &StringObj->Info.LineInfo[LineIndex];
			for (u32 i = 0; i < Info->CharCount; i++)
			{
				char c = Info->GetChar(i);

				Object2D Obj;
				Vector BitmapDim = { (f32)font->Glyphs[c].BitmapWidth,(f32)font->Glyphs[c].BitmapHeight };
				Obj.Init(Info->GetPosition(i), BitmapDim, { 0,0 }, { 0,0,0,0 });

				FontInstanceBuffer2D Instance;
				DirectX::XMStoreFloat4x4(&Instance.model, DirectX::XMMatrixTranspose(Obj.GetModelMatrix()));
				Instance.UVPercent = { BitmapDim.m128_f32[0] / Size,BitmapDim.m128_f32[1] / Size };
				Instance.Index = c;
				//TODO: Find a better way to do this. as not all font shaders will have this starting signature.
				str->InstanceBufferData.PartialAdd(&Instance,sizeof(FontInstanceBuffer2D));
				
				StringIndex++;
			}
		}
	}
}

String2D* ObjectManager::AddString(const char* String, Vector Pos, u32 Size,const char* PipelineName)
{
	Font* font = GEngine.GetClosestFont(Size);
	DX12String2D* DX12String = &StringMeshs[StringCount];
	String2D* Result = &Strings[StringCount];
	DynamicArray* PipelineList = DYNAMIC_ARR_GET_CAST_DATA(DynamicArray, PipelineStrings);

	s32 PipelineIndex = pipelineManager->GetPipelineIndex2D(PipelineName);

	if (PipelineIndex == -1)
	{
		printf("Error: AddString: Failed to find pipeline %s\n", PipelineName);
		return nullptr;
	}

	if (!PipelineList[PipelineIndex].data)
	{
		u32 NameLength = strlen(PipelineName);
		if (NameLength >= 32)
		{
			printf("Error: Pipeline name longer than buffer\n");
		}
		PipelineList[PipelineIndex].Init(8, sizeof(u32));
	}
	
	
	PipelineList[PipelineIndex].Add(&StringCount);

	DX12String->Init(String, pipelineManager->GetPipeline2D(PipelineIndex)->InstanceBufferElementSize);

	Result->TextSize = Size;
	Result->Pos = Pos;
	Result->Index = StringCount;
	Result->StringIndex = StringCount;
	Result->PipelineIndex = PipelineIndex;

	u32 NLCount = GetNewLineCount((char*)String) + 1;
	
	Result->Info.Initialize(50);

	UpdateString(Result, (char*)String, Size);
	StringCount++;
	//Strings[StringCount].PixelLength = VecList[StringMeshs[StringCount].Length - 1].m128_f32[0] - Pos.m128_f32[0] + Size;
	return Result;
}
void ObjectManager::Draw(DX12CommandQueue* Queue)
{ 
	Update();
#if !SERVER_MODE
	DirectX12* DX12 = GEngine.pRendererInterface;
	//3D
	for (u32 i = 0; i < PipelineObjects3D.elementCount; i++)
	{
		DynamicArray* PipelineLists = DYNAMIC_ARR_GET_CAST_DATA(DynamicArray, PipelineObjects3D);
		u32* ObjectIDs = DYNAMIC_ARR_GET_CAST_DATA(u32, PipelineLists[i]);

		if (PipelineLists[i].elementCount == 0)
		{
			continue;
		}

		pipelineManager->SetPipeline3D(Queue, DX12->frameIndex, i, true);

		for (u32 ObjIndex = 0; ObjIndex < PipelineLists[i].elementCount; ObjIndex++)
		{
			Object3D* Obj = &Objects3D[ObjectIDs[ObjIndex]];
			u32 Index = 2;
			if (Obj->Visible)
			{
				Obj->Update();
				Meshes3D[Obj->MeshID].Model = Obj->GetModelMatrix();
				for (u32 i = 0; i < Obj->TextureCount; i++)
				{
					Textures[Obj->TextureID[i]].SetTexture(Queue,2 + i);
				}
				
				Meshes3D[Obj->MeshID].Draw(Queue, pipelineManager->CurrentPipeline);
				Obj->Visible = false;
			}
		}
	}

	//Draw 2D
	for (u32 i = 0; i < PipelineObjects2D.elementCount; i++)
	{
		DynamicArray* PipelineLists = DYNAMIC_ARR_GET_CAST_DATA(DynamicArray, PipelineObjects2D);
		u32* ObjectIDs = DYNAMIC_ARR_GET_CAST_DATA(u32, PipelineLists[i]);
		
		if (PipelineLists[i].elementCount == 0)
		{
			continue;
		}

		pipelineManager->SetPipeline2D(Queue, DX12->frameIndex, i, true);

		for (u32 ObjIndex = 0; ObjIndex < PipelineLists[i].elementCount;ObjIndex++)
		{
			Object2D* Obj = &Objects[ObjectIDs[ObjIndex]];
			if (Obj->Visible)
			{
				Meshes[Obj->MeshID].Model = Obj->GetModelMatrix();
				Textures[Obj->TextureID].SetTexture(Queue,2);
				Meshes[Obj->MeshID].Draw(Queue, pipelineManager->CurrentPipeline,Obj->Color);
				Obj->Update(Meshes[Obj->MeshID].IsCentered);
				Obj->Visible = false;
			}
		}
	}
	//Draw Strings
	for (u32 i = 0; i < PipelineObjects2D.elementCount; i++)
	{
		DynamicArray* PipelineLists = DYNAMIC_ARR_GET_CAST_DATA(DynamicArray, PipelineStrings);
		u32* StringIDs = DYNAMIC_ARR_GET_CAST_DATA(u32, PipelineLists[i]);
		if (PipelineLists[i].elementCount == 0)
		{
			continue;
		}
		
		pipelineManager->SetPipeline2D(Queue, DX12->frameIndex, i, true);

		for (u32 ObjIndex = 0; ObjIndex < PipelineLists[i].elementCount; ObjIndex++)
		{
			String2D* Str = &Strings[StringIDs[ObjIndex]];
			if (Str->Visible)
			{
				GEngine.GetClosestFont(Str->TextSize)->TextureArray.SetTexture(Queue,2);
				StringMeshs[Str->StringIndex].Draw(Queue, pipelineManager->CurrentPipeline);
				Str->Visible = false;
			}
		}
		
	}
#endif
}

u32 ObjectManager::GetStringLength(const char* String,const char* FontName,u32 Size)
{
#if !SERVER_MODE
	s32 FontIndex = GetStringIndex(LoadedFonts, FontCount, FontName);
	if ( FontIndex == -1)
	{
		//fonts[FontCount].LoadFont((DirectX12*)PlatformInterface, &GEngine.Global);
		LoadedFonts[FontCount++] = FontName;
		FontIndex = FontCount - 1;
	}
	return fonts[FontIndex].GetFontStringLength(String);
#else
	return 1;
#endif;
	
	
}

void ObjectManager::Update()
{
	//DX12Pipeline* pipelines = DYNAMIC_ARR_GET_CAST_DATA();
	//for (u32 i = 0; i < )
	//{
		
	//}
}