// Copyright (c) 2008 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3

#include "stdafx.h"


#define countof(a) (sizeof(a)/sizeof(a[0]))

//////////////////////////////////////////////////////////////////////////////
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	wchar_t	rawbuf[ MAX_PATH+1 ];
	char	bytebuf[ MAX_PATH+1 ];
	HANDLE	fh;
	BOOL	succ;
	DWORD	readcnt;
	DWORD	numchars;
	wchar_t	*lastsep;
	
	numchars  = GetModuleFileName( NULL, rawbuf, countof( rawbuf ) );
	lastsep = wcsrchr( rawbuf, L'\\' );
	if( !lastsep ) {
		return 0;
	}
	wcscpy( lastsep, L"\\PNALaunch.ini" );

	fh = CreateFile( rawbuf, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( fh == INVALID_HANDLE_VALUE ) {
		return 0;
	}
	succ = ReadFile( fh, bytebuf, sizeof( bytebuf ), &readcnt, NULL );
	if( !succ || !readcnt || readcnt >= sizeof( bytebuf ) ) {
		return 0;
	}
	bytebuf[readcnt] = 0;
	mbstowcs( rawbuf, bytebuf, readcnt+1 );

	{
		PROCESS_INFORMATION	pi;
#ifndef	UNDER_CE
		STARTUPINFO	si;
		memset( &si, 0, sizeof( si ) );
		si.cb = sizeof( si );
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;

		succ = CreateProcess( rawbuf, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) != FALSE;
#else	//	UNDER_CE
		succ = CreateProcess( rawbuf, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi ) != FALSE;
		if( !succ ) {
			CreateProcess( L"\\Windows\\Explorer.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi );
		}
#endif	//	UNDER_CE
	}
	return 0;
}

