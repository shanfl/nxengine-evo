// NpcExeEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "NpcExeEditorDlg.h"
#include ".\npcexeeditordlg.h"

#include "General.h"


// NpcExeEditorDlg dialog

IMPLEMENT_DYNAMIC(NpcExeEditorDlg, CDialog)
NpcExeEditorDlg::NpcExeEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NpcExeEditorDlg::IDD, pParent)
{
}

NpcExeEditorDlg::~NpcExeEditorDlg()
{
}

void NpcExeEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NpcExeEditorDlg, CDialog)
	ON_BN_CLICKED(BUTTON_PREV, OnBnClickedPrev)
	ON_BN_CLICKED(BUTTON_NEXT, OnBnClickedNext)
	ON_EN_CHANGE(EDIT_NPC_CODE, OnEnChangeNpcCode)
	ON_EN_VSCROLL(EDIT_NPC_CODE, OnEnVscrollNpcCode)
END_MESSAGE_MAP()


// NpcExeEditorDlg message handlers

BOOL NpcExeEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	index = 0;

	//-0x400000 is to change from the memory adress to an offset into the physical file
	//the additional 0x001000 if to shift to the beginning of section 0
	//this should be changed to be dynamic but it's unlikely that the offset to section 0 will change
	sprintf(codeBuf, "NPC EXE Data Editor - Entity %i (0x%X)", index, exe.eJT[index]-0x400000);
	SetWindowText(codeBuf);

	exe.DecodeSegment(((BYTE*)exe.section[0]+exe.eJT[index]-0x401000), DECODE_MAX, codeBuf, 0xC3);
	SetDlgItemText(EDIT_NPC_CODE, codeBuf);

	exe.RawCodeSegment(((BYTE*)exe.section[0]+exe.eJT[index]-0x401000), DECODE_MAX, codeBuf, 0xC3);
	SetDlgItemText(EDIT_NPC_CODE_RAW, codeBuf);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

RECT NpcExeEditorDlg::findNpcRect(char* buffer)
{
	RECT rect;

	int i = 0;
	while (buffer[i] != 0)
	{
//		if (buffer[i] == 'M' && buffer[i] == 'O' && buffer[i] == 'V')

	}
	return rect;
}

void NpcExeEditorDlg::OnBnClickedPrev()
{

	if (index == 0)
		return;

	index--;

	sprintf(codeBuf, "NPC EXE Data Editor - Entity %i (0x%X)", index, exe.eJT[index]-0x400000);
	SetWindowText(codeBuf);

	exe.DecodeSegment(((BYTE*)exe.section[0]+exe.eJT[index]-0x401000), DECODE_MAX, codeBuf, 0xC3);
	SetDlgItemText(EDIT_NPC_CODE, codeBuf);

	exe.RawCodeSegment(((BYTE*)exe.section[0]+exe.eJT[index]-0x401000), DECODE_MAX, codeBuf, 0xC3);
	SetDlgItemText(EDIT_NPC_CODE_RAW, codeBuf);
}

void NpcExeEditorDlg::OnBnClickedNext()
{
	if (index > 400)//this is a random value greater than the entity count
		return;

	index++;

	sprintf(codeBuf, "NPC EXE Data Editor - Entity %i (0x%X)", index, exe.eJT[index]-0x400000);
	SetWindowText(codeBuf);

	exe.DecodeSegment(((BYTE*)exe.section[0]+exe.eJT[index]-0x401000), DECODE_MAX, codeBuf, 0xC3);
	SetDlgItemText(EDIT_NPC_CODE, codeBuf);

	exe.RawCodeSegment(((BYTE*)exe.section[0]+exe.eJT[index]-0x401000), DECODE_MAX, codeBuf, 0xC3);
	SetDlgItemText(EDIT_NPC_CODE_RAW, codeBuf);
}

void NpcExeEditorDlg::OnEnChangeNpcCode()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	((CEdit*)GetDlgItem(EDIT_NPC_CODE_RAW))->SetScrollPos(1, ((CEdit*)GetDlgItem(EDIT_NPC_CODE))->GetScrollPos(1));
}


void NpcExeEditorDlg::OnEnVscrollNpcCode()
{
	((CEdit*)GetDlgItem(EDIT_NPC_CODE_RAW))->SetScrollPos(1, ((CEdit*)GetDlgItem(EDIT_NPC_CODE))->GetScrollPos(1));

}
