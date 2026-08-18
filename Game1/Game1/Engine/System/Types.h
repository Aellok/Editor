#pragma once
#include <intrin.h>

#define PI 3.141592f
#define TO_RAD(x) ((float)(x) * 0.01745f)
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))


typedef unsigned char	   u8;
typedef unsigned short	   u16;
typedef unsigned int	   u32;
typedef unsigned long long u64;

typedef char	  s8;
typedef short	  s16;
typedef int		  s32;
typedef long long s64;

typedef float     f32;
typedef double    f64;

u32 builtin_clz(u32 x);