#pragma once

// This header ensures proper inclusion order on Windows to prevent API conflicts

#ifdef _WIN32
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  
  // Include winsock2.h first to get networking functions
  #include <winsock2.h>
  #include <ws2tcpip.h>
  
  // Undefine conflicting Windows macros that will clash with Raylib
  // These Windows GDI/User macros redirect to A/W versions
  #ifdef Rectangle
    #undef Rectangle
  #endif
  #ifdef CloseWindow  
    #undef CloseWindow
  #endif
  #ifdef ShowCursor
    #undef ShowCursor
  #endif
  #ifdef DrawText
    #undef DrawText
  #endif
  #ifdef DrawTextEx
    #undef DrawTextEx
  #endif
  #ifdef LoadImage
    #undef LoadImage
  #endif
#endif

// Now include raylib - conflicts are resolved
#include <raylib.h>
