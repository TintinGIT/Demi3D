
/********************************************************************
    File:     EngineDefines.h
*********************************************************************/

#pragma once

#ifdef DEMI_STATIC_API
#    define DI_GFX_API
#else
#    ifdef DI_GFX_EXPORT
#        define DI_GFX_API  __declspec(dllexport)
#    else
#        define DI_GFX_API __declspec(dllimport)
#    endif
#endif

#ifndef MAKEFOURCC
#   define MAKEFOURCC(ch0, ch1, ch2, ch3)    ((unsigned long)(uint8)(ch0) | ((unsigned long)(uint8)(ch1) << 8) \
    | ((unsigned long)(uint8)(ch2) << 16) | ((unsigned long)(uint8)(ch3) << 24 ))
#endif

#ifndef MAKEFOURCCSTR
#   define MAKEFOURCCSTR(str) MAKEFOURCC(str[0],str[1],str[2],str[3])
#endif

typedef void*   DiWndHandle;

#if DEMI_PLATFORM == DEMI_PLATFORM_WIN32
#   ifdef max
#       undef max
#   endif

#   ifdef min
#       undef min
#   endif
#endif
