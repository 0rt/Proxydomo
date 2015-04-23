/**
*	@file	Settings.cpp
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
#include "stdafx.h"
#include "Settings.h"
#include <fstream>
#include <codecvt>
#include <random>
#include <atomic>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\ini_parser.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include <boost\format.hpp>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include "Misc.h"
#include "proximodo\util.h"
//#include "proximodo\matcher.h"
#include "Log.h"
#include "Matcher.h"
#include "CodeConvert.h"
#include "Logger.h"
#include "Matcher.h"

using namespace CodeConvert;
using namespace boost::property_tree;


////////////////////////////////////////////////////////////
// CSettings

enum { DEFAULTPECA_PORT = 6060 };
LPCWSTR kDefaultLanguage = L"English";

uint16_t		CSettings::s_proxyPort	= DEFAULTPECA_PORT;
bool			CSettings::s_privateConnection = true;
bool			CSettings::s_filterText	= true;
bool			CSettings::s_filterIn	= true;
bool			CSettings::s_filterOut	= true;
bool			CSettings::s_bypass	= false;
bool			CSettings::s_SSLFilter = false;
bool			CSettings::s_WebFilterDebug	= false;

bool			CSettings::s_useRemoteProxy	= false;
std::string		CSettings::s_defaultRemoteProxy;
std::set<std::string> CSettings::s_setRemoteProxy;

std::string		CSettings::s_urlCommandPrefix;	

std::wstring	CSettings::s_language = kDefaultLanguage;

bool			CSettings::s_tasktrayOnCloseBotton = false;

std::thread		CSettings::s_threadSaveFilter;

std::vector<std::unique_ptr<FilterItem>>	CSettings::s_vecpFilters;
CCriticalSection							CSettings::s_csFilters;

std::shared_ptr<Proxydomo::CMatcher>	CSettings::s_pBypassMatcher;

std::recursive_mutex						CSettings::s_mutexHashedLists;
std::unordered_map<std::string, std::unique_ptr<HashedListCollection>>	CSettings::s_mapHashedLists;



void	CSettings::LoadSettings()
{
	std::ifstream fs(Misc::GetExeDirectory() + _T("settings.ini"));
	if (fs) {
		ptree pt;
		read_ini(fs, pt);
		if (auto value = pt.get_optional<uint16_t>("Setting.ProxyPort"))
			s_proxyPort	= value.get();
		if (auto value = pt.get_optional<bool>("Setting.privateConnection"))
			s_privateConnection = value.get();
		if (auto value = pt.get_optional<bool>("Setting.filterText"))
			s_filterText	= value.get();
		if (auto value = pt.get_optional<bool>("Setting.filterIn"))
			s_filterIn	= value.get();
		if (auto value = pt.get_optional<bool>("Setting.filterOut"))
			s_filterOut	= value.get();
		if (auto value = pt.get_optional<bool>("Setting.bypass"))
			s_bypass = value.get();

		s_useRemoteProxy = pt.get<bool>("RemoteProxy.UseRemoteProxy", s_useRemoteProxy);
		s_defaultRemoteProxy = pt.get("RemoteProxy.defaultRemoteProxy", "");
		const int remoteProxyCount = pt.get("RemoteProxy.Count", 0);
		for (int i = 0; i < remoteProxyCount; ++i) {
			std::string path = boost::io::str(boost::format("RemoteProxy.proxy%d") % i);
			std::string remoteproxy = pt.get(path, "");
			if (remoteproxy.length())
				s_setRemoteProxy.insert(remoteproxy);
		}

		if (auto value = pt.get_optional<std::string>("Setting.language")) {
			s_language = UTF16fromUTF8(value.get());
		} else {
			CString language = kDefaultLanguage;
			int len = GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, nullptr, 0);
			if (len) {			
				GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, language.GetBuffer(len + 1), len);
				language.ReleaseBuffer();

				CString traslateFilePath = Misc::GetExeDirectory() + L"lang\\" + language + L".lng";
				if (::PathFileExists(traslateFilePath) == FALSE) {
					language = kDefaultLanguage;
				}
			}
			s_language = language;
		}

		if (auto value = pt.get_optional<bool>("Setting.tasktrayOnCloseBotton"))
			s_tasktrayOnCloseBotton = value.get();
	}

	// prefix��ݒ�
	enum { kPrefixSize = 8 };
	static const char charactorSelection[] = "abcdefghijklmnopqrstuvqxyz0123456789";
	std::random_device	randEngine;
	std::uniform_int_distribution<int> dist(0, _countof(charactorSelection) - 2);
	for (int i = 0; i < kPrefixSize; ++i)
		s_urlCommandPrefix += charactorSelection[dist(randEngine)];
	s_urlCommandPrefix += '_';

	// Bypass matcher���쐬
	s_pBypassMatcher = Proxydomo::CMatcher::CreateMatcher(L"$LST(Bypass)");

	CSettings::LoadFilter();

	ForEachFile(Misc::GetExeDirectory() + _T("lists\\"), [](const CString& filePath) {
		LoadList(filePath);
	});
}

void	CSettings::SaveSettings()
{
	std::string settingsPath = CT2A(Misc::GetExeDirectory() + _T("settings.ini"));
	ptree pt;
	try {
		read_ini(settingsPath, pt);
	} catch (...) {
	}

	pt.put("Setting.ProxyPort", s_proxyPort);
	pt.put("Setting.privateConnection", s_privateConnection);
	pt.put("Setting.filterText"	, s_filterText);
	pt.put("Setting.filterIn"	, s_filterIn);
	pt.put("Setting.filterOut"	, s_filterOut);
	pt.put("Setting.bypass", s_bypass);

	pt.erase("RemoteProxy");
	pt.put<bool>("RemoteProxy.UseRemoteProxy", s_useRemoteProxy);
	pt.put("RemoteProxy.defaultRemoteProxy", s_defaultRemoteProxy);
	const int remoteProxyCount = (int)s_setRemoteProxy.size();
	pt.put("RemoteProxy.Count", remoteProxyCount);
	int i = 0;
	for (auto& remoteproxy : s_setRemoteProxy) {
		std::string path = boost::io::str(boost::format("RemoteProxy.proxy%d") % i);
		++i;
		pt.put(path, remoteproxy);
	}

	pt.put("Setting.language", UTF8fromUTF16(s_language));

	pt.put<bool>("Setting.tasktrayOnCloseBotton", s_tasktrayOnCloseBotton);

	write_ini(settingsPath, pt);
}

void	LoadFilterItem(wptree& ptChild, std::vector<std::unique_ptr<FilterItem>>& vecpFilter)
{
	for (auto& ptIt : ptChild) {
		wptree& ptFilter = ptIt.second;
		std::unique_ptr<FilterItem>	pFilterItem(new FilterItem);
		if (ptIt.first == L"filter") {
			std::unique_ptr<CFilterDescriptor> pFilter(new CFilterDescriptor);
			pFilter->Active		= ptFilter.get<bool>(L"Active", true);
			pFilter->title		= ptFilter.get(L"title", L"");
			pFilter->version	= UTF8fromUTF16(ptFilter.get(L"version", L""));
			pFilter->author		= UTF8fromUTF16(ptFilter.get(L"author", L""));
			pFilter->comment	= UTF8fromUTF16(ptFilter.get(L"comment", L""));
			pFilter->filterType	= 
				static_cast<CFilterDescriptor::FilterType>(
					ptFilter.get<int>(L"filterType", (int)CFilterDescriptor::kFilterText));
			pFilter->headerName	= UTF8fromUTF16(ptFilter.get(L"headerName", L""));
			pFilter->multipleMatches= ptFilter.get<bool>(L"multipleMatches", false);
			pFilter->windowWidth	= ptFilter.get<int>(L"windowWidth", 128);
			pFilter->boundsPattern	= (ptFilter.get(L"boundsPattern", L""));
			pFilter->urlPattern		= (ptFilter.get(L"urlPattern", L""));
			pFilter->matchPattern	= (ptFilter.get(L"matchPattern", L""));
			pFilter->replacePattern	= (ptFilter.get(L"replacePattern", L""));
			
			pFilter->CreateMatcher();

			pFilterItem->pFilter = std::move(pFilter);
			vecpFilter.push_back(std::move(pFilterItem));

		} else if (ptIt.first == L"folder") {
			pFilterItem->name = ptFilter.get(L"<xmlattr>.name", L"no name").c_str();
			pFilterItem->active = ptFilter.get<bool>(L"<xmlattr>.active", true);
			pFilterItem->pvecpChildFolder.reset(new std::vector<std::unique_ptr<FilterItem>>);
			LoadFilterItem(ptFilter, *pFilterItem->pvecpChildFolder);
			vecpFilter.push_back(std::move(pFilterItem));
		}
	}
}

void CSettings::LoadFilter()
{
	CString filterPath = Misc::GetExeDirectory() + _T("filter.xml");
	if (::PathFileExists(filterPath) == FALSE)
		return ;

	std::wifstream	fs(filterPath);
	if (!fs) {
		MessageBox(NULL, _T("filter.xml�̃I�[�v���Ɏ��s"), NULL, MB_ICONERROR);
		return ;
	}
	fs.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));

	wptree pt;
	try {
		read_xml(fs, pt);
	} catch (...) {
		return ;
	}
	if (auto& opChild = pt.get_child_optional(L"ProxydomoFilter")) {
		wptree& ptChild = opChild.get();
		LoadFilterItem(ptChild, s_vecpFilters);
	}
}

void	SaveFilterItem(std::vector<std::unique_ptr<FilterItem>>& vecpFilter, wptree& pt, std::atomic<bool>& bCansel)
{
	for (auto& pFilterItem : vecpFilter) {
		if (bCansel.load())
			return;

		if (pFilterItem->pvecpChildFolder) {
			wptree ptChild;
			SaveFilterItem(*pFilterItem->pvecpChildFolder, ptChild, bCansel);
			wptree& ptFolder = pt.add_child(L"folder", ptChild);
			ptFolder.put(L"<xmlattr>.name", (LPCWSTR)pFilterItem->name);
			ptFolder.put(L"<xmlattr>.active", pFilterItem->active);

		} else {
			wptree ptFilter;
			CFilterDescriptor* filter = pFilterItem->pFilter.get();
			ptFilter.put(L"Active", filter->Active);
			ptFilter.put(L"title", filter->title);
			ptFilter.put(L"version", UTF16fromUTF8(filter->version));
			ptFilter.put(L"author", UTF16fromUTF8(filter->author));
			ptFilter.put(L"comment", UTF16fromUTF8(filter->comment));
			ptFilter.put(L"filterType", (int)filter->filterType);
			ptFilter.put(L"headerName", UTF16fromUTF8(filter->headerName));
			ptFilter.put(L"multipleMatches", filter->multipleMatches);
			ptFilter.put(L"windowWidth", filter->windowWidth);
			ptFilter.put(L"boundsPattern", (filter->boundsPattern));
			ptFilter.put(L"urlPattern", (filter->urlPattern));
			ptFilter.put(L"matchPattern", (filter->matchPattern));
			ptFilter.put(L"replacePattern", (filter->replacePattern));
			pt.add_child(L"filter", ptFilter);
		}
	}
}

void CSettings::SaveFilter()
{
	typedef std::vector<std::unique_ptr<FilterItem>>*	FilterFolder;
	auto pFilter = new std::vector<std::unique_ptr<FilterItem>>;
	//s_BookmarkList
	std::function <void (FilterFolder, FilterFolder)> funcCopy;
	funcCopy = [&](FilterFolder pFolder, FilterFolder pFolderTo) {
		for (auto it = pFolder->begin(); it != pFolder->end(); ++it) {
			FilterItem& item = *(*it);
			unique_ptr<FilterItem> pItem(new FilterItem(item));
			if (item.pvecpChildFolder) {
				pItem->pvecpChildFolder.reset(new std::vector<std::unique_ptr<FilterItem>>);
				funcCopy(item.pvecpChildFolder.get(), pItem->pvecpChildFolder.get());
			}
			pFolderTo->push_back(std::move(pItem));
		}
	};
	funcCopy(&s_vecpFilters, pFilter);

	static CCriticalSection	 s_cs;
	static std::atomic<bool> s_bCancel(false);

	std::thread tdSave([&, pFilter]() {
		for (;;) {
			if (s_cs.TryEnter()) {
				try {
					wptree ptChild;
					SaveFilterItem(*pFilter, ptChild, s_bCancel);
					if (s_bCancel.load()) {
						s_cs.Leave();
						s_bCancel.store(false);
						delete pFilter;
						return;
					}
					wptree pt;
					pt.add_child(L"ProxydomoFilter", ptChild);

					CString tempPath = Misc::GetExeDirectory() + _T("filter.temp.xml");
					CString filterPath = Misc::GetExeDirectory() + _T("filter.xml");

					std::wofstream	fs(tempPath);
					if (!fs) {
						//MessageBox(NULL, _T("filter.temp.xml�̃I�[�v���Ɏ��s"), NULL, MB_ICONERROR);
						s_cs.Leave();
						s_bCancel.store(false);
						delete pFilter;
						return;
					}
					fs.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>));

					write_xml(fs, pt, xml_parser::xml_writer_make_settings<std::wstring>(L' ', 2, xml_parser::widen<std::wstring>("UTF-8")));

					fs.close();
					::MoveFileEx(tempPath, filterPath, MOVEFILE_REPLACE_EXISTING);

				}
				catch (const boost::property_tree::ptree_error& error) {
					CString strError = _T("filter.xml�ւ̏������݂Ɏ��s\n");
					strError += error.what();
					MessageBox(NULL, strError, NULL, MB_ICONERROR);
				}
				s_cs.Leave();
				s_bCancel.store(false);
				delete pFilter;
				break;

			} else {
				// ���̃X���b�h���ۑ����������s��...
				ATLTRACE(_T("SaveFilter : TryEnter failed\n"));
				s_bCancel.store(true);
				::Sleep(100);
				continue;
			}
		}
	});
	s_threadSaveFilter.swap(tdSave);
	if (tdSave.joinable())
		tdSave.detach();
}

static void ActiveFilterCallFunc(std::vector<std::unique_ptr<FilterItem>>& vecFilters, 
								 std::function<void (CFilterDescriptor*)>& func)
{
	for (auto& filterItem : vecFilters) {
		if (filterItem->pFilter) {
			CFilterDescriptor* filter = filterItem->pFilter.get();
			if (filter->Active && filter->errorMsg.empty())
				func(filter);
		} else {
			if (filterItem->active)
				ActiveFilterCallFunc(*filterItem->pvecpChildFolder, func);
		}
	}
}

void CSettings::EnumActiveFilter(std::function<void (CFilterDescriptor*)> func)
{
	if (s_bypass)
		return;

	CCritSecLock	lock(CSettings::s_csFilters);
	ActiveFilterCallFunc(s_vecpFilters, func);
}

void CSettings::LoadList(const CString& filePath)
{
	CString ext = Misc::GetFileExt(filePath);
	ext.MakeLower();
	if (ext != _T("txt"))
		return;

	// �ŏI�������ݎ������擾
	HANDLE hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return ;

	FILETIME lastWriteTime = {};
	::GetFileTime(hFile, NULL, NULL, &lastWriteTime);
	uint64_t time = lastWriteTime.dwHighDateTime;
	time <<= 32;
	time |= lastWriteTime.dwLowDateTime;
	::CloseHandle(hFile);

	std::string filename = CT2A(Misc::GetFileBaseNoExt(filePath));
	static std::unordered_map<std::string, uint64_t> s_mapFileLastWriteTime;
	uint64_t& prevLastWriteTime = s_mapFileLastWriteTime[filename];
	if (prevLastWriteTime == time)
		return ;	// �X�V����Ă��Ȃ�������A��
	prevLastWriteTime = time;

	std::wifstream fs(filePath, std::ios::in | std::ios::binary);
	if (!fs)
		return ;
	fs.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::codecvt_mode::consume_header>));
	{
		s_mutexHashedLists.lock();
		auto& hashedLists = s_mapHashedLists[filename];
		if (hashedLists == nullptr) {
			hashedLists.reset(new HashedListCollection);
		}
		boost::unique_lock<boost::shared_mutex>	lock(hashedLists->mutex);
		hashedLists->PreHashWordList.clear();
		hashedLists->URLHashList.clear();
		hashedLists->deqNormalNode.clear();

		std::wstring pattern;
		std::wstring strLine;
		int nLineCount = 0;
		while (std::getline(fs, strLine)) {
			if (strLine.length() && (strLine[0] == L' ' || strLine[0] == L'\t')) {
				CUtil::trim(strLine);
				pattern += strLine;

			} else {
				CUtil::trim(strLine);
				if (pattern.length()) {
					bool bSuccess = _CreatePattern(pattern, *hashedLists, nLineCount);
					if (bSuccess == false)
						CLog::FilterEvent(kLogFilterListBadLine, nLineCount, filename, "");
				}

				if (strLine.empty() || strLine[0] == L'#') {
					pattern.clear();
				} else {
					pattern = strLine;
				}
			}
			++nLineCount;
			if (fs.eof())
				break;
		}
		if (pattern.length()) {
			bool bSuccess = _CreatePattern(pattern, *hashedLists, nLineCount);
			if (bSuccess == false)
				CLog::FilterEvent(kLogFilterListBadLine, nLineCount, filename, "");
		}
		s_mutexHashedLists.unlock();
	}
	CLog::FilterEvent(kLogFilterListReload, -1, filename, "");
}

static inline bool isNonWildWord(wchar_t c) {
	return (c != L'*' && c != +'\\' && c != L'[' && c != L'$' && c != L'(' && c != L')' && c != L'|' && c != L'&' && c != L'?' && c != L'~');
}


bool CSettings::_CreatePattern(std::wstring& pattern, HashedListCollection& listCollection, int listLine)
{
	// �Œ�v���t�B�b�N�X
	enum { kMaxPreHashWordLength = 7 };
	bool	bPreHash = true;
	std::size_t length = pattern.length();
	for (std::size_t i = 0; i < length && i < kMaxPreHashWordLength; ++i) {
		if (isNonWildWord(pattern[i]) == false) {
			bPreHash = false;
			break;
		}
	}
	if (bPreHash) {
		auto pmapPreHashWord = &listCollection.PreHashWordList;

		// �ŏ��Ƀp�^�[�������킩�e�X�g
		if (Proxydomo::CMatcher::CreateMatcher(pattern) == nullptr)
			return false;

		UnicodeString pat(pattern.c_str(), (int32_t)pattern.length());
		StringCharacterIterator patternIt(pat);
		for (; patternIt.current() != patternIt.DONE && patternIt.getIndex() < kMaxPreHashWordLength; patternIt.next()) {
			auto& pmapChildHashWord = (*pmapPreHashWord)[towlower(patternIt.current())];
			if (pmapChildHashWord == nullptr) {
				pmapChildHashWord.reset(new HashedListCollection::PreHashWord);
			}

			if (patternIt.hasNext() == false || ((patternIt.getIndex() + 1) == kMaxPreHashWordLength)) {
				patternIt.next();
				try {
					pmapChildHashWord->vecNode.emplace_back(std::shared_ptr<Proxydomo::CNode>(Proxydomo::CMatcher::expr(patternIt)), listLine);
				}
				catch (...) {
					return false;
				}
				pmapChildHashWord->vecNode.back().node->setNextNode(nullptr);
				return true;
			}
			pmapPreHashWord = &pmapChildHashWord->mapChildPreHashWord;
		}

	}

	// URL�n�b�V��
	std::size_t slashPos = pattern.find(L'/');
	if (slashPos != std::wstring::npos && slashPos != 0) {
		std::wstring urlHost = pattern.substr(0, slashPos);
		if (urlHost.length() >= 5) {
			auto funcSplitDomain = [&urlHost](std::wstring::const_iterator it) -> std::deque<std::wstring> {
				auto itbegin = it;
				std::deque<std::wstring>	deqDomain;
				std::wstring domain;
				for (; it != urlHost.cend(); ++it) {
					if (isNonWildWord(*it) == false) {
						deqDomain.clear();	// fail
						break;
					}
					if (*it == L'.') {
						if (std::next(urlHost.cbegin()) == it)	// *.�̏ꍇ��������
							continue;
						if (it != itbegin && domain.empty()) {	// ..�������ꍇ
							ATLASSERT(FALSE);
							deqDomain.clear();	// fail
							break;
						}
						deqDomain.push_back(domain);
						domain.clear();

					} else if (std::next(it) == urlHost.cend()) {
						domain.push_back(towlower(*it));
						deqDomain.push_back(domain);
						domain.clear();
						break;

					} else {
						domain.push_back(towlower(*it));
					}
				}
				ATLASSERT(domain.empty());
				return deqDomain;
			};

			auto funcGetFirstWildcard = [&urlHost](std::wstring::const_iterator& it) -> std::wstring {
				for (; it != urlHost.cend(); ++it) {
					if (*it == L'.') {
						std::wstring firstWildcard(urlHost.cbegin(), it);
						if (Proxydomo::CMatcher::CreateMatcher(firstWildcard) == nullptr) {
							return L"";
						}
						++it;
						return firstWildcard;
					}
				}
				return L"";
			};

			wchar_t first = urlHost.front();
			std::wstring firstWildcard;
			std::deque<std::wstring>	deqDomain;
			switch (first) {
				case L'*':
				{
					auto it = std::next(urlHost.cbegin());
					firstWildcard = funcGetFirstWildcard(it);
					deqDomain = funcSplitDomain(it);
				}
				break;

				case L'(':
				{
					int nkakko = 1;
					for (auto it = std::next(urlHost.cbegin()); it != urlHost.cend(); ++it) {
						if (*it == L'(') {
							++nkakko;
						} else if (*it == L')') {
							nkakko--;
							if (nkakko == 0) {
								std::advance(it, 1);
								firstWildcard = funcGetFirstWildcard(it);
								deqDomain = funcSplitDomain(it);
								break;
							}
						}
					}
				}
				break;

				case L'[':
				{
					for (auto it = std::next(urlHost.cbegin()); it != urlHost.cend(); ++it) {
						if (*it == L']') {
							if (*std::prev(it) == L'\\')	// \]�͖���
								continue;
							auto next1 = std::next(it);
							if (next1 != urlHost.cend() && *next1 == L'+') {	// ]+
								auto next2 = std::next(next1);
								if (next2 != urlHost.cend() && *next2 == L'+') {	// ]++
									std::advance(next2, 1);
									firstWildcard = funcGetFirstWildcard(next2);
									deqDomain = funcSplitDomain(next2);
									break;
								} else {
									std::advance(next1, 1);
									firstWildcard = funcGetFirstWildcard(next1);
									deqDomain = funcSplitDomain(next1);
									break;
								}
							}
							std::advance(it, 1);
							firstWildcard = funcGetFirstWildcard(it);	// ]
							deqDomain = funcSplitDomain(it);
							break;
						}
					}
				}
				break;

				case L'\\':
				{
					if (*std::next(urlHost.cbegin()) == L'w') {
						auto it = std::next(urlHost.cbegin(), 2);
						firstWildcard = funcGetFirstWildcard(it);
						deqDomain = funcSplitDomain(it);
					}
				}
				break;
			}
			if (firstWildcard.length() > 0 && deqDomain.size() > 0) {
				// �ŏ��Ƀp�^�[�������킩�e�X�g
				if (Proxydomo::CMatcher::CreateMatcher(pattern) == nullptr)
					return false;

				std::wstring lastWildcard = pattern.substr(slashPos + 1);
				auto	pmapChildURLHash = &listCollection.URLHashList;
				for (auto it = deqDomain.rbegin(); it != deqDomain.rend(); ++it) {
					auto& pURLHash = (*pmapChildURLHash)[*it];
					if (pURLHash == nullptr)
						pURLHash.reset(new HashedListCollection::URLHash);

					if (std::next(it) == deqDomain.rend()) {
						UnicodeString patfirst(firstWildcard.c_str(), (int32_t)firstWildcard.length());
						StringCharacterIterator patternfirstIt(patfirst);
						UnicodeString patlast(lastWildcard.c_str(), (int32_t)lastWildcard.length());
						StringCharacterIterator patternlastIt(patlast);
						pURLHash->vecpairNode.emplace_back(
							std::shared_ptr<Proxydomo::CNode>(Proxydomo::CMatcher::expr(patternfirstIt)),
							std::shared_ptr<Proxydomo::CNode>(Proxydomo::CMatcher::expr(patternlastIt)),
							listLine);
						auto& pair = pURLHash->vecpairNode.back();
						pair.nodeFirst->setNextNode(nullptr);
						pair.nodeLast->setNextNode(nullptr);
						return true;
					}
					pmapChildURLHash = &pURLHash->mapChildURLHash;
				}
			}
		}
	}

    // parse the pattern
	try {
		UnicodeString pat(pattern.c_str(), (int32_t)pattern.length());
		StringCharacterIterator patternIt(pat);
		listCollection.deqNormalNode.emplace_back(std::shared_ptr<Proxydomo::CNode>(Proxydomo::CMatcher::expr(patternIt)), listLine);
	} catch (...) {
		return false;
	}

	listCollection.deqNormalNode.back().node->setNextNode(nullptr);
	return true;
}