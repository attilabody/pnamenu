// wbtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../PNAMenu/txtfile.h"
#include <string>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	wstring	ws;
	bool	succ;
	txtfile	f( L"Z:\\work\\compi\\trunk\\test\\winCE\\wbtest\\Tools.ini" );

	do {
		succ = f.readline( ws );
		OutputDebugString( ws.c_str() );
		OutputDebugString( L"\n" );
	} while( succ );

	return 0;
}

