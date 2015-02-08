// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "OptionsDlg.h"
#include ".\optionsdlg.h"

#include "General.h"

// OptionsDlg dialog

IMPLEMENT_DYNAMIC(OptionsDlg, CDialog)
OptionsDlg::OptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionsDlg::IDD, pParent)
{
}

OptionsDlg::~OptionsDlg()
{
}

void OptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OptionsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, SLIDER_ALPHA, OnNMCustomdrawAlpha)
END_MESSAGE_MAP()


// OptionsDlg message handlers

void OptionsDlg::OnBnClickedOk()
{
	CS_DEFAULT_SCALE = GetDlgItemInt(EDIT_SCALE);
	CS_DEFAULT_WHEEL_ZOOM = IsDlgButtonChecked(RADIO_WHEEL_ZOOM);
	CS_DEFAULT_TILESET_WINDOW = IsDlgButtonChecked(CHECK_TILESET_WINDOW);
	CS_DEFAULT_TILESET_SCALE = GetDlgItemInt(EDIT_TILESET_SCALE);
	CS_DEFAULT_TILETYPE_ALPHA = ((CSliderCtrl*)GetDlgItem(SLIDER_ALPHA))->GetPos();
	CS_DEFUALT_READ_ONLY = IsDlgButtonChecked(CHECK_READ_ONLY);
	CS_GRAPHIC_COLOR_DEPTH = GetDlgItemInt(EDIT_COLORDEPTH);
	char buffer[1024];
	::GetCurrentDirectory(1024, buffer);
	::SetCurrentDirectory(progPath);

	CFile config;

	config.Open("CaveEditor.ini", CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite);

	config.Write(&CS_DEFAULT_SCALE, sizeof(CS_DEFAULT_SCALE));
	config.Write(&CS_DEFAULT_TILESET_WINDOW, sizeof(CS_DEFAULT_TILESET_WINDOW));
	config.Write(&CS_DEFAULT_WHEEL_ZOOM, sizeof(CS_DEFAULT_WHEEL_ZOOM));
	config.Write(&CS_DEFAULT_TILESET_SCALE, sizeof(CS_DEFAULT_TILESET_SCALE));
	config.Write(&CS_DEFAULT_TILETYPE_ALPHA, sizeof(CS_DEFAULT_TILETYPE_ALPHA));
	config.Write(&CS_DEFUALT_READ_ONLY, sizeof(CS_DEFUALT_READ_ONLY));
	config.Write(&CS_GRAPHIC_COLOR_DEPTH, sizeof(CS_GRAPHIC_COLOR_DEPTH));

	config.Close();

	::SetCurrentDirectory(buffer);

	OnOK();
}

BOOL OptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemInt(EDIT_SCALE, CS_DEFAULT_SCALE);

	if (CS_DEFAULT_WHEEL_ZOOM)
		CheckRadioButton(RADIO_WHEEL_ZOOM, RADIO_WHEEL_SCROLL, RADIO_WHEEL_ZOOM);
	else
		CheckRadioButton(RADIO_WHEEL_ZOOM, RADIO_WHEEL_SCROLL, RADIO_WHEEL_SCROLL);

	CheckDlgButton(CHECK_TILESET_WINDOW, CS_DEFAULT_TILESET_WINDOW);

	SetDlgItemInt(EDIT_TILESET_SCALE, CS_DEFAULT_TILESET_SCALE);

	((CSliderCtrl*)GetDlgItem(SLIDER_ALPHA))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(SLIDER_ALPHA))->SetPos(CS_DEFAULT_TILETYPE_ALPHA);

	CheckDlgButton(CHECK_READ_ONLY, CS_DEFUALT_READ_ONLY);

	SetDlgItemInt(EDIT_COLORDEPTH, CS_GRAPHIC_COLOR_DEPTH);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void OptionsDlg::OnNMCustomdrawAlpha(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	SetDlgItemInt(TEXT_ALPHA, ((CSliderCtrl*)GetDlgItem(SLIDER_ALPHA))->GetPos());

	RECT rect;
	GetDlgItem(TEXT_ALPHA_TEST_AREA)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	rect.left++;
	rect.right--;
	rect.top++;
	rect.bottom--;

	CDC* cdc = GetDC();

	GetDlgItem(TEXT_ALPHA_TEST_AREA)->RedrawWindow();
	cdc->TextOut(rect.left, rect.top, "Testing Transparency");

	CDC cdc2;
	CBitmap bmpFinal;

	cdc2.CreateCompatibleDC(cdc);
	bmpFinal.CreateBitmap(rect.right-rect.left, rect.bottom-rect.top, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp = (CBitmap *)(cdc2.SelectObject(&bmpFinal));

	cdc2.FillSolidRect(&rect, 0);

	BLENDFUNCTION blend;
	blend.AlphaFormat = 0;
	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = GetDlgItemInt(TEXT_ALPHA);
	cdc->AlphaBlend(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, &cdc2,
		0, 0, rect.right-rect.left, rect.bottom-rect.top, blend);

	ReleaseDC(cdc);
	bmpFinal.DeleteObject();
	cdc2.DeleteDC();

	*pResult = 0;
}
