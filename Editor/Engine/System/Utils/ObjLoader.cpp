#include "ObjLoader.h"
#include "File.h"
#include <stdio.h>
#include <string.h>

void SkipLine(char** str)
{
	for (u32 i = 0; i < 255;i++)
	{
		if (*((*str)++) == '\n')
		{
			return;
		}
	}
}
f32 ParseValue(char** Str)
{
	char Buffer[255] = {0};
	u32 i = 0;
	while (**Str != ' ' && **Str != '\n')
	{
		Buffer[i++] = *((*Str)++);
		if (**Str == '/')
		{
			*Str += 1;
			break;
		}
	}

	return (f32)atof(Buffer);
}
Vector ParseVertexData(char** Str)
{
	u32 i = 0;

	Vector Result = {0,0,0,0};
	
	while(*((*Str)++) != '\n')
	{
		Result.m128_f32[i++] = ParseValue(Str);
	}
	return Result;
}
Vector ParseVertexIndexData(char** Str)
{
	Vector Result = {0};
	u32 i = 0;
	*Str += 1; // Get rid of Space;

	while(**Str != '\n' && **Str != ' ')
	{
		Result.m128_f32[i++] = ParseValue(Str);
	}
	return Result;
}
ObjData LoadObj(const char* FileName)
{
	ObjData Result = {};
	Result.VertexListSize = 16 * sizeof(Vector);
	Result.VertexList = (Vector*)calloc(1, Result.VertexListSize );
	Result.VertexCount = 0;

	Result.FaceListSize = 64 * sizeof(u32);
	Result.FaceList = (Vector*)calloc(1, Result.FaceListSize);
	Result.FaceCount = 0;

	Result.UVListSize = 16 * sizeof(u32);
	Result.UVList = (Vector*)calloc(1, Result.FaceListSize);
	Result.UVCount = 0;

	Result.NormalListSize = 16 * sizeof(Vector);
	Result.NormalList = (Vector*)calloc(1, Result.NormalListSize);
	Result.NormalCount = 0;

	File file;
	file.Load(FileName);

	if (!file.Data)
	{
		return Result;
	}

	char* Str = (s8*)file.Data;
	while(Str < (s8*)file.Data + file.FileSize)
	{
		if (*Str == '#' || *Str == 'o' || *Str == 's') // skip unwanted lines;
		{
			SkipLine(&Str);
			continue;
		}

		//extract data;
		if (*Str == 'v' && *(Str + 1) == ' ')
		{
			Str += 1; // header
			if (Result.VertexCount * sizeof(Result.VertexList[0]) == Result.VertexListSize)
			{
				Result.VertexListSize = Result.VertexListSize << 1;
				Result.VertexList = (Vector*)realloc(Result.VertexList, Result.VertexListSize);
			}
			Result.VertexList[Result.VertexCount] = ParseVertexData(&Str);
			
			Result.BoundingBox.Min.m128_f32[0] = fminf(Result.VertexList[Result.VertexCount].m128_f32[0],Result.BoundingBox.Min.m128_f32[0]);
			Result.BoundingBox.Min.m128_f32[1] = fminf(Result.VertexList[Result.VertexCount].m128_f32[1], Result.BoundingBox.Min.m128_f32[1]);
			Result.BoundingBox.Min.m128_f32[2] = fminf(Result.VertexList[Result.VertexCount].m128_f32[2], Result.BoundingBox.Min.m128_f32[2]);

			Result.BoundingBox.Max.m128_f32[0] = fmaxf(Result.VertexList[Result.VertexCount].m128_f32[0], Result.BoundingBox.Max.m128_f32[0]);
			Result.BoundingBox.Max.m128_f32[1] = fmaxf(Result.VertexList[Result.VertexCount].m128_f32[1], Result.BoundingBox.Max.m128_f32[1]);
			Result.BoundingBox.Max.m128_f32[2] = fmaxf(Result.VertexList[Result.VertexCount++].m128_f32[2], Result.BoundingBox.Max.m128_f32[2]);
			Result.BoundingBox.Center.m128_f32[0] = (Result.BoundingBox.Max.m128_f32[0] + Result.BoundingBox.Min.m128_f32[0]) * 0.5f;
			Result.BoundingBox.Center.m128_f32[1] = (Result.BoundingBox.Max.m128_f32[1] + Result.BoundingBox.Min.m128_f32[1]) * 0.5f;
			Result.BoundingBox.Center.m128_f32[2] = (Result.BoundingBox.Max.m128_f32[2] + Result.BoundingBox.Min.m128_f32[2]) * 0.5f;
		}
		else if (*Str == 'f' && *(Str + 1) == ' ')
		{
			Str += 1; // Get rid of header
			
			for (u32 i = 0; i < 3; i++)
			{
				if (Result.FaceCount * sizeof(Result.FaceList[0]) == Result.FaceListSize)
				{
					Result.FaceListSize = Result.FaceListSize << 1;
					Result.FaceList = (Vector*)realloc(Result.FaceList, Result.FaceListSize);
				}
				Result.FaceList[Result.FaceCount++] = ParseVertexIndexData(&Str);
			}
		}
		else if (*Str == 'v' && *(Str + 1) == 't')
		{
			Str += 2; // Get rid of header
			if (Result.UVCount * sizeof(Result.UVList[0]) == Result.UVListSize)
			{
				Result.UVListSize = Result.UVListSize << 1;
				Result.UVList = (Vector*)realloc(Result.UVList, Result.UVListSize);
			}
			Result.UVList[Result.UVCount++] = ParseVertexData(&Str);
		}
		else if (*Str == 'v' && *(Str + 1) == 'n')
		{
			Str += 2;
			if (Result.NormalCount * sizeof(Result.NormalList[0]) == Result.NormalListSize)
			{
				Result.NormalListSize = Result.NormalListSize << 1;
				Result.NormalList = (Vector*)realloc(Result.NormalList, Result.NormalListSize);
			}
			Result.NormalList[Result.NormalCount++] = ParseVertexData(&Str);
		}
		else
		{
			Str++;
		}
	}
	Result.VertexListSize = Result.VertexCount * sizeof(Vector);
	Result.VertexList = (Vector*)realloc(Result.VertexList, Result.VertexListSize);
	Result.FaceListSize = Result.FaceCount * sizeof(Vector);
	Result.FaceList = (Vector*)realloc(Result.FaceList, Result.FaceListSize);
	Result.UVListSize = Result.UVCount * sizeof(Vector);
	Result.UVList = (Vector*)realloc(Result.UVList, Result.UVListSize);
	Result.NormalListSize = Result.NormalCount * sizeof(Vector);
	Result.NormalList = (Vector*)realloc(Result.NormalList, Result.NormalListSize);
	file.Close();
	return Result;
}