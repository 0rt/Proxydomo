/**
*	@file	Settings.h
*	@brief	�S�t�B���^�[�ƑS���X�g�������Ă���
*/
/**
	this file is part of Proxydomo
	Copyright (C) amate 2013-

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#pragma once

#include <vector>
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <atlsync.h>
#include "FilterDescriptor.h"

class CSettings
{
public:
	static uint16_t		s_proxyPort;
	static bool			s_filterText;
	static bool			s_filterIn;
	static bool			s_filterOut;

	static bool			s_WebFilterDebug;

	static void	LoadSettings();
	static void	SaveSettings();

	// CCritSecLock	lock(CSettings::s_csFilters);
	static std::vector<std::unique_ptr<CFilterDescriptor>>	s_vecpFilters;
	static CCriticalSection									s_csFilters;

	static std::map<std::string, std::string>				s_mapListName;	// list name : file path
	static std::map<std::string, std::deque<std::string>>	s_mapLists;		// list name : contents

	static void	LoadFilter();
	static void SaveFilter();
};