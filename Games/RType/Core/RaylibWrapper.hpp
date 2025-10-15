#pragma once

// This header ensures proper inclusion order for Raylib on Windows
// to avoid conflicts with Windows API headers (especially WinGDI and WinUser)

#ifdef _WIN32
  // Set guards BEFORE including ANY Windows headers
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  // These must stay defined to prevent conflicts throughout the translation unit
  #ifndef NOGDI
    #define NOGDI     // Prevent wingdi.h inclusion (conflicts with raylib Rectangle)
  #endif
  #ifndef NOUSER
    #define NOUSER    // Prevent winuser.h inclusion (conflicts with raylib CloseWindow, ShowCursor, DrawText, LoadImage)
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
#endif

// Now safe to include raylib - Windows headers already included with proper guards
#include <raylib.h>
