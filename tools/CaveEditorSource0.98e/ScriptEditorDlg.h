#pragma once
#include "afxcmn.h"
#include "afxwin.h"


struct ScriptError
{
	int offset;
	int size;
	CString error;
};

// ScriptEditorDlg dialog

class ScriptEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(ScriptEditorDlg)

public:
	ScriptEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ScriptEditorDlg();

// Dialog Data
	enum { IDD = DIALOG_SCRIPT_EDITOR };

	void Open(char* file);
	void Save();

	HICON m_hIcon;

protected:
	TSC_Info command;//list of valid commands with syntax. lets hope there aren't any more than 256
	char fileName[256];
	int size;
	bool format;
	bool help;
	bool init;
	unsigned char* scriptText;
	int width;
	int height;

	void displayCommand(int i);
	void FormatText(bool line = false);
	bool CheckSyntax(int i = 0);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSyntax();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnSelchangeCommands();
	afx_msg void OnLbnDblclkCommands();
	afx_msg void OnBnClickedDetails();
	CRichEditCtrl scriptControl;
	afx_msg void OnEnChangeScript();
	afx_msg void OnEnSelchangeScript(NMHDR *pNMHDR, LRESULT *pResult);
	CListBox commandListControl;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEnUpdateScript();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
