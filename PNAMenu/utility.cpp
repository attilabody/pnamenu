#include "stdafx.h"
#include "utility.h"

//////////////////////////////////////////////////////////////////////////////
bool getcmd( std::wstring &cmd, std::wstring &params, const std::wstring &cmdline )
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
void towide( std::wstring &dst, const std::string &src )
{
	int rbs( MultiByteToWideChar( CP_UTF8, 0, src.c_str(), -1, NULL, 0 ));
	if( rbs < 0 ) {
		return;
	}
	std::vector<wchar_t>	buf( rbs );
	rbs = MultiByteToWideChar( CP_UTF8, 0, src.c_str(), -1, &buf[0], (int)buf.size() );
	if( rbs >=0 ) {
		dst = &buf[0];
	}
}

//////////////////////////////////////////////////////////////////////////////
void tombcs( std::string &dst, const std::wstring &src )
{
	std::vector<char>	buf( ( src.size() + 1 ) * 2 );
	wcstombs( &buf[0], src.c_str(), buf.size() );
	dst = &buf[0];
}

//////////////////////////////////////////////////////////////////////////////
void tombcs( std::string &dst, const wchar_t *src )
{
	std::vector<char>	buf( ( wcslen( src ) + 1 ) * 2 );
	wcstombs( &buf[0], src, buf.size() );
	dst = &buf[0];
}

//////////////////////////////////////////////////////////////////////////////
void rtrim( std::string &s )
{
	std::string::size_type	pos( s.find_last_not_of( " \t" ) );
	if( pos != std::string::npos ) {
		s.resize( pos+1 );
	} else {
		s.clear();
	}
}

//////////////////////////////////////////////////////////////////////////////
void rtrim( std::wstring &s )
{
	std::wstring::size_type	pos( s.find_last_not_of( L" \t" ) );
	if( pos != std::wstring::npos ) {
		s.resize( pos+1 );
	} else {
		s.clear();
	}
}

//////////////////////////////////////////////////////////////////////////////
void ltrim( std::string &s )
{
	std::string::size_type	pos( s.find_first_not_of( " \t" ) );
	if( pos == s.npos ) {
		s.clear();
	} else if( pos ) {
		s.erase( 0, pos );
	}
}

//////////////////////////////////////////////////////////////////////////////
void ltrim( std::wstring &s )
{
	std::wstring::size_type	pos( s.find_first_not_of( L" \t" ) );
	if( pos == s.npos ) {
		s.clear();
	} else if( pos ) {
		s.erase( 0, pos );
	}
}

//////////////////////////////////////////////////////////////////////////////
bool getparams( std::vector<std::wstring> &params, const std::wstring &cmdline )
{
	wchar_t	prot(0), pch(0);
	std::wstring	tmp;
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

