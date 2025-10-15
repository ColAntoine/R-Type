#pragma once

// Critical: Define these BEFORE any Windows headers to exclude problematic APIs

#ifdef _WIN32
  // Must be defined BEFORE including any Windows header
  #ifndef NOGDI
    #define NOGDI      // Exclude GDI - prevents Rectangle(), DrawText conflicts  
  #endif
  #ifndef NOUSER  
    #define NOUSER     // Exclude USER - prevents ShowCursor(), CloseWindow() conflicts
  #endif
  #ifndef NOMINMAX
    #define NOMINMAX   // Prevent min/max macros
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
#endif

// Now include platform-specific headers
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  // winsock2.h will include windows.h, but wingdi.h and winuser.h are excluded
#endif

// Include raylib - no conflicts because NOGDI/NOUSER prevented problematic declarations
#include <raylib.h>
