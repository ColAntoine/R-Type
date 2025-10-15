#pragma once

// This header ensures proper inclusion order for Raylib on Windows
// to avoid conflicts with Windows API headers (especially WinGDI and WinUser)

#ifdef _WIN32
  // Include Windows socket headers first with minimal Windows API
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #define NOGDI     // Prevent wingdi.h inclusion (conflicts with raylib Rectangle)
  #define NOUSER    // Prevent winuser.h inclusion (conflicts with raylib CloseWindow, ShowCursor)
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #undef NOGDI
  #undef NOUSER
#endif

// Now safe to include raylib
#include <raylib.h>
