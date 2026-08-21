#pragma once
#include "System\Types.h"
#include "System\Vector.h"

struct AABB2D
{
	union
	{
		DirectX::XMFLOAT4 IData;
		struct 
		{
			f32 x1;
			f32 y1;
			f32 x2;
			f32 y2;
		};
	};
	
	bool Intersect(AABB2D& other);
	AABB2D GetRelativePosition(AABB2D& box);
	AABB2D GetRelativeDim(AABB2D& box);
};
