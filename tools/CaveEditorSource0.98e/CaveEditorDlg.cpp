// CaveEditorDlg.cpp : implementation file
//


#include "stdafx.h"
#include "General.h"
#include "CaveEditor.h"
#include "CaveMapInfoDlg.h"
#include "CaveMapEditDlg.h"
#include "ScriptEditorDlg.h"
#include "PaletteSelectDlg.h"
#include "PBMEditorDlg.h"
#include "TilesetEditorDlg.h"
#include "WeaponEditorDlg.h"
#include "NpcExeEditorDlg.h"
#include "ReorderMapsDlg.h"

#include "NPCTableEditorDlg.h"

#include "CaveEditorDlg.h"
#include ".\caveeditordlg.h"
#include "CaveMapPropertiesDlg.h"


extern CCaveEditorDlg* mainWindow;
extern ScriptEditorDlg scriptEditorDlg;
//#include "SharedWindows.h"

#include "TextEntryDlg.h"
#include "OptionsDlg.h"
#include "GameSettingsDlg.h"

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CAboutDlg dialog used for App About


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CCaveEditorDlg dialog



CCaveEditorDlg::CCaveEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaveEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	close = false;
	changes = false;
	AfxInitRichEdit2();
	mainWindow = this;

	CFile config;

	CDC cdc;
	cdc.CreateCompatibleDC(NULL);

	if (!config.Open("CaveEditor.ini", CFile::modeRead | CFile::shareDenyWrite))//file doesn't exist
	{
		config.Open("CaveEditor.ini", CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite);
		//use "defaults" set in General.cpp

		config.Write(&CS_DEFAULT_SCALE, sizeof(CS_DEFAULT_SCALE));
		config.Write(&CS_DEFAULT_TILESET_WINDOW, sizeof(CS_DEFAULT_TILESET_WINDOW));
		config.Write(&CS_DEFAULT_WHEEL_ZOOM, sizeof(CS_DEFAULT_WHEEL_ZOOM));
		config.Write(&CS_DEFAULT_TILESET_SCALE, sizeof(CS_DEFAULT_TILESET_SCALE));
		config.Write(&CS_DEFAULT_TILETYPE_ALPHA, sizeof(CS_DEFAULT_TILETYPE_ALPHA));
		config.Write(&CS_DEFUALT_READ_ONLY, sizeof(CS_DEFUALT_READ_ONLY));
		config.Write(&CS_GRAPHIC_COLOR_DEPTH, sizeof(CS_GRAPHIC_COLOR_DEPTH));
	}
	else//file does exist
	{
		config.Read(&CS_DEFAULT_SCALE, sizeof(CS_DEFAULT_SCALE));
		config.Read(&CS_DEFAULT_TILESET_WINDOW, sizeof(CS_DEFAULT_TILESET_WINDOW));
		config.Read(&CS_DEFAULT_WHEEL_ZOOM, sizeof(CS_DEFAULT_WHEEL_ZOOM));
		config.Read(&CS_DEFAULT_TILESET_SCALE, sizeof(CS_DEFAULT_TILESET_SCALE));
		config.Read(&CS_DEFAULT_TILETYPE_ALPHA, sizeof(CS_DEFAULT_TILETYPE_ALPHA));
		config.Read(&CS_DEFUALT_READ_ONLY, sizeof(CS_DEFUALT_READ_ONLY));
		config.Read(&CS_GRAPHIC_COLOR_DEPTH, sizeof(CS_GRAPHIC_COLOR_DEPTH));
	}
	config.Close();

	CS_GRAPHIC_COLOR_DEPTH = cdc.GetDeviceCaps(BITSPIXEL);

}

void CCaveEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_MAPS, m_ListMaps);
	DDX_Control(pDX, LIST_TILESETS, m_ListTilesets);
	DDX_Control(pDX, LIST_SCRIPTS, m_ListScripts);
	DDX_Control(pDX, LIST_SPRITES, m_ListSpritesets);
	DDX_Control(pDX, LIST_BACKGROUNDS, m_ListBackgrounds);
}

BEGIN_MESSAGE_MAP(CCaveEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_TEST, OnFileTest)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_BN_CLICKED(BUTTON_MAP_DELETE, OnBnClickedMapDelete)
	ON_BN_CLICKED(BUTTON_MAP_ADD, OnBnClickedMapAdd)
	ON_BN_CLICKED(BUTTON_MAP_COPY, OnBnClickedMapCopy)
	ON_BN_CLICKED(BUTTON_MAP_EDIT, OnBnClickedMapEdit)
	ON_BN_CLICKED(BUTTON_TILESET_ADD, OnBnClickedTilesetAdd)
	ON_BN_CLICKED(BUTTON_TILESET_EDIT, OnBnClickedTilesetEdit)
	ON_BN_CLICKED(BUTTON_TILESET_COPY, OnBnClickedTilesetCopy)
	ON_BN_CLICKED(BUTTON_TILESET_DELETE, OnBnClickedTilesetDelete)
	ON_BN_CLICKED(BUTTON_SCRIPT_EDIT, OnBnClickedScriptEdit)
	ON_BN_CLICKED(BUTTON_NPCTABLE_EDIT, OnBnClickedNpctableEdit)
	ON_BN_CLICKED(BUTTON_GAME_SETTINGS, OnBnClickedGameSettings)
	ON_BN_CLICKED(BUTTON_SPRITES_ADD, OnBnClickedSpritesAdd)
	ON_BN_CLICKED(BUTTON_SPRITES_COPY, OnBnClickedSpritesCopy)
	ON_BN_CLICKED(BUTTON_SPRITES_EDIT, OnBnClickedSpritesEdit)
	ON_BN_CLICKED(BUTTON_SPRITES_DELETE, OnBnClickedSpritesDelete)
	ON_BN_CLICKED(BUTTON_BACKGROUND_ADD, OnBnClickedBackgroundAdd)
	ON_BN_CLICKED(BUTTON_BACKGROUND_COPY, OnBnClickedBackgroundCopy)
	ON_BN_CLICKED(BUTTON_BACKGROUND_EDIT, OnBnClickedBackgroundEdit)
	ON_BN_CLICKED(BUTTON_BACKGROUND_DELETE, OnBnClickedBackgroundDelete)
	ON_BN_CLICKED(BUTTON_EDIT_PBM, OnBnClickedEditPbm)
	ON_LBN_DBLCLK(LIST_MAPS, OnLbnDblclkMaps)
	ON_LBN_DBLCLK(LIST_TILESETS, OnLbnDblclkTilesets)
	ON_LBN_DBLCLK(LIST_SPRITES, OnLbnDblclkSprites)
	ON_LBN_DBLCLK(LIST_BACKGROUNDS, OnLbnDblclkBackgrounds)
	ON_LBN_DBLCLK(LIST_SCRIPTS, OnLbnDblclkScripts)
	ON_COMMAND(ID_MAPLIST_EDITMAP, OnMaplistEditmap)
	ON_COMMAND(ID_MAPLIST_EDITMAPPROPERTIES, OnMaplistEditmapproperties)
	ON_COMMAND(ID_MAPLIST_EDITMAPSCRIPT, OnMaplistEditmapscript)
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_TILESETLIST_EDITTILESET, OnTilesetlistEdittileset)
	ON_COMMAND(ID_SPRITESETLIST_EDITSPRITESET, OnSpritesetlistEditspriteset)
	ON_COMMAND(ID_BACKGROUNDLIST_EDITBACKGROUND, OnBackgroundlistEditbackground)
	ON_COMMAND(ID_SCRIPTLIST_EDITSCRIPT, OnScriptlistEditscript)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MAPLIST_COPYMAP, OnMaplistCopymap)
	ON_COMMAND(ID_MAPLIST_ADDMAP, OnMaplistAddmap)
	ON_COMMAND(ID_MAPLIST_DELETEMAP, OnMaplistDeletemap)
	ON_COMMAND(ID_DATA_BULLETDATA, OnDataBulletdata)
	ON_COMMAND(ID_DATA_NPCEXEDATA, OnDataNpcexedata)
//	ON_WM_KEYDOWN()
	ON_COMMAND(ID_DATA_REORDERMAPLIST, OnDataReordermaplist)
END_MESSAGE_MAP()


void CCaveEditorDlg::OnOK()
{
}

void CCaveEditorDlg::OnCancel()
{
	if (close)
	{
		if (changes)
		{
			switch(AfxMessageBox("Do you want to save before you quit?", MB_YESNOCANCEL))
			{
			case IDYES:
				OnFileSave();
				break;
			case IDNO:
				break;
			case IDCANCEL:
				close = false;
				return;
				break;
			}
			CDialog::OnCancel();
			return;
		}
		else
			CDialog::OnCancel();// don't allow [Escape]
		close = false;
	}
}

void CCaveEditorDlg::OnClose()
{
	close = true;

	CDialog::OnClose();
}


//This handles Context menus available through right clicking
void CCaveEditorDlg::OnContextMenu(CWnd* cwnd, CPoint point)
{
	int i = -1;
	BOOL outside;
	RECT rect;

	//select context specific menu
	m_ListMaps.GetClientRect(&rect);
	m_ListMaps.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 0;

	m_ListTilesets.GetClientRect(&rect);
	m_ListTilesets.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 1;

	m_ListSpritesets.GetClientRect(&rect);
	m_ListSpritesets.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 2;

	m_ListBackgrounds.GetClientRect(&rect);
	m_ListBackgrounds.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 3;

	m_ListScripts.GetClientRect(&rect);
	m_ListScripts.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 4;

	if (i == -1)//no context menu to display here
		return;

	switch (i)
	{
		case 0:
			m_ListMaps.ScreenToClient(&point);
			m_ListMaps.SetCurSel(m_ListMaps.ItemFromPoint(point, outside));
			m_ListMaps.ClientToScreen(&point);
			break;
		case 1:
			m_ListTilesets.ScreenToClient(&point);
			m_ListTilesets.SetCurSel(m_ListTilesets.ItemFromPoint(point, outside));
			m_ListTilesets.ClientToScreen(&point);
			break;
		case 2:
			m_ListSpritesets.ScreenToClient(&point);
			m_ListSpritesets.SetCurSel(m_ListSpritesets.ItemFromPoint(point, outside));
			m_ListSpritesets.ClientToScreen(&point);
			break;
		case 3:
			m_ListBackgrounds.ScreenToClient(&point);
			m_ListBackgrounds.SetCurSel(m_ListBackgrounds.ItemFromPoint(point, outside));
			m_ListBackgrounds.ClientToScreen(&point);
			break;
		case 4:
			m_ListScripts.ScreenToClient(&point);
			m_ListScripts.SetCurSel(m_ListScripts.ItemFromPoint(point, outside));
			m_ListScripts.ClientToScreen(&point);
			break;
	};

   CMenu* pPopup = conMenu.GetSubMenu(i);
   ASSERT(pPopup != NULL);
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x+1, point.y, this);
}

// CCaveEditorDlg message handlers

BOOL CCaveEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	mapEditDialog.Create(DIALOG_MAP_EDIT, NULL);
	scriptEditorDlg.Create(DIALOG_SCRIPT_EDITOR, NULL);
//	pbmEditorDlg.Create(DIALOG_PBM_EDITOR, this);

	VERIFY(conMenu.LoadMenu(MENU_LIST));//load context menu now

	//completments of http://www.codeguru.com/Cpp/W-D/dislog/tutorials/article.php/c4965
	//keyboard shortcuts!
	m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), m_lpszTemplateName);
	if (!m_hAccel)
		MessageBox("The hotkey table was not loaded");


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		  // Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCaveEditorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		return;
	}

	CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCaveEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCaveEditorDlg::OnFileSave()
{
	exe.Save();
	changes = false;
}

void CCaveEditorDlg::OnFileTest()
{
	OnFileSave();
	::SetCurrentDirectory(filePath);
	::spawnl(_P_NOWAIT, EXEname, EXEname);
//	::_execl(buffer, buffer);
}

void CCaveEditorDlg::OnFileLoad()
{
	int i;
	char szFilters[]= "*.exe Files|*.exe||";
	char buffer[1024];
	char temp[64];

  // Create an Open dialog
  CFileDialog loadFileDialog(TRUE, "*.exe", "Doukutsu.exe", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	filePath = loadFileDialog.GetPathName();
	EXEname = loadFileDialog.GetFileName();
	i = filePath.ReverseFind('\\');
	filePath.Delete(i+1, 256);
	CString fullname = filePath + EXEname;
	SetCurrentDirectory(filePath);
	if(!exe.Open(fullname))
	{
		filePath = "";//error loading EXE
		exe.Clear();
		return;
	}

	//activeate all working controls

	GetDlgItem(STATIC_MAPS)->EnableWindow(true);
	GetDlgItem(STATIC_TILESETS)->EnableWindow(true);
	GetDlgItem(STATIC_SPRITES)->EnableWindow(true);
	GetDlgItem(STATIC_BACKGROUNDS)->EnableWindow(true);
	GetDlgItem(STATIC_SCRIPTS)->EnableWindow(true);

	GetDlgItem(BUTTON_SCRIPT_EDIT)->EnableWindow(true);
	GetDlgItem(BUTTON_NPCTABLE_EDIT)->EnableWindow(true);
	GetDlgItem(BUTTON_GAME_SETTINGS)->EnableWindow(true);

	GetDlgItem(LIST_MAPS)->EnableWindow(true);
	GetDlgItem(LIST_TILESETS)->EnableWindow(true);
	GetDlgItem(LIST_SCRIPTS)->EnableWindow(true);
	GetDlgItem(LIST_SPRITES)->EnableWindow(true);
	GetDlgItem(LIST_BACKGROUNDS)->EnableWindow(true);

	GetDlgItem(BUTTON_MAP_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_MAP_EDIT)->EnableWindow(true);
	GetDlgItem(BUTTON_MAP_COPY)->EnableWindow(true);
	GetDlgItem(BUTTON_MAP_DELETE)->EnableWindow(true);

//	GetDlgItem(BUTTON_TILESET_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_TILESET_EDIT)->EnableWindow(true);
//	GetDlgItem(BUTTON_TILESET_COPY)->EnableWindow(true);
//	GetDlgItem(BUTTON_TILESET_DELETE)->EnableWindow(true);

//	GetDlgItem(BUTTON_SPRITES_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_SPRITES_EDIT)->EnableWindow(true);
//	GetDlgItem(BUTTON_SPRITES_COPY)->EnableWindow(true);
//	GetDlgItem(BUTTON_SPRITES_DELETE)->EnableWindow(true);

//	GetDlgItem(BUTTON_BACKGROUND_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_BACKGROUND_EDIT)->EnableWindow(true);
//	GetDlgItem(BUTTON_BACKGROUND_COPY)->EnableWindow(true);
//	GetDlgItem(BUTTON_BACKGROUND_DELETE)->EnableWindow(true);

	//adjust menu items availability (allow save, disable load)
	GetMenu()->EnableMenuItem(ID_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_FILE_TEST, MF_BYCOMMAND | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_FILE_LOAD, MF_BYCOMMAND | MF_GRAYED);

	GetMenu()->EnableMenuItem(ID_DATA_BULLETDATA, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_DATA_NPCEXEDATA, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_DATA_REORDERMAPLIST, MF_BYCOMMAND | MF_ENABLED);

	//adjust context menu item availability
	conMenu.EnableMenuItem(ID_MAPLIST_EDITMAP, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_EDITMAPPROPERTIES, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_EDITMAPSCRIPT, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_COPYMAP, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_ADDMAP, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_DELETEMAP, MF_BYCOMMAND | MF_ENABLED);

	conMenu.EnableMenuItem(ID_TILESETLIST_EDITTILESET, MF_BYCOMMAND | MF_ENABLED);
//	conMenu.EnableMenuItem(ID_TILESETLIST_COPYTILESET, MF_BYCOMMAND | MF_ENABLED);
//	conMenu.EnableMenuItem(ID_TILESETLIST_DELETETILESET, MF_BYCOMMAND | MF_ENABLED);

	conMenu.EnableMenuItem(ID_SPRITESETLIST_EDITSPRITESET, MF_BYCOMMAND | MF_ENABLED);
//	conMenu.EnableMenuItem(ID_SPRITESETLIST_COPYSPRITESET, MF_BYCOMMAND | MF_ENABLED);
//	conMenu.EnableMenuItem(ID_SPRITESETLIST_DELETESPRITESET, MF_BYCOMMAND | MF_ENABLED);

	conMenu.EnableMenuItem(ID_BACKGROUNDLIST_EDITBACKGROUND, MF_BYCOMMAND | MF_ENABLED);
//	conMenu.EnableMenuItem(ID_BACKGROUNDLIST_COPYBACKGROUND, MF_BYCOMMAND | MF_ENABLED);
//	conMenu.EnableMenuItem(ID_BACKGROUNDLIST_DELETEBACKGROUND, MF_BYCOMMAND | MF_ENABLED);

	conMenu.EnableMenuItem(ID_SCRIPTLIST_EDITSCRIPT, MF_BYCOMMAND | MF_ENABLED);

	for (i = 0; i < exe.mapCount; i++)//load map list
	{
		if (strcmp(exe.mapInfo[i].fileName, "\\empty") == 0)
			sprintf(buffer, "[%.3i]Map", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mapInfo[i].caption, exe.mapInfo[i].fileName);
		m_ListMaps.AddString(buffer);
	};

	//load list of available tile sets
	sprintf(buffer, "%sdata\\Stage\\", filePath);//which path to .\data\Stage\ 
	::SetCurrentDirectory(buffer);
	sprintf(temp, "Prt*.%s", CS_DEFAULT_PBM_EXT);
	m_ListTilesets.Dir(0x0010, temp);//get list of tileset files
	i = m_ListTilesets.GetCount();
	for (; i > 0; i--)//removes the "Prt" and ".pbm" from names in list
	{
		m_ListTilesets.GetText(0, temp);
		temp[strlen(temp)-4] = '\0';
		strcpy(buffer, &(temp[3]));
		m_ListTilesets.DeleteString(0);
		m_ListTilesets.AddString(buffer);
	};

	//Load list of available sprite sets
	sprintf(buffer, "%sdata\\Npc\\", filePath);//which path to .\data\Npc\ 
	::SetCurrentDirectory(buffer);
	sprintf(temp, "Npc*.%s", CS_DEFAULT_PBM_EXT);
	m_ListSpritesets.Dir(0x0010, temp);//get list of Sprite set  files
	i = m_ListSpritesets.GetCount();
	for (; i > 0; i--)//removes the "Npc" and ".pbm" from names in list
	{
		m_ListSpritesets.GetText(0, temp);
		temp[strlen(temp)-4] = '\0';
		strcpy(buffer, &(temp[3]));
		m_ListSpritesets.DeleteString(0);
		m_ListSpritesets.AddString(buffer);
	};

	//Load list of available backgrounds
	sprintf(buffer, "%sdata\\", filePath);//which path to .\data\ 
	::SetCurrentDirectory(buffer);
	sprintf(temp, "bk*.%s", CS_DEFAULT_PBM_EXT);
	m_ListBackgrounds.Dir(0x0010, temp);//get list of backgrounds files
	i = m_ListBackgrounds.GetCount();
	for (; i > 0; i--)
	{
		m_ListBackgrounds.GetText(0, temp);
		temp[strlen(temp)-4] = '\0';
		strcpy(buffer, &(temp[0]));
		m_ListBackgrounds.DeleteString(0);
		m_ListBackgrounds.AddString(buffer);
	};
//*
	//Load list of non map related scripts
	sprintf(buffer, "%sdata\\", filePath);//which path to .\data\ 
	::SetCurrentDirectory(buffer);
	m_ListScripts.Dir(0x0010, "*.tsc");//get list of tsc script files
	i = m_ListScripts.GetCount();
	for (; i > 0; i--)
	{
		m_ListScripts.GetText(0, temp);
		temp[strlen(temp)-4] = '\0';
		strcpy(buffer, &(temp[0]));
		m_ListScripts.DeleteString(0);
		m_ListScripts.AddString(buffer);
	};
//*/
	::SetCurrentDirectory(filePath);
}


void CCaveEditorDlg::reloadMapList()
{
	char buffer[256];
	int i;
	int sel;
	int top;

	sel = m_ListMaps.GetCurSel();
	top = m_ListMaps.GetTopIndex();
	i = m_ListMaps.GetCount();
	for (; i > 0; i--)//clear list
		m_ListMaps.DeleteString(0);

	for (i = 0; i < exe.mapCount; i++)//load map list
	{
		if (strcmp(exe.mapInfo[i].fileName, "\\empty") == 0)
			sprintf(buffer, "[%.3i]Map", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mapInfo[i].caption, exe.mapInfo[i].fileName);
		m_ListMaps.AddString(buffer);
	};

	if (sel < exe.mapCount)
		m_ListMaps.SetCurSel(sel);
	m_ListMaps.SetTopIndex(top);
}

void CCaveEditorDlg::OnOptions()
{
	OptionsDlg o;
	o.DoModal();
}

void CCaveEditorDlg::OnHelpContents()
{
	// TODO: Add your command handler code here
}

void CCaveEditorDlg::OnHelpAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CCaveEditorDlg::OnBnClickedMapDelete()
{
	if (m_ListMaps.GetCurSel() == LB_ERR)
		return;


	if (exe.DeleteMap(m_ListMaps.GetCurSel()))
	{
		changes = true;
		reloadMapList();
		m_ListMaps.GetFocus();
	}
}

void CCaveEditorDlg::OnBnClickedMapAdd()
{
	CaveMapINFO info;

	info.fileName[0] = '\0';
	info.caption[0] = '\0';
	info.bossNum = 0;
	info.backType = 0;
	strcpy(info.NPCset1, "0");
	strcpy(info.NPCset2, "0");
	strcpy(info.tileset, "0");
	strcpy(info.background, "bk0");

	CaveMapPropertiesDlg mapProp;
	mapProp.width = 21;
	mapProp.height = 16;
	mapProp.caveInfo = &info;
	if (mapProp.DoModal() != IDOK)
		return;

	if (exe.AddMap(&info, mapProp.width, mapProp.height))
	{
		changes = true;
		reloadMapList();
	}
}

void CCaveEditorDlg::OnBnClickedMapCopy()
{
	if (m_ListMaps.GetCurSel() == LB_ERR)
		return;

	TextEntryDlg txt;
	txt.text = exe.mapInfo[m_ListMaps.GetCurSel()].fileName;
	txt.text += "_";

	if (txt.DoModal() != IDOK)
		return;
	if (txt.text.IsEmpty())
		return;

	if (exe.CopyMap(m_ListMaps.GetCurSel(), txt.text))
	{
		changes = true;
		reloadMapList();
	}
}

void CCaveEditorDlg::OnBnClickedMapEdit()
{
	CaveMapINFO info;

	int i = m_ListMaps.GetCurSel();
	if (m_ListMaps.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a map first.", "Error");
		return;
	}
	if (strcmp(exe.mapInfo[m_ListMaps.GetCurSel()].fileName, "\\empty") == 0)
	{
		if (AfxMessageBox("Map previously deleted.  Create a new map?", MB_YESNO) == IDNO)
			return;

		//User wanted to create a map
		info.fileName[0] = '\0';
		info.caption[0] = '\0';
		info.bossNum = 0;
		info.backType = 0;
		strcpy(info.NPCset1, "0");
		strcpy(info.NPCset2, "0");
		strcpy(info.tileset, "0");
		strcpy(info.background, "bk0");

		CaveMapPropertiesDlg mapProp;
		mapProp.width = 2;
		mapProp.height = 2;
		mapProp.caveInfo = &info;
		if (mapProp.DoModal() != IDOK)//Get map properties for new map
			return;

		//Adding map
		if (exe.AddMap(&info, mapProp.width, mapProp. height,m_ListMaps.GetCurSel()))
		{
			changes = true;
			reloadMapList();
		}
		else
			return;//don't try to edit the map on failure

	}

	::SetCurrentDirectory(filePath);
	if (mapEditDialog.loadMap(&(exe.mapInfo[m_ListMaps.GetCurSel()]), m_ListMaps.GetCurSel()))
		mapEditDialog.ShowWindow(SW_SHOW);
}

void CCaveEditorDlg::OnBnClickedTilesetAdd()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedTilesetEdit()
{
	if (m_ListTilesets.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a tileset first.", "Error");
		return;
	}
	TilesetEditorDlg tilesetEditorDlg;

	char buffer[256];

	m_ListTilesets.GetText(m_ListTilesets.GetCurSel(), buffer);
	tilesetEditorDlg.tileset.load(buffer);

	if (tilesetEditorDlg.tileset.tileName[0] == '\0')
		return;//return if no file selected

	tilesetEditorDlg.DoModal();

	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedTilesetCopy()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedTilesetDelete()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedScriptEdit()
{
	if (m_ListScripts.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a script first.", "Error");
		return;
	}
	char temp[256];
	temp[0] = '\0';
	char buffer[1024];
	m_ListScripts.GetText(m_ListScripts.GetCurSel(), temp);
	if (temp[0] == '\0')//no script selected
		return;

	sprintf(buffer, ".\\data\\%s.tsc", /*filePath,*/ temp);
	scriptEditorDlg.ShowWindow(SW_SHOW);
	scriptEditorDlg.Open(buffer);
}

void CCaveEditorDlg::OnBnClickedNpctableEdit()
{
	NPCTableEditorDlg npc;
	npc.DoModal();
}

void CCaveEditorDlg::OnBnClickedGameSettings()
{
	GameSettingsDlg settings;
	settings.pbm = &(exe.string[CS_PBM_FOLDER_STRING][strlen(exe.string[CS_PBM_FOLDER_STRING])-3]);
	settings.title = exe.string[CS_WINDOW_STRING];
	settings.saveFile = exe.string[CS_PROFILE_DAT_STRING];
	settings.pixel = exe.string[CS_C_PIXEL_STRING];

	settings.titleX = *exe.titleX;
	settings.titleY = *exe.titleY;
	settings.titleMapEvent = *exe.titleMapEvent;
	settings.titleMap = *exe.titleMap;

	settings.startX = *exe.startX;
	settings.startY = *exe.startY;
	settings.startMapEvent = *exe.startMapEvent;
	settings.startMap = *exe.startMap;

	settings.startCurHP = *exe.startCurrentHP;
	settings.startMaxHP = *exe.startMaxHP;

	settings.startFlags = *exe.startFlags;
	settings.startFacing = *exe.startFacing;

	if (settings.DoModal() == IDOK)
	{
		if (strcmp(settings.pbm, &(exe.string[CS_PBM_FOLDER_STRING][strlen(exe.string[CS_PBM_FOLDER_STRING])-3]))!=0
		 ||strcmp(settings.pbm, &(exe.string[CS_PBM_FOLDER_STRING][strlen(exe.string[CS_PBM_FOLDER_STRING])-3]))!=0
		 ||strcmp(settings.pbm, &(exe.string[CS_PBM_FOLDER_STRING][strlen(exe.string[CS_PBM_FOLDER_STRING])-3]))!=0)
		{
			renameImages(settings.pbm);
			strcpy(&(exe.string[CS_PBM_FOLDER_STRING][strlen(exe.string[CS_PBM_FOLDER_STRING])-3]), settings.pbm);
			strcpy(&(exe.string[CS_PBM_FOLDER_STRING2][strlen(exe.string[CS_PBM_FOLDER_STRING2])-3]), settings.pbm);
			strcpy(&(exe.string[CS_PBM_FOLDER_STRING3][strlen(exe.string[CS_PBM_FOLDER_STRING3])-3]), settings.pbm);
		}

		strcpy(exe.string[CS_WINDOW_STRING], settings.title);
		strcpy(exe.string[CS_PROFILE_DAT_STRING], settings.saveFile);

		if (settings.pixel != exe.string[CS_C_PIXEL_STRING])
		{
			strcpy(exe.string[CS_C_PIXEL_STRING], settings.pixel);
			fixImages();
		}

		*exe.titleX = settings.titleX;
		*exe.titleY = settings.titleY;
		*exe.titleMapEvent = settings.titleMapEvent;
		*exe.titleMap = settings.titleMap;

		*exe.startX = settings.startX;
		*exe.startY = settings.startY;
		*exe.startMapEvent = settings.startMapEvent;
		*exe.startMap = settings.startMap;

		*exe.startCurrentHP = settings.startCurHP;
		*exe.startMaxHP = settings.startMaxHP;

		*exe.startFlags = settings.startFlags;
		*exe.startFacing = settings.startFacing;
		changes = true;
	}
}

void CCaveEditorDlg::renameImages(const char* newExt)
{
	char buffer[1024];
	char buffer2[1024];
	CString temp;

	int i;
	i = m_ListTilesets.GetCount()-1;
	for (; i >= 0; i--)
	{
		m_ListTilesets.GetText(i, temp);
		sprintf(buffer, ".\\data\\Stage\\Prt%s.%s", temp, CS_DEFAULT_PBM_EXT);
		sprintf(buffer2, ".\\data\\Stage\\Prt%s.%s", temp, newExt);
		::MoveFile(buffer, buffer2);
	};

	i = m_ListSpritesets.GetCount()-1;
	for (; i >= 0; i--)
	{
		m_ListSpritesets.GetText(i, temp);
		sprintf(buffer, ".\\data\\Npc\\Npc%s.%s", temp, CS_DEFAULT_PBM_EXT);
		sprintf(buffer2, ".\\data\\Npc\\Npc%s.%s", temp, newExt);
		::MoveFile(buffer, buffer2);
	};

	i = m_ListBackgrounds.GetCount()-1;
	for (; i >= 0; i--)
	{
		m_ListBackgrounds.GetText(i, temp);
		sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
		sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
		::MoveFile(buffer, buffer2);
	};

	temp = exe.string[CS_MYCHAR_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_TITLE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_ARMSIMAGE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_ARMS_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_ITEMIMAGE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_STAGEIMAGE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

// These two are taken care of in the Sprite List loop
  temp = exe.string[CS_NPC_NPCSYM_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_NPC_NPCREGU_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_CARET_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_BULLET_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_FACE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_FADE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_LOADING_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_TEXTBOX_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);

	temp = exe.string[CS_CASTS_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	sprintf(buffer2, ".\\data\\%s.%s", temp, newExt);
	::MoveFile(buffer, buffer2);
}

void CCaveEditorDlg::fixImages()
{
	memcpy(CaveBitMap::extra8, exe.string[CS_C_PIXEL_STRING], 8);

	char buffer[1024];
	CString temp;

	CaveBitMap file;

	int i;
	i = m_ListTilesets.GetCount()-1;
	for (; i >= 0; i--)
	{
		m_ListTilesets.GetText(i, temp);
		sprintf(buffer, ".\\data\\Stage\\Prt%s.%s", temp, CS_DEFAULT_PBM_EXT);
		file.Load(buffer);
		file.Save();
	};

	i = m_ListSpritesets.GetCount()-1;
	for (; i >= 0; i--)
	{
		m_ListSpritesets.GetText(i, temp);
		sprintf(buffer, ".\\data\\Npc\\Npc%s.%s", temp, CS_DEFAULT_PBM_EXT);
		file.Load(buffer);
		file.Save();
	};

	i = m_ListBackgrounds.GetCount()-1;
	for (; i >= 0; i--)
	{
		m_ListBackgrounds.GetText(i, temp);
		sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
		file.Load(buffer);
		file.Save();
	};

	temp = exe.string[CS_MYCHAR_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_TITLE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_ARMSIMAGE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_ARMS_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_ITEMIMAGE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_STAGEIMAGE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

// These two are taken care of in the Sprite List loop
  temp = exe.string[CS_NPC_NPCSYM_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_NPC_NPCREGU_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_CARET_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_BULLET_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_FACE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_FADE_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_LOADING_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_TEXTBOX_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();

	temp = exe.string[CS_CASTS_STRING];
	sprintf(buffer, ".\\data\\%s.%s", temp, CS_DEFAULT_PBM_EXT);
	file.Load(buffer);
	file.Save();
}


void CCaveEditorDlg::OnBnClickedSpritesAdd()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedSpritesCopy()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedSpritesEdit()
{
	if (m_ListSpritesets.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a sprite set first.", "Error");
		return;
	}

	PBMEditorDlg pbmEditorDlg(DIALOG_TILESET_EDITOR);
	char buffer[256];
	char temp[256];

	m_ListSpritesets.GetText(m_ListSpritesets.GetCurSel(), buffer);

	sprintf(temp, "%sdata\\Npc\\Npc%s.%s", filePath, buffer, CS_DEFAULT_PBM_EXT);
	if (!pbmEditorDlg.pbm.Load(temp))
	{
		MessageBox(temp, "Error loading file");
		return;//return with useless error message if error loading pbm
	};
	pbmEditorDlg.title = "Sprite Editor";
	pbmEditorDlg.DoModal();
}

void CCaveEditorDlg::OnBnClickedSpritesDelete()
{
	// TODO: Add your control notification handler code here
}


void CCaveEditorDlg::OnBnClickedBackgroundAdd()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedBackgroundCopy()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedBackgroundEdit()
{
	if (m_ListBackgrounds.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a background first.", "Error");
		return;
	}
	PBMEditorDlg pbmEditorDlg(DIALOG_TILESET_EDITOR);
	char buffer[256];
	char temp[256];

	m_ListBackgrounds.GetText(m_ListBackgrounds.GetCurSel(), buffer);

	sprintf(temp, ".\\data\\%s.%s", buffer, CS_DEFAULT_PBM_EXT);
	if (!pbmEditorDlg.pbm.Load(temp))
	{
		MessageBox(temp, "Error loading file");
		return;//return with useless error message if error loading pbm
	};
	pbmEditorDlg.title = "Background Editor";
	pbmEditorDlg.DoModal();
}

void CCaveEditorDlg::OnBnClickedBackgroundDelete()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedEditPbm()
{
	PBMEditorDlg pbmEditorDlg;
	pbmEditorDlg.OnFileLoad();

	if (pbmEditorDlg.pbm.IsNull())//if file didn't load then exit
		return;
	pbmEditorDlg.DoModal();
	::SetCurrentDirectory(filePath);
}

void CCaveEditorDlg::OnLbnDblclkMaps()
{
	OnBnClickedMapEdit();
}

void CCaveEditorDlg::OnLbnDblclkTilesets()
{
	OnBnClickedTilesetEdit();
}

void CCaveEditorDlg::OnLbnDblclkSprites()
{
	OnBnClickedSpritesEdit();
}

void CCaveEditorDlg::OnLbnDblclkBackgrounds()
{
	OnBnClickedBackgroundEdit();
}

void CCaveEditorDlg::OnLbnDblclkScripts()
{
	OnBnClickedScriptEdit();
}

void CCaveEditorDlg::OnMaplistEditmap()
{
	OnBnClickedMapEdit();
}

void CCaveEditorDlg::OnTilesetlistEdittileset()
{
	OnBnClickedTilesetEdit();
}

void CCaveEditorDlg::OnSpritesetlistEditspriteset()
{
	OnBnClickedSpritesEdit();
}

void CCaveEditorDlg::OnBackgroundlistEditbackground()
{
	OnBnClickedBackgroundEdit();
}

void CCaveEditorDlg::OnScriptlistEditscript()
{
	OnBnClickedScriptEdit();
}

void CCaveEditorDlg::OnMaplistEditmapproperties()
{
	CaveMapPropertiesDlg temp;
	temp.width = -1;
	temp.height = -1;
	temp.caveInfo = &(exe.mapInfo[m_ListMaps.GetCurSel()]);
	temp.oldFileName = exe.mapInfo[m_ListMaps.GetCurSel()].fileName;
	
	if (temp.DoModal() == IDOK)
	{
		reloadMapList();
		changes = true;
	}

}

void CCaveEditorDlg::OnMaplistEditmapscript()
{
	char temp[64];
	sprintf(temp, ".\\data\\Stage\\%s.tsc", exe.mapInfo[m_ListMaps.GetCurSel()].fileName);

	scriptEditorDlg.Open(temp);
	scriptEditorDlg.ShowWindow(SW_SHOW);
}

void CCaveEditorDlg::OnFileExit()
{
	OnClose();//just like how pressing the [x] works
	OnCancel();
}

void CCaveEditorDlg::OnDestroy()
{
	scriptEditorDlg.DestroyWindow();

	CDialog::OnDestroy();
}


void CCaveEditorDlg::OnMaplistCopymap()
{
	OnBnClickedMapCopy();
}

void CCaveEditorDlg::OnMaplistAddmap()
{
	OnBnClickedMapAdd();
}

void CCaveEditorDlg::OnMaplistDeletemap()
{
	OnBnClickedMapDelete();
}

void CCaveEditorDlg::OnDataBulletdata()
{
	WeaponEditorDlg dlg;
	dlg.DoModal();
}

void CCaveEditorDlg::OnDataNpcexedata()
{
	NpcExeEditorDlg dlg;
	dlg.DoModal();
}

void CCaveEditorDlg::OnDataReordermaplist()
{
	ReorderMapsDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		changes = true;
		reloadMapList();
	}

}

BOOL CCaveEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST && m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}
