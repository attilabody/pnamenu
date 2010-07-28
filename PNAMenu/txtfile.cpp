// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#include "StdAfx.h"
#include "txtfile.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////
txtfile::txtfile( std::wstring name, size_t buffersize ) :
m_file( INVALID_HANDLE_VALUE ),
m_valid(0),
m_index(0),
m_lasterror( NO_ERROR ),
m_buffer( buffersize)
{
	BOOL	succ(FALSE);
	DWORD	rd;

	m_file = CreateFile( name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( m_file == INVALID_HANDLE_VALUE ) {
		m_lasterror = GetLastError();
		return;
	}
	succ = ReadFile( m_file, &m_buffer[0], (DWORD)m_buffer.size(), &rd, NULL );
	if( !succ ) {
		m_lasterror = GetLastError();
		return;
	}
	m_valid = rd;

	if( m_valid >= 2 && m_buffer[0] == 0xff && m_buffer[1] == 0xfe) {
		m_type = UCS16;
		m_index += 2;
	}else if( m_valid >= 3 && m_buffer[0] == 0Xef && m_buffer[1] == 0xbb && m_buffer[2] == 0xbf ) {
		m_type = UTF8;
		m_index +=3;
	} else {
		m_type = ASCII;
	}
}

//////////////////////////////////////////////////////////////////////////////
txtfile::~txtfile(void)
{
	CloseHandle( m_file );
}

//////////////////////////////////////////////////////////////////////////////
bool txtfile::readline(std::wstring &out)
{
	string	btmp;
	wstring	wtmp;

	bool	eolfound( m_type == UCS16 ? findeolws( wtmp ) : findeolbs( btmp ) );

	while( !eolfound ) {
		DWORD	rd;
		BOOL	succ( ReadFile( m_file, &m_buffer[0], (DWORD)m_buffer.size(), &rd, NULL ) );
		if( !succ ) {
			m_lasterror = GetLastError();
			break;
		} else if( !rd ) {
			break;
		} else {
			m_valid = rd;
			m_index = 0;
		}
		eolfound = m_type == UCS16 ? findeolws( wtmp ) : findeolbs( btmp );
	}

	if( m_type != UCS16 )
	{
		int rbs( MultiByteToWideChar( m_type == UTF8 ? CP_UTF8 : CP_ACP, 0, btmp.c_str(), -1, NULL, 0 ));
		if( rbs >= 0 ) {
			vector<wchar_t>	buf( rbs );
			rbs = MultiByteToWideChar( m_type == UTF8 ? CP_UTF8 : CP_ACP, 0, btmp.c_str(), -1, &buf[0], (int)buf.size() );
			if( rbs >=0 ) {
				out = &buf[0];
			}
		}
	} else {
		out = wtmp;
	}

	return eolfound || out.size();
}

//////////////////////////////////////////////////////////////////////////////
bool txtfile::findeolbs( string &str )
{
	size_t	index;
	if( m_index >= m_valid ) {
		return false;
	}
	char	*ptr( (char*)&m_buffer[m_index] );
	for( index = m_index; index < m_valid; ++index, ++ptr ) {
		if( *ptr == 0x0a ) {
			m_index = index + 1;
			return true;
		} else if( *ptr != 0x0d ) {
			str.push_back( *ptr );
		}
	}
	m_index = index + 1;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
bool txtfile::findeolws( wstring &str )
{
	size_t	index;
	if( m_index >= m_valid ) {
		return false;
	}
	wchar_t	*ptr( (wchar_t*)&m_buffer[m_index] );
	for( index = m_index; index < m_valid; index+=2, ++ptr ) {
		if( *ptr == 0x0a ) {
			m_index = index + 2;
			return true;
		} else if( *ptr != 0x0d ) {
			str.push_back( *ptr );
		}
	}
	m_index = index + 2;
	return false;
}