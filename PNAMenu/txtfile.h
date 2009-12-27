// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#pragma once
#include <windows.h>
#include <string>
#include <vector>

class txtfile
{
public:
	txtfile( std::wstring name, size_t buffersize = 4096 );
	virtual ~txtfile(void);
	DWORD	getlasterror() { return m_lasterror; }
	bool readline( std::wstring &out );

protected:
	bool	findeolbs( std::string &str );
	bool	findeolws( std::wstring &str );

	enum ftype { ASCII, UCS16, UTF8 };

	HANDLE						m_file;
	std::vector<unsigned char>	m_buffer;
	size_t						m_valid, m_index;
	DWORD						m_lasterror;
	ftype						m_type;
};
