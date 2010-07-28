// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#pragma once
#include "subclasser.h"
#include "iconfig.h"

class fancybutton : public subclasser
{
public:
	fancybutton( HWND hwnd, int dpi, COLORREF *colors, int selthick, int txtmarginx, int txtmarginy, iconfig *cfg );
	virtual ~fancybutton(void);
	void			clearselect();

	virtual LRESULT	onpaint();
	virtual LRESULT onsettext( LPCTSTR text );
	virtual LRESULT onlbuttondown();
/*	virtual LRESULT onmousemove( UINT_PTR flags, LONG x, LONG y );
	virtual LRESULT onerasebkgnd( HDC dc );
*/
protected:

	HFONT						m_font;
	RECT						m_txtrect;
	std::basic_string< TCHAR >	m_text;
	std::basic_string< TCHAR >	m_clicksound;
	int							m_dpi;
	COLORREF					m_bg;
	COLORREF					m_fg;
	COLORREF					m_bgselect;
	COLORREF					m_fgselect;
	int							m_selthick;
	int							m_txtmarginx, m_txtmarginy;
	HBRUSH						m_bgbrush;
	HBRUSH						m_fgbrush;
	HBRUSH						m_bgselectbrush;
	HBRUSH						m_fgselectbrush;
	bool						m_select;
};
