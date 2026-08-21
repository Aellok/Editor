#include "AABB2D.h"
#include <math.h>
#include <emmintrin.h>  // SSE2 base
#include <tmmintrin.h>  // SSSE3
#include <smmintrin.h>  // SSE4.1 (required for _mm_min_epi32)
bool AABB2D::Intersect(AABB2D& other)
{
	return !(x1 > other.x2 || 
			 x2 < other.x1 ||
			 y1 > other.y2 ||
			 y2 < other.y1);
}

AABB2D AABB2D::GetRelativePosition( AABB2D& box )
{
	AABB2D res;
	__m128 d = _mm_loadu_ps(&box.IData.x);
	__m128 d2 = _mm_loadu_ps(&IData.x);
	_mm_storeu_ps(&res.IData.x, _mm_sub_ps(d, d2));
	return res;
}

AABB2D AABB2D::GetRelativeDim(AABB2D& box)
{
	__m128 a = _mm_loadu_ps(&box.IData.x);
	__m128 b = {x2 - x1,y2 - y1,x2 - x1,y2 - y1 };

	__m128 sub = _mm_sub_ps(a, b);         // element-wise subtraction
	__m128 add = _mm_add_ps(a, b);         // element-wise addition

	__m128 res = _mm_blend_ps(sub, add, 0b1100);
	
	AABB2D ModifiedDim;
	_mm_storeu_ps(&ModifiedDim.IData.x, res);

	AABB2D result = GetRelativePosition(ModifiedDim);

	return result;
}