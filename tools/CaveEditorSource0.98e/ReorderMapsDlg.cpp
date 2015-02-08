// ReorderMapsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "ReorderMapsDlg.h"

#include "General.h"
#include ".\reordermapsdlg.h"

// ReorderMapsDlg dialog

IMPLEMENT_DYNAMIC(ReorderMapsDlg, CDialog)
ReorderMapsDlg::ReorderMapsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ReorderMapsDlg::IDD, pParent)
{

}

ReorderMapsDlg::~ReorderMapsDlg()
{
}

void ReorderMapsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_MAPS, m_mapList);
}


BEGIN_MESSAGE_MAP(ReorderMapsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// ReorderMapsDlg message handlers

BOOL ReorderMapsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char buffer[256];
	int i;

	for (i = 0; i < exe.mapCount; i++)//load map list
	{
		if (strcmp(exe.mapInfo[i].fileName, "\\empty") == 0)
			sprintf(buffer, "[%.3i]Map", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mapInfo[i].caption, exe.mapInfo[i].fileName);
		m_mapList.AddString(buffer);
	};


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ReorderMapsDlg::OnBnClickedOk()
{
	CaveMapINFO* newMapInfo = new CaveMapINFO[exe.mapCount];
	char buffer[256];
	int i, j;
//*/

	for (i = 0; i < exe.mapCount; i++)//load map list
	{
		m_mapList.GetText(i, buffer);
		sscanf(buffer, "[%d]", &j);
		newMapInfo[i] = exe.mapInfo[j];
	}

	delete[] exe.section[exe.mapSection];
	exe.mapInfo = newMapInfo;
	exe.section[exe.mapSection] = exe.mapInfo;
//*/
	OnOK();
}
