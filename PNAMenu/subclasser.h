// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#pragma once

class subclasser
{
public:
	subclasser( HWND hwnd );
	virtual ~subclasser(void);

	static LRESULT CALLBACK	_wndproc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
	virtual LRESULT			wndproc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );

	virtual LRESULT	onpaint() { return prevproc(); }
	virtual LRESULT onsettext( LPCTSTR text ) { return prevproc(); }
	virtual LRESULT onlbuttondown() { return prevproc(); }
	virtual LRESULT onmousemove( UINT_PTR flags, LONG x, LONG y ) { return prevproc(); }
	virtual LRESULT onmouseleave() { return prevproc(); }
	virtual LRESULT onerasebkgnd( HDC dc ) { return prevproc(); }

	inline HWND gethandle() { return m_hwnd; }

protected:
	LRESULT prevproc() { return CallWindowProc( m_prevproc, m_hwndrcvd, m_msgrcvd, m_wparamrcvd, m_lparamrcvd ); }

	WNDPROC	m_prevproc;
	HWND	m_hwnd;
	HWND	m_hwndrcvd;
	UINT	m_msgrcvd;
	LPARAM	m_lparamrcvd;
	WPARAM	m_wparamrcvd;
};
