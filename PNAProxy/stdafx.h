// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define	_CRT_SECURE_NO_WARNINGS 1

#ifdef UNDER_CE
#	pragma comment(linker, "/nodefaultlib:libc.lib")
#	pragma comment(linker, "/nodefaultlib:libcd.lib")
#	define WINVER _WIN32_WCE
#	include <ceconfig.h>
#	if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#		define SHELL_AYGSHELL
#	endif
#	ifdef _CE_DCOM
#		define _ATL_APARTMENT_THREADED
#	endif

#else	//	UNDER_CE
#	ifndef WINVER				// Allow use of features specific to Windows XP or later.
#		define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#	endif
#endif	//	UNDER_CE



// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#ifdef UNDER_CE

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#ifndef _DEVICE_RESOLUTION_AWARE
#define _DEVICE_RESOLUTION_AWARE
#endif
#endif

#ifdef _DEVICE_RESOLUTION_AWARE
#include "DeviceResolutionAware.h"
#endif

#if _WIN32_WCE < 0x500 && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
	#pragma comment(lib, "ccrtrtti.lib")
	#ifdef _X86_	
		#if defined(_DEBUG)
			#pragma comment(lib, "libcmtx86d.lib")
		#else
			#pragma comment(lib, "libcmtx86.lib")
		#endif
	#endif
#endif

#include <altcecrt.h>

#endif	//	UNDER_CE

// TODO: reference additional headers your program requires here
