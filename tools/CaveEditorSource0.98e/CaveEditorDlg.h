// CaveEditorDlg.h : header file
//



#pragma once
#include "afxwin.h"


class CaveMapEditDlg;//forward declaration for circular referencing

// CCaveEditorDlg dialog
class CCaveEditorDlg : public CDialog
{
// Construction
public:
	CCaveEditorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = DIALOG_CAVEEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation

public:
	bool changes;

protected:
	void renameImages(const char* newExt);
	void fixImages();
	void reloadMapList();

	CMenu conMenu;//context menu
	HACCEL m_hAccel; // accelerator table

	bool close;
	HICON m_hIcon;
	CaveMapEditDlg mapEditDialog;

	// Generated message map functions
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListMaps;
	CListBox m_ListTilesets;
	CListBox m_ListScripts;
	CListBox m_ListSpritesets;
	CListBox m_ListBackgrounds;

	afx_msg void OnFileSave();
	afx_msg void OnFileTest();
	afx_msg void OnFileLoad();
	afx_msg void OnOptions();
	afx_msg void OnHelpContents();
	afx_msg void OnHelpAbout();
	afx_msg void OnBnClickedMapDelete();
	afx_msg void OnBnClickedMapAdd();
	afx_msg void OnBnClickedMapCopy();
	afx_msg void OnBnClickedMapEdit();
	afx_msg void OnBnClickedTilesetAdd();
	afx_msg void OnBnClickedTilesetEdit();
	afx_msg void OnBnClickedTilesetCopy();
	afx_msg void OnBnClickedTilesetDelete();
	afx_msg void OnBnClickedScriptEdit();
	afx_msg void OnBnClickedNpctableEdit();
	afx_msg void OnBnClickedGameSettings();
	afx_msg void OnBnClickedSpritesAdd();
	afx_msg void OnBnClickedSpritesCopy();
	afx_msg void OnBnClickedSpritesEdit();
	afx_msg void OnBnClickedSpritesDelete();
	afx_msg void OnBnClickedBackgroundAdd();
	afx_msg void OnBnClickedBackgroundCopy();
	afx_msg void OnBnClickedBackgroundEdit();
	afx_msg void OnBnClickedBackgroundDelete();
	afx_msg void OnBnClickedEditPbm();
	afx_msg void OnLbnDblclkMaps();
	afx_msg void OnLbnDblclkTilesets();
	afx_msg void OnLbnDblclkSprites();
	afx_msg void OnLbnDblclkBackgrounds();
	afx_msg void OnLbnDblclkScripts();
	afx_msg void OnMaplistEditmap();
	afx_msg void OnMaplistEditmapproperties();
	afx_msg void OnMaplistEditmapscript();
	afx_msg void OnClose();
	afx_msg void OnFileExit();
	afx_msg void OnTilesetlistEdittileset();
	afx_msg void OnSpritesetlistEditspriteset();
	afx_msg void OnBackgroundlistEditbackground();
	afx_msg void OnScriptlistEditscript();
	afx_msg void OnDestroy();
	afx_msg void OnMaplistCopymap();
	afx_msg void OnMaplistAddmap();
	afx_msg void OnMaplistDeletemap();
	afx_msg void OnDataBulletdata();
	afx_msg void OnDataNpcexedata();
	afx_msg void OnDataReordermaplist();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
