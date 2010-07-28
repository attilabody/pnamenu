// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3

#include "stdafx.h"
#include "pnamenuapp.h"
#include <Tlhelp32.h>

using namespace std;
//#define	_PNAGUI_DEBUG_START_DELAY

string		g_ourname;
pnamenuapp	*g_app( NULL );

//////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return g_app->wndproc( hWnd, message, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////////
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
#if defined( _DEBUG ) && defined( UNDER_CE ) && defined( _PNAGUI_DEBUG_START_DELAY )
//give a chance for the debugger to attach while in the loop
	DWORD	ast( GetTickCount() );
	DWORD	st;
//this loop will exit only if we spent more than 600ms in the loop (breakpoint)
//or timeout occurs (60 seconds)
	do {
		st = GetTickCount();
		Sleep( 100 );
	} while( GetTickCount() - st < 600 && GetTickCount() - ast < 60000 );
//	MessageBox( NULL, L"Attach to the process with a debugger now!", L"Debug wait", MB_OK );
#endif

	g_app = new pnamenuapp( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
	g_app->registerwindowclass( WndProc );
	g_app->initinstance( nCmdShow );

	int retval( (int)g_app->messageloop() );
	delete g_app;
	return retval;

	delete g_app;
	return 0;
}

