// ScriptEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "General.h"
#include "ScriptEditorDlg.h"
#include ".\scripteditordlg.h"


//any color
#define EVENT_COLOR    RGB(  0,  0,  0)
#define COMMAND_COLOR  RGB(  0,  0,255)
#define NUMBER_COLOR   RGB(128,  0,128)
#define SPACER_COLOR   RGB(128,128,128)
#define INVALID_COLOR  RGB(255,  0,  0)

//bold or not bold
#define EVENT_STYLE    CFM_BOLD
#define COMMAND_STYLE  0
#define NUMBER_STYLE   0
#define SPACER_STYLE   CFM_BOLD
#define INVALID_STYLE  0

// ScriptEditorDlg dialog

IMPLEMENT_DYNAMIC(ScriptEditorDlg, CDialog)
ScriptEditorDlg::ScriptEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ScriptEditorDlg::IDD, pParent)
{
	scriptText = NULL;
	init = false;
	help = true;
	width = 0;
	height = 0;

}

ScriptEditorDlg::~ScriptEditorDlg()
{
	if (scriptText != NULL)
		delete[] scriptText;
}

void ScriptEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, EDIT_SCRIPT, scriptControl);
	DDX_Control(pDX, LIST_COMMANDS, commandListControl);
}


BEGIN_MESSAGE_MAP(ScriptEditorDlg, CDialog)
	ON_BN_CLICKED(BUTTON_SYNTAX, OnBnClickedSyntax)
	ON_BN_CLICKED(BUTTON_SAVE, OnBnClickedSave)
	ON_LBN_SELCHANGE(LIST_COMMANDS, OnLbnSelchangeCommands)
	ON_LBN_DBLCLK(LIST_COMMANDS, OnLbnDblclkCommands)
	ON_BN_CLICKED(BUTTON_DETAILS, OnBnClickedDetails)
	ON_EN_CHANGE(EDIT_SCRIPT, OnEnChangeScript)
	ON_NOTIFY(EN_SELCHANGE, EDIT_SCRIPT, OnEnSelchangeScript)
	ON_WM_DROPFILES()
	ON_EN_UPDATE(EDIT_SCRIPT, OnEnUpdateScript)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ScriptEditorDlg message handlers
BOOL ScriptEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		  // Set small icon

	RECT rect;
	GetClientRect(&rect);

	width = rect.right;
	height = rect.bottom;

	scriptControl.SetEventMask(ENM_UPDATE | ENM_CHANGE | ENM_SELCHANGE);
	scriptControl.LimitText(500000);//maximum script length

	format = true;
	init = true;

	char buffer[256];

	for (int i = 0; i < command.count; i++)
	{
		sprintf(buffer, "%s  -  %s", command.cmd[i].command, command.cmd[i].name);

		commandListControl.AddString(buffer);
	}
	return TRUE;
}

void ScriptEditorDlg::OnBnClickedSyntax()
{
	int offset = 0;
	while (true)
	{
		try//<-- first time using these things
		{
			if (CheckSyntax(offset))
			{
				if (offset == 0)
					MessageBox("No Syntax Errors.");
				break;
			}
		}
		catch (ScriptError err)
		{
			scriptControl.SetSel(err.offset, err.offset+err.size);
			if (MessageBox(err.error, "Syntax Error", MB_OKCANCEL) == IDCANCEL)
				break;
			offset = err.offset+3;
		}
	}
	scriptControl.SetFocus();
}

void ScriptEditorDlg::OnBnClickedSave()
{
	Save();
	scriptControl.SetModify(FALSE);
}

void ScriptEditorDlg::OnLbnSelchangeCommands()
{
	displayCommand(commandListControl.GetCurSel());
}

void ScriptEditorDlg::OnLbnDblclkCommands()
{
	//paste command
	scriptControl.ReplaceSel(command.cmd[commandListControl.GetCurSel()].command);
	FormatText(true);
	//switch focus to edit box for immediate typing
	scriptControl.SetFocus();
}

void ScriptEditorDlg::OnBnClickedDetails()
{
	CString temp;
	RECT rect, rect2;
	int i;

	GetDlgItemText(BUTTON_DETAILS, temp);
	GetWindowRect(&rect2);

	GetDlgItem(FRAME_SCRIPT_HELP)->GetWindowRect(&rect);

	if (!help)
	{
		help = true;
		SetDlgItemText(BUTTON_DETAILS, "Hide Command Details");
		CalcWindowRect(&rect);
		rect2.right = rect.right;
	}
	else
	{
		help = false;
		SetDlgItemText(BUTTON_DETAILS, "Show Command Details");
		rect2.right = rect.left-rect.right;
		CalcWindowRect(&rect);
		rect2.right += rect.right;
	}

	MoveWindow(&rect2);
}

void ScriptEditorDlg::Open(char* file)
{
	int i;
	format = false;
	strcpy(fileName, file);
	CFile scrFile;
	int shift = 0;
	char buffer[1024];

	sprintf(buffer, "Script Editor - %s", file);
	SetWindowText(buffer);

	scrFile.Open(fileName, CFile::modeRead | CFile::shareDenyNone);
	size = scrFile.GetLength();

	//resize string to hold entire script
	if (scriptText != NULL)
		delete[] scriptText;
	scriptText = new unsigned char [size+1];//+1 for the terminating character

	scrFile.Read(scriptText, size);
	scrFile.Close();

	shift = scriptText[size/2];//now shift all OTHER bytes by this amount

	for (i = 0; i < size; i++)//decode file
		if (i != size/2)
			scriptText[i] -= shift;

	scriptText[size] = '\0';
	scriptControl.LineScroll(-scriptControl.GetFirstVisibleLine(), 0);//scroll to top
	scriptControl.SetWindowText((char*)scriptText);
	format = true;
	FormatText();
}

void ScriptEditorDlg::Save()
{
	//this is an attempt to "throw" errors to see if it make more sense that passing them some other way
	try
	{
		CheckSyntax();
	}
	catch (ScriptError err)
	{
		if (MessageBox("Errors in syntax, are you sure you want to save?", 0, MB_YESNO) != IDYES)
			return;
	}
//1,461
	int i, j;
	CString buffer;

	char diff;
	scriptControl.GetTextRange(0, scriptControl.GetTextLength(), buffer);

	i = 0;
	//this adds back in the new line carriage return pair that's removed when editing
	while(i >=0 && i < buffer.GetLength())
	{
		j = buffer.Find(13, i)+2;
		if (j < i)
			break;
		else
			i = j;
		if (buffer[i-1] != 10)
			buffer.Insert(i-1, 10);
	}
	i = buffer.Find(13, i);
	diff = buffer[buffer.Find(13, 0)+1];

	size = buffer.GetLength();


	//resize string to hold entire script
	if (scriptText != NULL)
		delete[] scriptText;
	scriptText = new unsigned char [size+1];//+1 for the terminating character

	strcpy((char*)scriptText, buffer.GetString());

	int shift = scriptText[size/2];

	for (i = 0; i < size; i++)//encode file
		if (i != size/2)
			scriptText[i] += shift;

	CFileStatus status;
	if (CS_DEFUALT_READ_ONLY)
	{
		CFile::GetStatus(fileName, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(fileName, status);
	}

	CFile scrFile;
	CFileException e;
	if (scrFile.Open(fileName, CFile::modeWrite | CFile::modeCreate, &e))
	{
		scrFile.Write(scriptText, size);
		scrFile.Close();
	}
	else
		MessageBox("Error writing to file!");

	for (i = 0; i < size; i++)//decode file
		if (i != size/2)
			scriptText[i] -= shift;

}

void ScriptEditorDlg::FormatText(bool line)
{
	//this first if statement is to prevent this function from being called after
	//every syntanx highlighting that's done which would certainly cause an infinite loop
	if (format == false)
		return;
	format = false;

	int i, j, k, end;
	CString buffer;
	CString temp;
	char extra[5];


	//prevent flicker
	int eventMask = ::SendMessage(scriptControl.m_hWnd, EM_SETEVENTMASK, 0, 0);
	scriptControl.HideSelection(TRUE, FALSE);
	scriptControl.SetRedraw(FALSE);

	/********* basic formating ***************
	All commands will appear in blue
	All text will appear in black
	All numbers will appear in cyan
	All spacers will appear in gray
	All invalid commands or improperly formated ones
	will appear in red
	events will appear in bold
	********** basic formating **************/

	CHARRANGE oldSel;
	scriptControl.GetSel(oldSel);

//	CHARRANGE sel;

	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);

	scriptControl.GetTextRange(0, scriptControl.GetTextLength(), buffer);

	if (!line)
	{

		scriptControl.SetSel(0, -1);
	//	strcpy(cf.szFaceName, "Courier New");
		strcpy(cf.szFaceName, "Lucida Console");//I think this looks best.
		strcpy(cf.szFaceName, "Lucida Sans Unicode");//I think this looks best.
	//	strcpy(cf.szFaceName, "Fixedsys");
		cf.dwMask = CFM_FACE | CFM_BOLD | CFM_COLOR;
		cf.crTextColor =  RGB(0,0,0);
		cf.dwEffects = 0;
		scriptControl.SetSelectionCharFormat(cf);
		i = 0;
		end = buffer.GetLength();
	}
	else
	{
		i = scriptControl.LineIndex(scriptControl.LineFromChar(oldSel.cpMin)); 
		end = min(oldSel.cpMax+20, buffer.GetLength());

		cf.dwMask = CFM_BOLD | CFM_COLOR;
		scriptControl.SetSel(i, oldSel.cpMax+20);
		cf.crTextColor =  RGB(0,0,0);
		cf.dwEffects = 0;
		scriptControl.SetSelectionCharFormat(cf);
	}

	for (; i < end; i++)
	{
		//if a # is found this is the begining of an event ID
		if (buffer[i] == '#')
		{
			scriptControl.SetSel(i+1,i+5);//select whole ID
			k = -1;
			temp = scriptControl.GetSelText();

			//this checks to see if there is a number for the event between the numbers 0 and 9999
			//if not then this event is incorrectly written and is flagged red
			if (1 != sscanf(scriptControl.GetSelText(), "%d%s", &k, extra) || k < 0 || k > 9999)
				cf.crTextColor =  INVALID_COLOR;
			else
				cf.crTextColor =  EVENT_COLOR;
			cf.dwEffects = EVENT_STYLE;
			scriptControl.SetSelectionCharFormat(cf);
			i += 4;
		}
		else
		if (buffer[i] == '<')//It's a command
		{
			scriptControl.SetSel(i,i+4);//select whole ID
			temp = scriptControl.GetSelText();
			j = command.Find(scriptControl.GetSelText());

			if (j == -1)//command not found
			{
				cf.crTextColor =  INVALID_COLOR;
				cf.dwEffects = INVALID_STYLE;
				scriptControl.SetSelectionCharFormat(cf);
				i += 3;
			}
			else//Command Found
			{
				cf.crTextColor =  COMMAND_COLOR;
				cf.dwEffects = COMMAND_STYLE;
				scriptControl.SetSelectionCharFormat(cf);
				i += 3;
				j = command.cmd[j].args;//get number and convert to a number
				for (; j > 0; j--)
				{
					scriptControl.SetSel(i+1,i+5);//select whole ID
					k = -1;
					temp = scriptControl.GetSelText();

					if (1 != sscanf(temp, "%d%s", &k, extra) || k < 0 || k > 9999)
						cf.crTextColor =  INVALID_COLOR;
					else
						cf.crTextColor =  NUMBER_COLOR;
					cf.dwEffects = NUMBER_STYLE;
					scriptControl.SetSelectionCharFormat(cf);
					i+=4;
					if (j > 1)//account for spacer
					{
						i++;
						scriptControl.SetSel(i,i+1);//select whole ID
						cf.crTextColor =  SPACER_COLOR;
						cf.dwEffects = SPACER_STYLE;
						scriptControl.SetSelectionCharFormat(cf);
					}
				}
			}
		}
	}

	//Select old selection
	scriptControl.SetSel(oldSel);
	scriptControl.HideSelection(FALSE, FALSE);//show selection
	::SendMessage(scriptControl.m_hWnd, EM_SETEVENTMASK, 0, eventMask);
	scriptControl.SetRedraw(true);//allow redraw
	scriptControl.RedrawWindow();

	scriptControl.EmptyUndoBuffer();//prevent undo for now

	format = true;
}

bool ScriptEditorDlg::CheckSyntax(int i)
{
	int j, k, end;
	CString buffer;
	CString temp;
	char extra[5];

	ScriptError err;

	scriptControl.GetTextRange(0, scriptControl.GetTextLength(), buffer);

	end = buffer.GetLength();

	for (; i < end; i++)
	{
		//if a # is found this is the begining of an event ID
		if (buffer[i] == '#')
		{
			scriptControl.GetTextRange(i+1, i+5, temp);
			k = -1;
			if (1 != sscanf(temp, "%d%s", &k, extra) || k < 0 || k > 9999)
			{
				err.offset = i;
				sprintf(extra, "%d", scriptControl.LineFromChar(err.offset));
				err.error = "Invalid Event Number.  Line: ";
				err.error += extra;
				err.size = 5;
				throw err;
				return false;
			}
			i += 4;
		}
		else
		if (buffer[i] == '<')
		{
			scriptControl.GetTextRange(i,i+4, temp);//select whole ID
			j = command.Find(temp);

			if (j == -1)//command not found
			{
				err.offset = i;
				sprintf(extra, "%d", scriptControl.LineFromChar(err.offset));
				err.error = "Invalid Script Command.  Line: ";
				err.error += extra;
				err.size = 4;
				throw err;
				return false;
			}
			else//Command Found
			{
				i += 3;
				j = command.cmd[j].args;//get number and convert to a number
				for (; j > 0; j--)
				{
					scriptControl.GetTextRange(i+1,i+5, temp);//select whole ID
					k = -1;
					if (1 != sscanf(temp, "%d%s", &k, extra) || k < 0 || k > 9999)
					{
						err.offset = i+1;
						sprintf(extra, "%d", scriptControl.LineFromChar(err.offset));
						err.error = "Invalid Command Parameter.  Line: ";
						err.error += extra;
						err.size = 4;
						throw err;
						return false;
					}
					i+=4;
					if (j > 1)//account for spacer
						i++;
				}
			}
		}
	}
	return true;
}


void ScriptEditorDlg::OnEnChangeScript()
{
	FormatText(true);
}

void ScriptEditorDlg::OnEnUpdateScript()
{
}


void ScriptEditorDlg::OnEnSelchangeScript(NMHDR *pNMHDR, LRESULT *pResult)
{
	SELCHANGE *pSelChange = reinterpret_cast<SELCHANGE *>(pNMHDR);
	*pResult = 0;

	CString temp;

	int i = pSelChange->chrg.cpMin;
	scriptControl.GetTextRange(i, i, temp);
	while (i >= 0 && temp[0] != '<')//not beginning of command
	{
		i--;
		scriptControl.GetTextRange(i, i+1, temp);
	};
	if (i == -1)
		return;//no command found

	scriptControl.GetTextRange(i, i+4, temp);

	i = command.Find(temp);
	if (i == -1)
		return;//not a command

	displayCommand(i);

}


void ScriptEditorDlg::displayCommand(int i)
{
	char buffer[26];

	SetDlgItemText(TEXT_COMMAND, command.cmd[i].name);
	SetDlgItemText(TEXT_DESC, command.cmd[i].desc);

	switch (command.cmd[i].args)
	{
	case 0:
		strcpy(buffer, command.cmd[i].command);
		break;
	case 1:
		sprintf(buffer, "%sXXXX", command.cmd[i].command);
		break;
	case 2:
		sprintf(buffer, "%sXXXX:YYYY", command.cmd[i].command);
		break;
	case 3:
		sprintf(buffer, "%sXXXX:YYYY:ZZZZ", command.cmd[i].command);
		break;
	case 4:
		sprintf(buffer, "%sXXXX:YYYY:ZZZZ:WWWW", command.cmd[i].command);
		break;
	}

	SetDlgItemText(TEXT_SYNTAX, buffer);
}

void ScriptEditorDlg::OnDropFiles(HDROP hDropInfo)
{
	char fileName[512];
	CFile file;
	
	if (::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0) == 1)//only works with one file
	{
		::DragQueryFile(hDropInfo, 0, fileName, 512);

		Open(fileName);
	}

	CDialog::OnDropFiles(hDropInfo);
}

void ScriptEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (cx == 0 || cy == 0)
		return;

	int i, j, k;
	CRect rect, rect2;
	if (init)
	{

		GetDlgItem(FRAME_SCRIPT_HELP)->GetWindowRect(&rect);
		ScreenToClient(&rect);

		k = rect.left;
		if (!help)
		{
			i = 0;
			j = rect.right-rect.left;
		}
		else
		{
			i = -(rect.right-rect.left);
			j = 0;
		}
		rect.left = cx+i;
		rect.right = cx+j;
		GetDlgItem(FRAME_SCRIPT_HELP)->MoveWindow(&rect);

		k = rect.left - k;//this is the horizontal shift for all help controls

		if (k != 0)
		{
			GetDlgItem(TEXT_SCRIPT_FRAME1)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.MoveToX(rect.left+k);
			GetDlgItem(TEXT_SCRIPT_FRAME1)->MoveWindow(&rect);

			GetDlgItem(TEXT_SCRIPT_FRAME2)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.MoveToX(rect.left+k);
			GetDlgItem(TEXT_SCRIPT_FRAME2)->MoveWindow(&rect);

			GetDlgItem(TEXT_SCRIPT_COMMAND)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.MoveToX(rect.left+k);
			GetDlgItem(TEXT_SCRIPT_COMMAND)->MoveWindow(&rect);

			GetDlgItem(TEXT_COMMAND)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.MoveToX(rect.left+k);
			GetDlgItem(TEXT_COMMAND)->MoveWindow(&rect);

			GetDlgItem(TEXT_SCRIPT_DESC)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.MoveToX(rect.left+k);
			GetDlgItem(TEXT_SCRIPT_DESC)->MoveWindow(&rect);

			GetDlgItem(TEXT_DESC)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.MoveToX(rect.left+k);
			GetDlgItem(TEXT_DESC)->MoveWindow(&rect);

			GetDlgItem(TEXT_SYNTAX)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.MoveToX(rect.left+k);
			GetDlgItem(TEXT_SYNTAX)->MoveWindow(&rect);

			GetDlgItem(LIST_COMMANDS)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.MoveToX(rect.left+k);
			GetDlgItem(LIST_COMMANDS)->MoveWindow(&rect);

			GetDlgItem(TEXT_SCRIPT_FRAME1)->ShowWindow(SW_HIDE);
			GetDlgItem(TEXT_SCRIPT_FRAME1)->ShowWindow(SW_SHOW);
			GetDlgItem(TEXT_SCRIPT_FRAME2)->ShowWindow(SW_HIDE);
			GetDlgItem(TEXT_SCRIPT_FRAME2)->ShowWindow(SW_SHOW);
		}

		GetDlgItem(EDIT_SCRIPT)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right += k;
		rect.bottom += cy - height;
		GetDlgItem(EDIT_SCRIPT)->MoveWindow(&rect);

		GetDlgItem(BUTTON_SYNTAX)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(BUTTON_SYNTAX)->MoveWindow(&rect);

		GetDlgItem(BUTTON_SAVE)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(BUTTON_SAVE)->MoveWindow(&rect);

		GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(IDCANCEL)->MoveWindow(&rect);

		GetDlgItem(BUTTON_DETAILS)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(BUTTON_DETAILS)->MoveWindow(&rect);

		width = cx;
		height = cy;
	}
}
