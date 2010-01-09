// PNAProxy.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PNAProxy.h"
#include <windows.h>
#include <commctrl.h>

#define MAX_LOADSTRING 100
//#define	_DEBUG_PNAPROXY

// Global Variables:
HINSTANCE			g_hInst;			// current instance
HWND				g_hWndCommandBar;	// command bar handle

// Forward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE, LPTSTR);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
#if defined( _DEBUG ) && defined( _DEBUG_PNAPROXY )
	//give a chance for the debugger to attach while in the loop
//	DWORD	ast( GetTickCount() );
//	DWORD	st;
	//this loop will exit only if we spent some serious amount
	//of time in the loop (breakpoint)
	//if it does not happen in a minute, continue
//	do {
//		st = GetTickCount();
//		Sleep( 100 );
//	} while( GetTickCount() - st < 1000 && GetTickCount() - ast < 60000 );

	MessageBox( NULL, L"Attach to the process with a debugger now!", L"Debug wait", MB_OK );

#endif
	LPTSTR	p( lpCmdLine );

#ifdef UNDER_CE
	bool	prot( false );
	TCHAR	pc(0);
	while( *p ) {
		if( !prot ) {
			if( iswspace( *p ) ) {
				break;
			}
			if( *p == _T('"') || *p == _T('\'') ) {
				pc = *p;
				prot = true;
			}
		} else {
			if( *p == pc ) {
				pc=0;
				prot=false;
			}
		}
		++p;
	}
	while( *p && iswspace( *p ) ) {
		++p;
	}
#endif

	LPTSTR	caller( p );

	while( *p && *p != _T('|') ) {
		++p;
	}
	if( *p ) {
		*p = 0;
	} else {
		return -1;
	}

	++p;
	LPTSTR	cmd( p );
	bool	have_cmdline( false );
	while( *p && *p != _T('|') ) {
		++p;
	}
	if( *p ) {
		*p = 0;
		have_cmdline = true;
	}

	LPTSTR	cmdline( have_cmdline ? p+1 : NULL );

	bool				succ;
	PROCESS_INFORMATION	pi;
#ifndef	UNDER_CE
	STARTUPINFO	si;
	memset( &si, 0, sizeof( si ) );
	si.cb = sizeof( si );
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	succ = CreateProcess( cmd, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) != FALSE;
#else	//	UNDER_CE
	succ = CreateProcess( cmd, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi ) != FALSE;
#endif	//	UNDER_CE
	if( succ ) {
		WaitForSingleObject( pi.hProcess, INFINITE );
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
#ifndef	UNDER_CE
	memset( &si, 0, sizeof( si ) );
	si.cb = sizeof( si );
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	succ = CreateProcess( caller, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) != FALSE;
#else	//	UNDER_CE
	succ = CreateProcess( caller, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi ) != FALSE;
#endif	//	UNDER_CE
	if( succ ) {
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}

	return 0;
}
