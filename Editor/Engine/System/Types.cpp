#include "Types.h"

u32 builtin_clz(u32 x)
{
	unsigned long res = 0;
	_BitScanForward64(&res, x);
	return (u32)res;
}