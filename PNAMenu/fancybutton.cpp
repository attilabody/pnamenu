// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#include "StdAfx.h"
#include "fancybutton.h"
#include <sstream>
#include <string>
#include <vector>

using namespace std;

fancybutton::fancybutton( HWND hwnd, int dpi, COLORREF *colors, int selthick, int txtmarginx, int txtmarginy, iconfig *cfg ) :
subclasser( hwnd ),
m_dpi( dpi ),
m_bg( colors[0] ),
m_fg( colors[1] ),
m_bgselect( colors[2] ),
m_fgselect( colors[3] ),
m_selthick( selthick ),
m_txtmarginx( txtmarginx ),
m_txtmarginy( txtmarginy ),
m_bgbrush( CreateSolidBrush( m_bg ) ),
m_fgbrush( CreateSolidBrush( m_fg ) ),
m_bgselectbrush( CreateSolidBrush( m_bgselect ) ),
m_fgselectbrush( CreateSolidBrush( m_fgselect ) ),
m_select( false ),
m_clicksound( cfg->getappath() )
{
	m_clicksound.append( L"click.wav" );

	HDC		dc( GetDC( hwnd ) );
	LOGFONT	lf;

	memset( &lf, 0, sizeof(lf) );

	GetClientRect( m_hwnd, &m_txtrect );

	m_txtrect.right -= m_txtmarginx*2;
	m_txtrect.bottom -= m_txtmarginy*2;

	int lpy = GetDeviceCaps(dc, LOGPIXELSY);

	lf.lfHeight = -((LONG)( ((long long)12 * ( m_dpi ? m_dpi: lpy )) / 72) );
	lf.lfWeight = FW_BOLD;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	wcsncpy(lf.lfFaceName, L"MS Shell Dlg", sizeof(lf.lfFaceName) / sizeof( lf.lfFaceName[0] ) );
	m_font = CreateFontIndirect( &lf );

	std::vector< TCHAR >	strbuf( GetWindowTextLength( hwnd ) + 1 );

	GetWindowText( hwnd, &strbuf[0], (int)strbuf.size() );
	m_text = &strbuf[0];

	RECT	oldrect( m_txtrect );
	HFONT	oldfont( (HFONT) SelectObject( dc, m_font ) );
	int		dtres( DrawText( dc, m_text.c_str(), -1, &m_txtrect, DT_CALCRECT | DT_CENTER | DT_WORDBREAK | DT_WORD_ELLIPSIS ));
	if( m_txtrect.bottom - m_txtrect.top  > oldrect.bottom - oldrect.top ) {
		m_txtrect.top = oldrect.top;
		m_txtrect.bottom = oldrect.bottom;
	}
	SelectObject( dc, oldfont );

	ReleaseDC( hwnd, dc );
}

fancybutton::~fancybutton(void)
{
	DeleteObject( m_bgbrush );
	DeleteObject( m_fgbrush );
	DeleteObject( m_bgselectbrush );
	DeleteObject( m_fgselectbrush );
	DeleteObject( m_font );
}

void fancybutton::clearselect()
{
	RECT	cr;
	m_select = false;
	GetClientRect( m_hwnd, &cr );
	InvalidateRect( m_hwnd, &cr, false );
}

LRESULT fancybutton::onpaint()
{
	PAINTSTRUCT	ps;
	HDC			dc;
	RECT		cr, crt, tr;

	dc = BeginPaint( m_hwnd, &ps );
	GetClientRect( m_hwnd, &cr );
	crt = cr;
	FillRect( dc, &crt, m_select? m_fgselectbrush : m_fgbrush );
	int	bw( ( m_select && m_selthick ) ? m_selthick : 1 );
	crt.bottom-=bw;
	crt.top+=bw;
	crt.left+=bw;
	crt.right-=bw;
	HBRUSH	oldbrush( (HBRUSH)SelectObject( dc, (m_select ? m_bgselectbrush : m_bgbrush) ) );
	RoundRect( dc, crt.left, crt.top, crt.right, crt.bottom, bw*10, bw*10 );
	SelectObject( dc, oldbrush );

	tr = m_txtrect;
	int xoffset = ((cr.right - cr.left) - (tr.right - tr.left )) / 2;
	int yoffset = ((cr.bottom - cr.top) - ( tr.bottom -tr.top )) / 2;
	tr.left += xoffset;
	tr.right += xoffset;
	tr.top +=yoffset;
	tr.bottom +=yoffset;

	HFONT	oldfont( (HFONT) SelectObject( dc, m_font ) );
	SetTextColor( dc, m_select? m_fgselect : m_fg );
	SetBkMode( dc, TRANSPARENT );
	int dtres( DrawText( dc, m_text.c_str(), -1, &tr, DT_CENTER | DT_WORDBREAK | DT_WORD_ELLIPSIS | DT_END_ELLIPSIS ) );
	SelectObject( dc, oldfont );
	
	EndPaint( m_hwnd, &ps );
	return 0;
}

LRESULT fancybutton::onlbuttondown()
{
	m_select = true;
	InvalidateRect( m_hwnd, NULL, false );
	onpaint();
	PlaySound( m_clicksound.c_str(), NULL, SND_FILENAME | SND_ASYNC );
	return prevproc();
}

LRESULT fancybutton::onsettext( LPCTSTR text )
{
	m_text = text;
	HDC		dc( GetDC( m_hwnd ) );
	HFONT	oldfont( (HFONT) SelectObject( dc, m_font ) );
	int		dtres = DrawText( dc, m_text.c_str(), -1, &m_txtrect, DT_CALCRECT | DT_LEFT | DT_TOP);
	SelectObject( dc, oldfont );
	ReleaseDC( m_hwnd, dc );
	return prevproc();
}
