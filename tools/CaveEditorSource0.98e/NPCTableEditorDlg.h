#pragma once


// NPCTableEditorDlg dialog

class NPCTableEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(NPCTableEditorDlg)

public:
	int record;
	bool init;
	bool close;
	bool change;
	NPCtable table;
	NPCTableEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~NPCTableEditorDlg();
	void DisplayRecord();
	void GetDialogData();

// Dialog Data
	enum { IDD = DIALOG_NPC_TABLE_EDITOR };

protected:
	void Redraw(CDC* cdc);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedSave();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeTileset();
	afx_msg void OnEnChangeHitTop();
	afx_msg void OnEnChangeHitRight();
	afx_msg void OnEnChangeHitLeft();
	afx_msg void OnEnChangeHitBottom();
	afx_msg void OnEnChangeDisplayTop();
	afx_msg void OnEnChangeDisplayLeft();
	afx_msg void OnEnChangeDisplayBottom();
	afx_msg void OnEnChangeDisplayRight();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeHurtSound();
	afx_msg void OnCbnSelchangeDeathSound();
	afx_msg void OnCbnSelchangeDeathGraphic();
	afx_msg void OnBnClickedFlag1();
	afx_msg void OnBnClickedFlag2();
	afx_msg void OnBnClickedFlag3();
	afx_msg void OnBnClickedFlag4();
	afx_msg void OnBnClickedFlag5();
	afx_msg void OnBnClickedFlag6();
	afx_msg void OnBnClickedFlag7();
	afx_msg void OnBnClickedFlag8();
	afx_msg void OnBnClickedFlag9();
	afx_msg void OnBnClickedFlag10();
	afx_msg void OnBnClickedFlag11();
	afx_msg void OnBnClickedFlag12();
	afx_msg void OnBnClickedFlag13();
	afx_msg void OnBnClickedFlag14();
	afx_msg void OnBnClickedFlag15();
	afx_msg void OnBnClickedFlag16();
	afx_msg void OnCbnSelchangeTestTileset();
};
