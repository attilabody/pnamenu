// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#pragma once
#include <string>
#include "fancybutton.h"

struct launcher
{
	launcher(
		bool submenu,
		const std::wstring command,
		const std::wstring cmdline,
		const std::wstring cmdparams,
		const std::wstring comment,
		COLORREF *colors = NULL
	) :
		m_submenu( submenu ),
		m_command( command ),
		m_cmdline( cmdline ),
		m_cmdparams( cmdparams ),
		m_comment( comment ),
		m_button( NULL )
	{
		if( colors ) memcpy( m_colors, colors, sizeof( m_colors ));
	}

	std::wstring	m_command;
	std::wstring	m_cmdline;
	std::wstring	m_cmdparams;
	std::wstring	m_comment;
	fancybutton		*m_button;
	COLORREF		m_colors[4];
	bool			m_submenu;
};
