#pragma once
#include "System/Types.h"
#include <windows.h>
struct Timer
{
	f64 Frequency;
	LARGE_INTEGER StartTime;
	LARGE_INTEGER EndTime;
	f32 LastTime;
	void Start();
	f32 Stop();
};