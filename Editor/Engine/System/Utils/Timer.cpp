#include "Timer.h"

void Timer::Start()
{
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    Frequency = f64(li.QuadPart);
    QueryPerformanceCounter(&StartTime);
}
f32 Timer::Stop()
{
    QueryPerformanceCounter(&EndTime);
    LastTime = (f32)(f64(EndTime.QuadPart - StartTime.QuadPart) / Frequency);
    return LastTime;
}