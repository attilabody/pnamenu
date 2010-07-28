// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#pragma once
#include <string>
#include <map>
#include <vector>

#include "launcher.h"
#include "fancybutton.h"
#include "iconfig.h"

class pnamenuapp : public iconfig
{
public:
	typedef std::map< std::wstring, HMODULE >	dllookup_t;
	typedef dllookup_t::iterator				dllookupiter_t;
	typedef std::vector< std::wstring >			wsvect_t;
	typedef wsvect_t::iterator					wsvectit_t;

	pnamenuapp( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
	virtual ~pnamenuapp(void);

	virtual bool	registerwindowclass( WNDPROC wndproc );
	virtual bool	initinstance( int nCmdShow );
	WPARAM	messageloop();
	virtual LRESULT	wndproc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	virtual const std::wstring&	getappath();

protected:
	HINSTANCE		m_instance;
	HWND			m_hwnd;
	int				m_xres, m_yres;
	HACCEL			m_accel;
	std::wstring	m_wndcname, m_wndtitle;
	std::wstring	m_appath;
	std::wstring	m_exittext, m_exitcmd;
	std::wstring	m_backtext;
	int				m_hmargin, m_vmargin;
	COLORREF		m_colors[4], m_tempcolors[4], m_exitcolors[4];
	int				m_dpi;

	wsvect_t		m_prevmenus;
	std::wstring	m_currmenu;

	std::vector< launcher >		m_launchers;
	fancybutton					*m_exitbutton;
	wsvect_t					m_register;

//	std::vector< fancybutton* >	m_buttons;
//	std::vector< std::wstring >	m_launchables;
//	std::vector< std::wstring > m_comments;
//	std::vector< std::wstring >	m_launchables_lc;

	dllookup_t	m_preloaded;
	wsvect_t	m_autorunhist;

	virtual	LRESULT	on_create( HWND hwnd, CREATESTRUCT *cs );
	virtual LRESULT on_paint( HWND hwnd );
	virtual LRESULT on_destroy( HWND hwnd );
	virtual LRESULT on_command( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
	virtual LRESULT on_size( HWND hwnd, WPARAM wparam, LPARAM lparam );

	void createbuttons( HWND hwnd );
	void updatebuttons( HWND hwnd );
	void destroybuttons();
	bool isprocessrunning( const wchar_t *procname );

	void loaddlls( const wchar_t *basedir );
	bool processline( const std::wstring &cmdline, const std::wstring &comment );

	void	savestate();
	void	loadstate();
	void	writerms( HKEY kh, wchar_t *name,  wsvect_t &data );

	enum internalcommands {
		unknown = 0,
		submenu,
		exittext,
		exitcmd,
		backtext,
		color,
		tempcolor,
		exitcolor,
		preload,
		dpi,
		autorun,
	};

};
