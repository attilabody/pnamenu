// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#include "StdAfx.h"
#include "subclasser.h"

//////////////////////////////////////////////////////////////////////////////
subclasser::subclasser( HWND hwnd ) :
m_hwnd( hwnd )
{
	SetWindowLong( m_hwnd, GWL_USERDATA, (LONG_PTR) this );
	m_prevproc = (WNDPROC) SetWindowLong( m_hwnd, GWL_WNDPROC, (LONG_PTR)_wndproc );

	HDC		dc( GetDC( hwnd ) );
	LOGFONT	lf;

	memset( &lf, 0, sizeof(lf) );
}

//////////////////////////////////////////////////////////////////////////////
subclasser::~subclasser(void)
{
	SetWindowLong( m_hwnd, GWL_WNDPROC, (LONG_PTR) m_prevproc );
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK subclasser::_wndproc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
	subclasser *_this_ = (subclasser*) GetWindowLong( hwnd, GWL_USERDATA );
	return _this_->wndproc( hwnd, message, wparam, lparam );
}

//////////////////////////////////////////////////////////////////////////////
LRESULT subclasser::wndproc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
	m_hwndrcvd = hwnd;
	m_msgrcvd = message;
	m_wparamrcvd = wparam;
	m_lparamrcvd = lparam;

	switch( message )
	{
	case WM_PAINT:
		return onpaint();

	case WM_SETTEXT:
		return onsettext( (LPCTSTR) lparam );

	case WM_LBUTTONDOWN:
		return onlbuttondown();

	case WM_MOUSEMOVE:
		return onmousemove( wparam, LOWORD( lparam ), HIWORD( lparam ) );

	case WM_ERASEBKGND:
		return onerasebkgnd( (HDC) wparam );

	};

	return prevproc();
}

