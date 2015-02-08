// GameSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "GameSettingsDlg.h"
#include ".\gamesettingsdlg.h"

#include "General.h"

// GameSettingsDlg dialog

IMPLEMENT_DYNAMIC(GameSettingsDlg, CDialog)
GameSettingsDlg::GameSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GameSettingsDlg::IDD, pParent)
{
}

GameSettingsDlg::~GameSettingsDlg()
{
}

void GameSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GameSettingsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_UPDATE(EDIT_PBM_EXT, OnEnUpdatePbmExt)
	ON_EN_UPDATE(EDIT_SAVE_FILE_NAME, OnEnUpdateSaveFileName)
	ON_EN_UPDATE(EDIT_C_PIXEL, OnEnUpdateCPixel)
	ON_BN_CLICKED(CHECK_NO_C_PIXEL, OnBnClickedNoCPixel)
END_MESSAGE_MAP()


// GameSettingsDlg message handlers

void GameSettingsDlg::OnBnClickedOk()
{
	bool changes = false;
	CString temp;

	GetDlgItemText(EDIT_PBM_EXT, temp);
	if (temp != pbm) changes = true;
	GetDlgItemText(EDIT_SAVE_FILE_NAME, temp);
	if (temp != saveFile) changes = true;
	GetDlgItemText(EDIT_TITLE, temp);
	if (temp != title) changes = true;

	GetDlgItemText(EDIT_PBM_EXT, pbm);
	GetDlgItemText(EDIT_SAVE_FILE_NAME, saveFile);
	GetDlgItemText(EDIT_TITLE, title);

	if (IsDlgButtonChecked(CHECK_NO_C_PIXEL) != noPixel)
	{
		noPixel = IsDlgButtonChecked(CHECK_NO_C_PIXEL);
		changes = true;
	}
	if (!noPixel)
	{
		GetDlgItemText(EDIT_C_PIXEL, temp);
		if (temp != pixel) changes = true;
	}

	if (IsDlgButtonChecked(CHECK_NO_C_PIXEL))
		pixel = '\0';
	else
  		GetDlgItemText(EDIT_C_PIXEL, pixel);

	if (titleX != GetDlgItemInt(EDIT_TITLE_X))
	{
		changes = true;
		titleX = GetDlgItemInt(EDIT_TITLE_X);
	}
	if (titleY != GetDlgItemInt(EDIT_TITLE_Y))
	{
		changes = true;
		titleY = GetDlgItemInt(EDIT_TITLE_Y);
	}
	if (titleMapEvent != GetDlgItemInt(EDIT_TITLE_EVENT))
	{
		changes = true;
		titleMapEvent = GetDlgItemInt(EDIT_TITLE_EVENT);
	}
	if (titleMap != ((CComboBox*)GetDlgItem(COMBO_TITLE_MAP))->GetCurSel())
	{
		changes = true;
		titleMap = ((CComboBox*)GetDlgItem(COMBO_TITLE_MAP))->GetCurSel();
	}

	if (startX != GetDlgItemInt(EDIT_START_X))
	{
		changes = true;
		startX = GetDlgItemInt(EDIT_START_X);
	}
	if (startY != GetDlgItemInt(EDIT_START_Y))
	{
		changes = true;
		startY = GetDlgItemInt(EDIT_START_Y);
	}
	if (startMapEvent != GetDlgItemInt(EDIT_START_EVENT))
	{
		changes = true;
		startMapEvent = GetDlgItemInt(EDIT_START_EVENT);
	}
	if (startMap != ((CComboBox*)GetDlgItem(COMBO_START_MAP))->GetCurSel())
	{
		changes = true;
		startMap = ((CComboBox*)GetDlgItem(COMBO_START_MAP))->GetCurSel();
	}
	if (startCurHP != GetDlgItemInt(EDIT_START_HP))
	{
		changes = true;
		startCurHP = GetDlgItemInt(EDIT_START_HP);
	}
	if (startMaxHP != GetDlgItemInt(EDIT_START_MAX_HP))
	{
		changes = true;
		startMaxHP = GetDlgItemInt(EDIT_START_MAX_HP);
	}

	if (startFacing != GetDlgItemInt(EDIT_START_FACING))
	{
		changes = true;
		startFacing = GetDlgItemInt(EDIT_START_FACING);
	}

	if (startFlag01 != IsDlgButtonChecked(CHECK_FLAG1))
	{
		changes = true;
		startFlag01 = IsDlgButtonChecked(CHECK_FLAG1);
	}
	if (startFlag02 != IsDlgButtonChecked(CHECK_FLAG2))
	{
		changes = true;
		startFlag02 = IsDlgButtonChecked(CHECK_FLAG2);
	}
	if (startFlag04 != IsDlgButtonChecked(CHECK_FLAG3))
	{
		changes = true;
		startFlag04 = IsDlgButtonChecked(CHECK_FLAG3);
	}
	if (startFlag08 != IsDlgButtonChecked(CHECK_FLAG4))
	{
		changes = true;
		startFlag08 = IsDlgButtonChecked(CHECK_FLAG4);
	}
	if (startFlag10 != IsDlgButtonChecked(CHECK_FLAG5))
	{
		changes = true;
		startFlag10 = IsDlgButtonChecked(CHECK_FLAG5);
	}
	if (startFlag20 != IsDlgButtonChecked(CHECK_FLAG6))
	{
		changes = true;
		startFlag20 = IsDlgButtonChecked(CHECK_FLAG6);
	}
	if (startFlag40 != IsDlgButtonChecked(CHECK_FLAG7))
	{
		changes = true;
		startFlag40 = IsDlgButtonChecked(CHECK_FLAG7);
	}
	if (startFlag80 != IsDlgButtonChecked(CHECK_FLAG8))
	{
		changes = true;
		startFlag80 = IsDlgButtonChecked(CHECK_FLAG8);
	}


	if (changes)
		OnOK();
	else
		OnCancel();
}

void GameSettingsDlg::OnBnClickedCancel()
{
	OnCancel();
}

void GameSettingsDlg::OnEnUpdatePbmExt()
{
	char test[5];
	int sel;
	GetDlgItemText(EDIT_PBM_EXT, test, 5);
	if (strlen(test) > 3)
	{
		test[3] = '\0';
		sel = ((CEdit*)GetDlgItem(EDIT_PBM_EXT))->GetSel();
		if (sel > 3)
			sel = 3;
		SetDlgItemText(EDIT_PBM_EXT, test);
		((CEdit*)GetDlgItem(EDIT_PBM_EXT))->SetSel(sel, sel);
	}
}

void GameSettingsDlg::OnEnUpdateSaveFileName()
{
	char test[15];
	int sel;
	GetDlgItemText(EDIT_SAVE_FILE_NAME, test, 15);
	if (strlen(test) > 12)
	{
		test[12] = '\0';
		sel = ((CEdit*)GetDlgItem(EDIT_SAVE_FILE_NAME))->GetSel();
		if (sel > 12)
			sel = 12;
		SetDlgItemText(EDIT_SAVE_FILE_NAME, test);
		((CEdit*)GetDlgItem(EDIT_SAVE_FILE_NAME))->SetSel(sel, sel);
	}
}

void GameSettingsDlg::OnEnUpdateCPixel()
{
	char test[10];
	int sel;
	GetDlgItemText(EDIT_C_PIXEL, test, 10);
	if (strlen(test) > 8)
	{
		test[8] = '\0';
		sel = ((CEdit*)GetDlgItem(EDIT_C_PIXEL))->GetSel();
		if (sel > 8)
			sel = 8;
		SetDlgItemText(EDIT_C_PIXEL, test);
		((CEdit*)GetDlgItem(EDIT_C_PIXEL))->SetSel(sel, sel);
	}
}

void GameSettingsDlg::OnBnClickedNoCPixel()
{
	GetDlgItem(EDIT_C_PIXEL)->EnableWindow(!IsDlgButtonChecked(CHECK_NO_C_PIXEL));
}

BOOL GameSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;
	char buffer[256];
	//create maps lists
	for (i = 0; i < exe.mapCount; i++)//load map list
	{
		if (strcmp(exe.mapInfo[i].fileName, "\\empty") == 0)
			sprintf(buffer, "[%.3i] ***No Map***", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mapInfo[i].caption, exe.mapInfo[i].fileName);
		((CComboBox*)GetDlgItem(COMBO_START_MAP))->AddString(buffer);
		((CComboBox*)GetDlgItem(COMBO_TITLE_MAP))->AddString(buffer);
	};


	SetDlgItemText(EDIT_PBM_EXT, pbm);
	SetDlgItemText(EDIT_SAVE_FILE_NAME, saveFile);
	SetDlgItemText(EDIT_TITLE, title);

	if (pixel[0] != '\0')
	{
		noPixel = false;
		SetDlgItemText(EDIT_C_PIXEL, pixel);
	}
	else
	{
		noPixel = true;
		SetDlgItemText(EDIT_C_PIXEL, "(C)Pixel");
		CheckDlgButton(CHECK_NO_C_PIXEL, true);
		GetDlgItem(EDIT_C_PIXEL)->EnableWindow(!IsDlgButtonChecked(CHECK_NO_C_PIXEL));
	}

	SetDlgItemInt(EDIT_TITLE_X, titleX);
	SetDlgItemInt(EDIT_TITLE_Y, titleY);
	SetDlgItemInt(EDIT_TITLE_EVENT, titleMapEvent);
	((CComboBox*)GetDlgItem(COMBO_TITLE_MAP))->SetCurSel(titleMap);

	SetDlgItemInt(EDIT_START_X, startX);
	SetDlgItemInt(EDIT_START_Y, startY);
	SetDlgItemInt(EDIT_START_EVENT, startMapEvent);
	((CComboBox*)GetDlgItem(COMBO_START_MAP))->SetCurSel(startMap);

	SetDlgItemInt(EDIT_START_HP, startCurHP);
	SetDlgItemInt(EDIT_START_MAX_HP, startMaxHP);

	CheckDlgButton(CHECK_FLAG1, startFlag01);
	CheckDlgButton(CHECK_FLAG2, startFlag02);
	CheckDlgButton(CHECK_FLAG3, startFlag04);
	CheckDlgButton(CHECK_FLAG4, startFlag08);
	CheckDlgButton(CHECK_FLAG5, startFlag10);
	CheckDlgButton(CHECK_FLAG6, startFlag20);
	CheckDlgButton(CHECK_FLAG7, startFlag40);
	CheckDlgButton(CHECK_FLAG8, startFlag80);

	SetDlgItemInt(EDIT_START_FACING, startFacing);

	return TRUE;  // return TRUE unless you set the focus to a control
}
