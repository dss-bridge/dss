/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#if defined(_WIN32)
  #if defined(__MINGW32__) && !defined(WINVER)
      #define WINVER 0x500 
  #endif
  #include <windows.h>
  #include <process.h>
  
  #define USES_DLLMAIN
      /* DLL uses DllMain() for initialization */

  #if defined (_MSC_VER)
    #include <intrin.h>
  #endif

#elif defined (__CYGWIN__)
  #include <windows.h>
  #include <process.h>

  #define USES_DLLMAIN

#endif

// In C++11 UNUSED(x) is explicitly provided
#if __cplusplus <= 199711L
  #if defined (_MSC_VER)
    #define UNUSED(x) (void) (x);
  #else
    #define UNUSED(x) (void) (sizeof((x), 0))
  #endif
  #if ! defined (nullptr)
    #define nullptr NULL
  #endif
#endif
