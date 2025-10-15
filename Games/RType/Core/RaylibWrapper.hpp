#pragma once

// This header ensures proper inclusion order on Windows to prevent API conflicts

#ifdef _WIN32
  // Step 1: Define guards BEFORE any Windows headers
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  
  // Step 2: Include winsock2.h FIRST (before windows.h) with guards to prevent conflicts
  #define NOGDI     // Prevents Rectangle conflict with Raylib
  #define NOUSER    // Prevents LoadImage, DrawText, CloseWindow, ShowCursor conflicts with Raylib
  #include <winsock2.h>
  #include <ws2tcpip.h>
  // Keep NOGDI and NOUSER defined so they remain active for raylib inclusion
#endif

// Step 3: Include raylib - on Windows, windows.h is already included via winsock2.h with proper guards
#include <raylib.h>
