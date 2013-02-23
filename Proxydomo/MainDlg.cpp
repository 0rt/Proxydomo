/**
*	@file	MainDlg.cpp
*	@brief	���C���t���[��
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
#include "MainDlg.h"
#include <fstream>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\ini_parser.hpp>
#include "AboutDlg.h"
#include "Misc.h"

using namespace boost::property_tree;

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	DoDataExchange(DDX_LOAD);

	{	// �g���C�A�C�R�����쐬
		NOTIFYICONDATA	nid = { sizeof(NOTIFYICONDATA) };
		nid.uFlags	= NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.hWnd	= m_hWnd;
		nid.hIcon	= hIconSmall;
		nid.uID		= kTrayIconId;
		nid.uCallbackMessage	= WM_TRAYICONNOTIFY;
		::_tcscpy_s(nid.szTip, _T("Proxydomo"));
		::Shell_NotifyIcon(NIM_ADD, &nid);
	}

	std::ifstream fs(Misc::GetExeDirectory() + _T("settings.ini"));
	if (fs) {
		ptree pt;
		read_ini(fs, pt);
		int	top  = pt.get("MainWindow.top", -1);
		int left = pt.get("MainWindow.left", -1);
		if (top != -1 && left != -1) {
			SetWindowPos(NULL, left, top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		if (pt.get("MainWindow.ShowWindow", true))
			ShowWindow(TRUE);
	} else {
		ShowWindow(TRUE);
		// center the dialog on the screen
		CenterWindow();
	}
	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	DoDataExchange(DDX_SAVE);


	{	// �g���C�A�C�R�����폜
		NOTIFYICONDATA	nid = { sizeof(NOTIFYICONDATA) };
		nid.hWnd	= m_hWnd;
		nid.uID		= kTrayIconId;
		::Shell_NotifyIcon(NIM_DELETE, &nid);
	}


	std::string settingsPath = CT2A(Misc::GetExeDirectory() + _T("settings.ini"));
	ptree pt;
	try {
		read_ini(settingsPath, pt);
	} catch (...) {

	}

	pt.put("MainWindow.ShowWindow", m_bVisibleOnDestroy);
	if (m_bVisibleOnDestroy) {
		RECT rc;
		GetWindowRect(&rc);
		pt.put("MainWindow.top", rc.top);
		pt.put("MainWindow.left", rc.left);
	}

	write_ini(settingsPath, pt);

	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}


LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	if (nType == SIZE_MINIMIZED) {
		ShowWindow(FALSE);
	}
}

LRESULT CMainDlg::OnTrayIconNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (lParam == WM_LBUTTONDOWN) {
		ShowWindow(TRUE);
		SetForegroundWindow(m_hWnd);
	} else if (lParam == WM_RBUTTONUP) {
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, 1U, _T("�I��"));
		CPoint pt;
		::GetCursorPos(&pt);
		::SetForegroundWindow(m_hWnd);
		BOOL bRet = menu.TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
		if (bRet == 1) {
			CloseDialog(0);
		}
	}
	return 0;
}


LRESULT CMainDlg::OnShowLogWindow(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_logView.IsWindow() == FALSE)
		m_logView.Create(m_hWnd);
	m_logView.ShowWindow();
	return 0;
}

LRESULT CMainDlg::OnShowFilterManageWindow(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_filterManagerWindow.IsWindow() == FALSE)
		m_filterManagerWindow.Create(m_hWnd);
	m_filterManagerWindow.ShowWindow(TRUE);
	return 0;
}

LRESULT CMainDlg::OnFilterButtonCheck(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE, wID);
	return 0;
}


void CMainDlg::CloseDialog(int nVal)
{
	m_bVisibleOnDestroy	= IsWindowVisible() != 0;
	DestroyWindow();
	::PostQuitMessage(0);
}















