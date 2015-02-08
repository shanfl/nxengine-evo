#pragma once


// WeaponEditorDlg dialog

class WeaponEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(WeaponEditorDlg)

public:
	int index;
	CaveBitMap wepImage;

	WeaponEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~WeaponEditorDlg();

// Dialog Data
	enum { IDD = DIALOG_WEAPON_EDITOR };

protected:
	void updateView();
	void saveData();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedPrev();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedOk();
};
