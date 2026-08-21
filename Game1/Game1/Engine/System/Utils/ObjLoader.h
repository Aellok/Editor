#pragma once

#include "System/Types.h"
#include "System/Vector.h"
#include "System/Physics/AABB.h"
struct ObjData
{
	u32 VertexListSize;
	u32 VertexCount;
	Vector* VertexList;

	u32 FaceListSize;
	u32 FaceCount;
	Vector* FaceList;

	u32 UVListSize;
	u32 UVCount;
	Vector* UVList;

	u32 NormalListSize;
	u32 NormalCount;
	Vector* NormalList;
	//TODO: add normals and tangents and things

	AABB BoundingBox;
};

ObjData LoadObj(const char* path);