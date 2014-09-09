/**
*	@file	OptionDialog.h
*	@brief	�I�v�V����
*/

#pragma once

#include <atlwin.h>
#include <atlddx.h>
#include "resource.h"
#include "Settings.h"
#include "ssl.h"


class COptionDialog : public CDialogImpl<COptionDialog>, public CWinDataExchange<COptionDialog>
{
public:
	enum { IDD = IDD_OPTION };

	BEGIN_DDX_MAP(CMainDlg)
		DDX_UINT_RANGE(IDC_EDIT_PROXYPORT, CSettings::s_proxyPort, (uint16_t)1024, (uint16_t)65535)

	END_DDX_MAP()

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_GENERATE_CACERTIFICATE, OnGenerateCACertificate)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());

		DoDataExchange(DDX_LOAD);
		return TRUE;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (!DoDataExchange(DDX_SAVE))
			return 0;

		CSettings::SaveSettings();

		EndDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnGenerateCACertificate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int ret = MessageBox(_T("SSL�t�B���^�����O�Ŏg�p����CA�ؖ������쐬���܂��B"), _T("�m�F"), MB_OKCANCEL);
		if (ret == IDCANCEL)
			return 0;

		try {
			GenerateCACertificate();
		} catch (std::exception& e) {
			CString errormsg = L"�ؖ����̍쐬�Ɏ��s���܂����B: ";
			errormsg += e.what();
			MessageBox(errormsg, _T("�G���["), MB_ICONERROR);
			return 0;
		}
		MessageBox(_T("CA�ؖ����̍쐬�ɐ������܂����I\nSSL�̃t�B���^�����O��Proxydomo�ċN����ɗL���ɂȂ�܂�\nSSL�t�B���^�����O�𖳌��ɂ������ꍇ�̓t�H���_���ɐ������ꂽ[ca.pem.crt]��[ca-key.pem]���폜���Ă��������B"), _T("����"));
		return 0;
	}

private:
	// Data members
};





















