// Copyright (c) 2009 Attila Body <attila.body@gmail.com>
// This file is released under GPLv3
#pragma once
#include <string>

class iconfig
{
public:
	virtual const std::wstring& getappath() = 0;
};