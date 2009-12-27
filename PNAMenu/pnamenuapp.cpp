// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#include "StdAfx.h"
#include "resource.h"
#include "pnamenuapp.h"
#include "txtfile.h"

#include <vector>
#include <cctype>
#ifndef UNDER_CE
#pragma comment( lib, "winmm" )
#else
#include "Tlhelp32.h"
#pragma comment( lib, "ToolHelp" )
#endif
using namespace std;


//////////////////////////////////////////////////////////////////////////////
const wstring& pnamenuapp::getappath()
{
	return m_appath;
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::towide( wstring &dst, const string &src )
{
	int rbs( MultiByteToWideChar( CP_UTF8, 0, src.c_str(), -1, NULL, 0 ));
	if( rbs < 0 ) {
		return;
	}
	vector<wchar_t>	buf( rbs );
	rbs = MultiByteToWideChar( CP_UTF8, 0, src.c_str(), -1, &buf[0], (int)buf.size() );
	if( rbs >=0 ) {
		dst = &buf[0];
	}
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::tombcs( string &dst, const wstring &src )
{
	vector<char>	buf( ( src.size() + 1 ) * 2 );
	wcstombs( &buf[0], src.c_str(), buf.size() );
	dst = &buf[0];
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::tombcs( string &dst, const wchar_t *src )
{
	vector<char>	buf( ( wcslen( src ) + 1 ) * 2 );
	wcstombs( &buf[0], src, buf.size() );
	dst = &buf[0];
}

//////////////////////////////////////////////////////////////////////////////
inline void rtrim( string &s )
{
	string::size_type	pos( s.find_last_not_of( " \t" ) );
	if( pos != string::npos ) {
		s.resize( pos+1 );
	} else {
		s.clear();
	}
}

//////////////////////////////////////////////////////////////////////////////
inline void rtrim( wstring &s )
{
	wstring::size_type	pos( s.find_last_not_of( L" \t" ) );
	if( pos != wstring::npos ) {
		s.resize( pos+1 );
	} else {
		s.clear();
	}
}

//////////////////////////////////////////////////////////////////////////////
inline void ltrim( string &s )
{
	string::size_type	pos( s.find_first_not_of( " \t" ) );
	if( pos == s.npos ) {
		s.clear();
	} else if( pos ) {
		s.erase( 0, pos );
	}
}

//////////////////////////////////////////////////////////////////////////////
inline void ltrim( wstring &s )
{
	wstring::size_type	pos( s.find_first_not_of( L" \t" ) );
	if( pos == s.npos ) {
		s.clear();
	} else if( pos ) {
		s.erase( 0, pos );
	}
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::loaddlls( const wchar_t *name )
{
	DWORD						error( NO_ERROR );
	WIN32_FIND_DATA				fd;
	HANDLE						fh;
	std::vector< std::wstring>	dllnames;
	bool						dirload( false );
	std::wstring				target( name );

	DWORD	attr( GetFileAttributes( name ) );
	if( attr == INVALID_FILE_ATTRIBUTES ) {
		return;
	}

	if( attr & FILE_ATTRIBUTE_DIRECTORY ) {
		dirload = true;
	}

	if( dirload && *target.rbegin() != L'\\' ) {
		target.push_back( L'\\' );
	}

	std::wstring	findpattern( target );

	findpattern.append( L"*.dll" );
	transform( target.begin(), target.end(), target.begin(), static_cast<int (*)(int)>(std::tolower) );

	if( dirload ) {
		fh = FindFirstFile( findpattern.c_str(), &fd );
		if( fh == INVALID_HANDLE_VALUE ) {
			return;
		}

		do {
			if( !(fd.dwFileAttributes & ( FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_OFFLINE ) ) ) {
				std::wstring	dllname( target );
				dllname.append( fd.cFileName );
				dllnames.push_back( dllname );
			}
			if( !FindNextFile( fh, &fd ) ) {
				error = GetLastError();
			}
		} while( !error );
		FindClose( fh );
	} else {
		dllnames.push_back( target );
	}

	bool loaded;
	do {
		loaded = false;
		std::vector< std::wstring >::iterator	it( dllnames.begin() ), end( dllnames.end() );
		while( it != end ) {
			if( it->size() ) {
				dllookupiter_t	lit( m_preloaded.find( *it ) );
				if( lit == m_preloaded.end() ) {
					HMODULE	dh( LoadLibrary( it->c_str() ) );
					if( dh ) {
						m_preloaded[ *it ] = dh;
						it->clear();
						loaded = true;
					}
				}
			}
			++it;
		}
	} while( loaded );
}
//////////////////////////////////////////////////////////////////////////////
pnamenuapp::pnamenuapp( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
: m_instance( hInstance )
, m_exitbutton( NULL )
, m_exittext( L"EXIT" )
, m_backtext( L"BACK" )
, m_dpi( 0 )
{
	vector< wchar_t >	stringbuf(256);
	m_colors[0] = RGB(5,0x2a,0x41);
	m_colors[1] = RGB(0xc8,0xc8,0xc8);
	m_colors[2] = RGB(0x0a,0x50,0x80);
	m_colors[3] = RGB(0xff,0xff,0xff);

	memcpy( m_tempcolors, m_colors, sizeof( m_tempcolors ) );
	memcpy( m_exitcolors, m_colors, sizeof( m_exitcolors ) );

	LoadString( m_instance, IDC_PNAMENU, &stringbuf[0], (int)stringbuf.size() );
	m_wndcname = &stringbuf[0];
	LoadString( m_instance, IDS_APP_TITLE, &stringbuf[0], (int)stringbuf.size() );
}

//////////////////////////////////////////////////////////////////////////////
pnamenuapp::~pnamenuapp(void)
{
	dllookupiter_t	it( m_preloaded.begin() ), end( m_preloaded.end() );

	while( it != end ) {
		FreeLibrary( (it++)->second );
	}
}

//////////////////////////////////////////////////////////////////////////////
bool pnamenuapp::registerwindowclass( WNDPROC wndproc )
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = wndproc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_instance;
	wc.hIcon         = LoadIcon( m_instance, MAKEINTRESOURCE(IDI_PNAMENU));
#ifdef UNDER_CE
	wc.hCursor       = 0;
#else
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
#endif
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = m_wndcname.c_str();

	return RegisterClass(&wc) != 0;
}

//////////////////////////////////////////////////////////////////////////////
bool pnamenuapp::initinstance( int ncmdshow )
{
#ifdef UNDER_CE
	m_hwnd = CreateWindow(
		m_wndcname.c_str(), m_wndtitle.c_str(),
		WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, m_instance, NULL );
#else
	m_hwnd = CreateWindow(
		m_wndcname.c_str(), NULL,
		WS_VISIBLE|WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 480, 272,
		NULL, NULL, m_instance, NULL );
#endif
	if( m_hwnd ) {
		m_accel = LoadAccelerators( m_instance, MAKEINTRESOURCE(IDC_PNAMENU));
		return true;
	}

	ShowWindow( m_hwnd, ncmdshow );
	UpdateWindow( m_hwnd );
	return false;
}

//////////////////////////////////////////////////////////////////////////////
WPARAM	pnamenuapp::messageloop()
{
	MSG	msg;

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, m_accel, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT pnamenuapp::wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_COMMAND:
		return on_command( hWnd, message, wParam, lParam );
		break;
	case WM_CREATE:
		return on_create( hWnd, (CREATESTRUCT*)lParam );
		break;
	case WM_PAINT:
		return on_paint( hWnd );
		break;
	case WM_SIZE:
		return on_size( hWnd, wParam, lParam );
		break;
	case WM_DESTROY:
		return on_destroy( hWnd );
		break;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT pnamenuapp::on_create(HWND hwnd, CREATESTRUCT *cs)
{
	wchar_t	rawbuf[ MAX_PATH+1 ];
	DWORD	numchars( GetModuleFileName( NULL, rawbuf, sizeof( rawbuf )/sizeof( rawbuf[0] ) ) );

	wstring ourname( rawbuf) ;
	wstring	lname( rawbuf );
	transform( lname.begin(), lname.end(), lname.begin(), static_cast<int (*)(int)>(std::tolower) );
	m_register.push_back( lname );

	wchar_t	*lastsep( wcsrchr( rawbuf, L'\\' ));
	if( lastsep ) {
		*++lastsep = 0;
	}
	m_appath = rawbuf;
	m_currmenu = rawbuf;
	m_currmenu.append( L"PNAMenu.ini" );

	wstring	comment;
	processline( m_currmenu, comment );

	if( m_launchers.size() ) {
		m_hmargin = 10;
		m_vmargin = 3;
		createbuttons( hwnd );
	} else {
		PostMessage( hwnd, WM_CLOSE, 0, 0 );
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT pnamenuapp::on_paint( HWND hwnd )
{
	PAINTSTRUCT ps;
	HDC			dc;

	dc = BeginPaint(hwnd, &ps);

	// TODO: Add any drawing code here...

	EndPaint(hwnd, &ps);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT pnamenuapp::on_destroy( HWND hwnd )
{
	PostQuitMessage(0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::updatebuttons( HWND hwnd )
{
}

//////////////////////////////////////////////////////////////////////////////
LRESULT pnamenuapp::on_command(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	int		id( LOWORD( wparam )), evnt( HIWORD( wparam ));
	bool	succ(false);

	if( evnt == BN_CLICKED )
	{
/*		if( id >= 500 && (size_t)id < 500+m_buttons.size() )
		{
			wstring	sndname( m_appath );
			sndname.append( L"button.wav" );
			PlaySound( sndname.c_str(), NULL, SND_FILENAME | SND_SYNC );
		}
*/
		if( id >= 500 && (size_t)id < 500+m_launchers.size() )
		{
			launcher lncr( m_launchers[id-500] );

			if( lncr.m_submenu ) {
				destroybuttons();
				m_launchers.clear();
				m_register.clear();
				processline( lncr.m_command, lncr.m_comment );
				if( m_launchers.size() ) {
					m_prevmenus.push_back( m_currmenu );
					createbuttons( m_hwnd );
				} else {
					wstring	comment;
					MessageBox( m_hwnd, L"No buttons can be crated from the ini file.", NULL, MB_OK );
					processline( m_currmenu, comment );
					createbuttons( m_hwnd );
				}
			} else {
				bool	useproxy( false );
				wstring	proxycmdline, proxycmd;
				if( lncr.m_command[0] != L'*' && lncr.m_command[0] != L'<' && lncr.m_command[0] != L'>' )
				{
					wchar_t		rawbuf[ MAX_PATH+1 ];
					DWORD		numchars( GetModuleFileName( NULL, rawbuf, sizeof( rawbuf )/sizeof( rawbuf[0] ) ) );
					useproxy = true;
					proxycmdline = m_appath;

					proxycmdline.append(L"PNAProxy.exe");

					proxycmd = proxycmdline;

					proxycmdline.push_back( L' ' );
					proxycmdline.append( rawbuf );
					proxycmdline.push_back( L'|');
					proxycmdline.append( lncr.m_command );
					if( lncr.m_cmdparams.size() ) {
						proxycmdline.push_back( L'|');
						proxycmdline.append( lncr.m_cmdline );
					}
				} else {
					proxycmd = lncr.m_command.c_str() + 1;
					proxycmdline = lncr.m_cmdline.c_str() + 1;
				}

				vector< wchar_t >	buf( proxycmdline.size()+1 );
				buf[buf.size()-1] = 0;
				copy( proxycmdline.begin(), proxycmdline.end(), buf.begin() );

				PROCESS_INFORMATION	pi;
#ifndef	UNDER_CE
				STARTUPINFO	si;
				memset( &si, 0, sizeof( si ) );
				si.cb = sizeof( si );
				si.dwFlags = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_SHOW;

				succ = CreateProcess( proxycmd.c_str(), (useproxy || lncr.m_cmdparams.size() ) ? &buf[0] : NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) != FALSE;
#else	//	UNDER_CE
				succ = CreateProcess(
					proxycmd.c_str(),
					(useproxy || lncr.m_cmdparams.size()) ? &buf[0] : NULL,
					NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi
				) != FALSE;
#endif	//	UNDER_CE
				if( succ && lncr.m_command[0] != L'<' ) {
					destroybuttons();
					PostMessage( hwnd, WM_CLOSE, 0, 0 );
				} else {
					lncr.m_button->clearselect();
				}
			}
		} else if( id == 500 + m_launchers.size() ) {	//EXIT/BACK
			BOOL	succ( TRUE );
			if( m_prevmenus.size() ) {	//	BACK
				wstring	comment;
				destroybuttons();
				m_launchers.clear();
				m_register.clear();
				if( processline( m_prevmenus.back(), comment ) ) {
					m_currmenu = m_prevmenus.back();
					m_prevmenus.pop_back();
					createbuttons( m_hwnd );
				} else {
					PostMessage( hwnd, WM_CLOSE, 0, 0 );
				}
			} else {	//	if( m_prevmenus.size() )	EXIT
				if( m_exitcmd.size() )
				{
					wstring	cmd, params;
					PROCESS_INFORMATION	pi;

					getcmd( cmd, params, m_exitcmd );

					vector< wchar_t >	buf( m_exitcmd.size()+1 );
					buf[buf.size()-1] = 0;
					copy( m_exitcmd.begin(), m_exitcmd.end(), buf.begin() );

	#ifndef	UNDER_CE
					STARTUPINFO	si;
					memset( &si, 0, sizeof( si ) );
					si.cb = sizeof( si );
					si.dwFlags = STARTF_USESHOWWINDOW;
					si.wShowWindow = SW_SHOW;

					succ = CreateProcess( cmd.c_str(), &buf[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
	#else	//	UNDER_CE
					succ = CreateProcess( cmd.c_str(), &buf[0], NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi );
	#endif	//	UNDER_CE
					if( succ ) {
//						DWORD	wr( WaitForSingleObject( pi.hProcess, 10000 ) );
//						if( wr == WAIT_TIMEOUT ) {
							destroybuttons();
							PostMessage( hwnd, WM_CLOSE, 0, 0 );
//						} else {
//							m_exitbutton->clearselect();
//						}
					} else {
						m_exitbutton->clearselect();
					}
				} else {	//	if( m_exitcmd.size() )
					destroybuttons();
					PostMessage( hwnd, WM_CLOSE, 0, 0 );
				}
			}	//else of: if( m_prevmenus.size() )
		}	//else if( id == 500 + m_launchers.size() )
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT pnamenuapp::on_size( HWND hwnd, WPARAM wparam, LPARAM lparam )
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
bool pnamenuapp::getparams( vector<wstring> &params, const wstring &cmdline )
{
	wchar_t	prot(0), pch(0);
	wstring	tmp;
	const wchar_t	*cch;

	for( cch = cmdline.c_str(); *cch; ++cch ) {
		if( !prot ) {
			if( *cch == L'"' || *cch == L'\'' ) {
				prot = *cch;
			} else if( iswspace( *cch ) ) {
				break;
			}
		} else if( *cch == prot ) {
			prot = 0;
		}
	}

	for( ; *cch && iswspace( *cch ); cch++ );

	prot = 0;
	pch = 0;

	for( ; *cch; ++cch ) {
		if( !prot ) {
			if( *cch == L'"' || *cch == L'\'' ) {
				prot = *cch;
				tmp.push_back( *cch );
			} else if( !iswspace( *cch ) ) {
				tmp.push_back( *cch );
			} else if( !iswspace( pch ) ) {
				params.push_back( tmp );
				tmp.clear();
				pch = 0;
			}
		} else if( *cch == prot ) {
			prot = 0;
			tmp.push_back( *cch );
		} else {
			tmp.push_back( *cch );
		}

		pch = *cch;
	}

	if( tmp.size() ) {
		params.push_back( tmp );
	}

	return params.size() != 0;
}
//////////////////////////////////////////////////////////////////////////////
bool pnamenuapp::getcmd( std::wstring &cmd, wstring &params, const std::wstring &cmdline )
{
	wchar_t			prot(0), pch(0);
	const wchar_t	*cch;

	for( cch = cmdline.c_str(); *cch; ++cch ) {
		if( !prot ) {
			if( *cch == L'"' || *cch == L'\'' ) {
				prot = *cch;
			} else if( !iswspace( *cch ) ) {
				cmd.push_back( *cch );
			} else {
				break;
			}
		} else {
			if( *cch == prot ) {
				prot = 0;
			} else {
				cmd.push_back( *cch );
			}
		}
	}
	for( ; iswspace( *cch ); cch++ );
	params = cch;

	return cmd.size() != 0;
}
//////////////////////////////////////////////////////////////////////////////
bool pnamenuapp::processline( const wstring &cmdline, const std::wstring &comment )
{
	internalcommands	cmd( unknown );
	COLORREF			*actcolors( NULL );
	wstring				name, paramstr;

	if( !getcmd( name, paramstr, cmdline ) ) {
		return false;
	}

	wstring	lcmd( cmdline );
	wstring lname( name );
	transform( lcmd.begin(), lcmd.end(), lcmd.begin(), static_cast<int (*)(int)>(std::tolower) );
	transform( lname.begin(), lname.end(), lname.begin(), static_cast<int (*)(int)>(std::tolower) );
#ifdef _DEBUG
	{
		OutputDebugString( L"Processing command ");
		OutputDebugString( cmdline.c_str() );
		OutputDebugString( L"\n" );
	}
#endif	//	_DEBUG
	if( lname == L"color" ) {
		cmd = color;
		actcolors = m_colors;
	} else if( lname == L"tempcolor" ) {
		cmd = tempcolor;
		actcolors = m_tempcolors;
	} else if( lname == L"exitcolor" ) {
		cmd = exitcolor;
		actcolors = m_exitcolors;
	} else if( lname == L"submenu" ) {
		cmd = submenu;
	} else if( lname == L"exittext" ) {
		cmd = exittext;
	} else if( lname == L"exitcmd" ) {
		cmd = exitcmd;
	} else if( lname == L"backtext" ) {
		cmd = backtext;
	} else if( lname == L"preload") {
		cmd = preload;
	} else if( lname == L"dpi") {
		cmd = dpi;
	} else if( lname == L"autorun" ) {
		cmd = autorun;
	}

	if( cmd == color || cmd == tempcolor || cmd == exitcolor )
	{
		//color bglo fglo bghi fghi
		COLORREF	tmpcolors[4];

		vector< wstring >	params;
		if( !getparams( params, cmdline ) ) {
			return false;
		}
		if( params.size() != 4 ) {
			return false;
		}

		for( int param = 0; param < 4; param++ ) {
			transform( params[param].begin(), params[param].end(), params[param].begin(), static_cast<int (*)(int)>(std::tolower) );
			if( !swscanf( params[param].c_str(), L"%x", &tmpcolors[param] ) ) {
				return false;
			}
		}
		memcpy( actcolors, tmpcolors, sizeof( tmpcolors ) );
		if( cmd == color ) {
			memcpy( m_tempcolors, tmpcolors, sizeof( m_tempcolors ) );
		}

		return true;
	}

	if( cmd == submenu ) {
		wstring ininame, iniparams;
		getcmd( ininame, iniparams, paramstr );
		wstring	fullname;
#ifdef UNDER_CE
		if( ininame[0] != L'\\' ) {
#else
		if( ininame[0] != L'\\' && ( ininame.size() < 2 || ininame[1] != L':') ) {
#endif
			fullname = m_appath;
		}
		fullname.append( ininame );

		HANDLE	h = CreateFile( fullname.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( h == INVALID_HANDLE_VALUE ) {
			return false;
		} else {
			CloseHandle( h );
		}
		m_launchers.push_back( launcher( true, ininame, cmdline, paramstr, comment, m_tempcolors ) );
		memcpy( m_tempcolors, m_colors, sizeof( m_tempcolors ) );
		return true;
	}

	if( cmd == exittext ) {
		if( paramstr.size() ) {
			m_exittext = paramstr;
			return true;
		}
		return false;
	}

	if( cmd == backtext ) {
		if( paramstr.size() ) {
			m_backtext = paramstr;
			return true;
		}
		return false;
	}

	if( cmd == exitcmd ) {
		DWORD	attr( GetFileAttributes( paramstr.c_str() ) );
		if( attr != INVALID_FILE_ATTRIBUTES && !( attr & FILE_ATTRIBUTE_DIRECTORY ) ) {
			m_exitcmd = paramstr;
		}
		return true;
	}

	if( cmd == preload ) {
		loaddlls( paramstr.c_str() );
		return true;
	}

	if( cmd == dpi ) {
		m_dpi = _wtoi( paramstr.c_str() );
		return true;
	}

	if( cmd == autorun )
	{
		return true;
	}

	vector< wstring >::iterator	stit( m_register.begin() );
	while( stit != m_register.end() ) {
		if( lcmd == *stit++ ) {
			return false;
		}
	}
	m_register.push_back( lcmd );

	if( lname.rfind( L".ini" ) == lname.size() - 4 )
	{
		wstring	fullname;
#ifdef UNDER_CE
		if( name[0] != L'\\' ) {
#else
		if( name[0] != L'\\' && ( name.size() < 2 || name[1] != L':') ) {
#endif
			fullname =m_appath;
		}
		fullname.append( name );
		txtfile	file( fullname, 1024 );
		if( file.getlasterror() ) {
			return false;
		}

		wstring	line;
		bool	done( false );

		while( file.readline( line ) ) {
			if( ! line.size() ) {
				continue;
			}
			size_t pos( line.rfind( L';' ));
			if( pos != line.npos ) {
				wstring	newcomment( line, pos+1 );
				ltrim( newcomment );
				line.resize( pos );
				rtrim( line );
				if( line.size() ) {
					done |= processline( line, newcomment );
				}
			} else {
				rtrim( line );
				wstring	nocomment;
				done |= processline( line, nocomment );
			}
		}
//		stack.pop_back();
		return done;
	}
	else if( lname.rfind( L".exe" ) == lname.size() - 4 )
	{
		DWORD attribs( GetFileAttributes( name.c_str() + ( (name[0] == L'*' || name[0] == L'<' ) ? 1 : 0 ) ) );
		if( attribs == INVALID_FILE_ATTRIBUTES || ( attribs & FILE_ATTRIBUTE_DIRECTORY ) ) {
			return false;
		}
	
		m_launchers.push_back( launcher( false, name, cmdline, paramstr, comment, m_tempcolors ) );
		memcpy( m_tempcolors, m_colors, sizeof( m_tempcolors ) );
		return true;

	} else 

	return false;
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::createbuttons( HWND hwnd )
{
	RECT	rect;
	int		numbuttons( (int)m_launchers.size()+1 );
	int		bw;
	double	bh;

	int numlaunchers( numbuttons - 1 );
	GetClientRect( hwnd, &rect );

	bool	twoclmn( numlaunchers > 4 );
	bool	longexit( !twoclmn || (numlaunchers&1) == 0);
	int		numlines;

	if( twoclmn ) {
		numlines = (numbuttons + 1) / 2;
		bw = (rect.right - rect.left) / 2;
		bh = ((double)rect.bottom - rect.top) / numlines;
	} else {
		numlines = numbuttons;
		bw = rect.right - rect.left;
		bh = ((double)rect.bottom - rect.top) / numbuttons;
	}

	HWND	button;
	int		index;

	for( index = 0; index < numlaunchers; index++ )
	{
		int		x, y, rbh;
		if( twoclmn ) {
			if( index < numlines - longexit) {	//first column
				x = 0;
				y = (int)(index * bh);
				rbh = (int)((index + 1) * bh) - y;
			} else {	//second column
				x = bw;
				y = (int)((index-numlines+longexit) * bh);
				rbh = ((int)((index+1-numlines+longexit) * bh)) -y;
			}
			
		} else {
			x = 0;
			y = (int)(index*bh);
			rbh = ((int)((index+1) * bh)) -y;
		}

		button = CreateWindowEx( WS_TABSTOP,
			TEXT ("BUTTON"),
			m_launchers[index].m_comment.size() ? m_launchers[index].m_comment.c_str() : m_launchers[index].m_command.c_str(),
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			x,
			y,
			bw,
			(int)rbh,
			hwnd,
			(HMENU) (500+index),
			m_instance,
			0
		);
		fancybutton *fbtn( new fancybutton( button, m_dpi, m_launchers[index].m_colors, 2, m_hmargin, m_vmargin, this ) );
		m_launchers[index].m_button = fbtn;
	}

	int		x, y, rbh;
	if( twoclmn ) {
		if( index < numlines - longexit) {	//first column
			y = (int)(index * bh);
			rbh = (int)((index + 1) * bh) - y;
		} else {	//second column
			y = (int)((index-numlines+longexit) * bh);
			rbh = ((int)((index+1-numlines+longexit) * bh)) -y;
		}
		
	} else {
		x = 0;
		y = (int)(index*bh);
		rbh = ((int)((index+1) * bh)) -y;
	}

	button = CreateWindowEx (WS_TABSTOP,
		TEXT ("BUTTON"),
		m_prevmenus.size() ? m_backtext.c_str() : m_exittext.c_str(),
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		longexit? 0 : bw,
		y,//(int)( ( numlines - 1) * bh ),
		longexit ? rect.right - rect.left : bw,
		rbh,//(int)bh,
		hwnd,
		(HMENU) (500+index),
		m_instance,
		0
	);
	fancybutton *fbtn( new fancybutton( button, m_dpi, m_exitcolors, 2, m_hmargin, m_vmargin, this ) );
	m_exitbutton = fbtn;
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::destroybuttons()
{
	HWND	wh;
	for( size_t index=0; index < m_launchers.size(); ++index ) {
		wh = m_launchers[index].m_button->gethandle();
		delete( m_launchers[index].m_button );
		DestroyWindow( wh );
	}
	wh = m_exitbutton->gethandle();
	delete m_exitbutton;
	DestroyWindow( wh );
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::savestate()
{
	DWORD					disp(0);
	HKEY					kh(0);

	LONG	error (	RegCreateKeyEx( HKEY_LOCAL_MACHINE, L"Software\\PNAMenu", 0, L"", REG_OPTION_NON_VOLATILE, 0, NULL, &kh, &disp ) );
	if( error != ERROR_SUCCESS ) {
		return;
	}

	writerms( kh, L"AutoRunHistory", m_autorunhist );
	writerms( kh, L"PrevMenus", m_prevmenus );
	RegSetValueEx( kh, L"CurMenu", 0, REG_SZ, (BYTE*)m_currmenu.c_str(), (m_currmenu.size() + 1) *sizeof( m_currmenu[0] ) );

	RegCloseKey( kh );
}

//////////////////////////////////////////////////////////////////////////////
void pnamenuapp::writerms(HKEY kh, wchar_t *name, pnamenuapp::wsvect_t &data)
{
	std::vector< wchar_t >	buf;
	wsvectit_t				cur( data.begin() ), end( data.end() );

	while( cur != end ) {
		size_t	curendpos( buf.size() );
		buf.resize( curendpos + cur->size() );
		std::copy( cur->begin(), cur->end(), &buf[curendpos] );
		buf.push_back(0);
		++cur;
	}
	buf.push_back(0);

	RegSetValueEx( kh, name, 0, REG_MULTI_SZ, (BYTE*)&buf[0], buf.size() * sizeof buf[0] );
}

//////////////////////////////////////////////////////////////////////////////
bool pnamenuapp::isprocessrunning(const wchar_t *procname)
{
#if defined( UNDER_CE )
	PROCESSENTRY32	pe;
	HANDLE	ths( CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 ) );
	if( ths == INVALID_HANDLE_VALUE ) {
		//TODO: throw an exception or something...
		return false;
	}

	pe.dwSize = sizeof( pe );
	BOOL	succ( Process32First( ths, & pe ) );
	while( succ )
	{
		std::wstring	exename( pe.szExeFile );
		if( exename.find( procname ) != std::wstring::npos ) {
			CloseToolhelp32Snapshot( ths );
			return true;
		}
		succ = Process32Next( ths, &pe );
	};
	
	CloseToolhelp32Snapshot( ths );
#endif
	return false;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
