#pragma once

// This header ensures proper inclusion order on Windows to prevent API conflicts

#ifdef _WIN32
  // Prevent Windows.h from including problematic headers
  #define NOGDI        // Exclude GDI (Graphics Device Interface) - prevents Rectangle function
  #define NOUSER       // Exclude USER (Window management) - prevents ShowCursor, CloseWindow
  #define NOMINMAX     // Prevent min/max macros
  #define WIN32_LEAN_AND_MEAN  // Reduce Windows header bloat
  
  // Include winsock2.h with restricted Windows headers
  #include <winsock2.h>
  #include <ws2tcpip.h>
  
  // Now undefine NOGDI and NOUSER so raylib can work properly
  #undef NOGDI
  #undef NOUSER
#endif

// Include raylib - it can now define Rectangle, CloseWindow, ShowCursor freely
#include <raylib.h>
