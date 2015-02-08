#pragma once


// GameSettingsDlg dialog

class GameSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(GameSettingsDlg)

public:
	GameSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~GameSettingsDlg();

	CString pbm;
	CString title;
	CString saveFile;
	CString pixel;
	bool noPixel;

	int titleX;
	int titleY;
	int titleMap;
	int titleMapEvent;

	int startX;
	int startY;
	int startMapEvent;
	int startMap;

	int startCurHP;
	int startMaxHP;
	union
	{
		unsigned char startFlags;
		struct
		{
			unsigned char startFlag01:1;
			unsigned char startFlag02:1;
			unsigned char startFlag04:1;
			unsigned char startFlag08:1;
			unsigned char startFlag10:1;
			unsigned char startFlag20:1;
			unsigned char startFlag40:1;
			unsigned char startFlag80:1;
		};
	};
	int startFacing;

// Dialog Data
	enum { IDD = DIALOG_GAME_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnUpdatePbmExt();
	afx_msg void OnEnUpdateSaveFileName();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnUpdateCPixel();
	afx_msg void OnBnClickedNoCPixel();
};
