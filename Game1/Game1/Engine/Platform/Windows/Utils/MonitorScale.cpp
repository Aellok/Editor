#include "MonitorScale.h"
#include "Windows.h"

Vector GetMonitorScale()
{
    HWND activeWindow = GetActiveWindow();
    HMONITOR monitor = MonitorFromWindow(activeWindow, MONITOR_DEFAULTTONEAREST);

    // Get the logical width and height of the monitor
    MONITORINFOEX monitorInfoEx;
    monitorInfoEx.cbSize = sizeof(monitorInfoEx);
    GetMonitorInfo(monitor, &monitorInfoEx);
    auto cxLogical = monitorInfoEx.rcMonitor.right - monitorInfoEx.rcMonitor.left;
    auto cyLogical = monitorInfoEx.rcMonitor.bottom - monitorInfoEx.rcMonitor.top;

    // Get the physical width and height of the monitor
    DEVMODE devMode;
    devMode.dmSize = sizeof(devMode);
    devMode.dmDriverExtra = 0;
    EnumDisplaySettings(monitorInfoEx.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    DWORD cxPhysical = devMode.dmPelsWidth;
    DWORD cyPhysical = devMode.dmPelsHeight;

    // Calculate the scaling factor
    float horizontalScale = ((float)cxPhysical / (float)cxLogical);
    float verticalScale = ((float)cyPhysical / (float)cyLogical);
    return { horizontalScale,verticalScale };
}