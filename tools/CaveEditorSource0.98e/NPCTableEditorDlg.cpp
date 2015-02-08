// NPCTableEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "General.h"

#include "NPCTableEditorDlg.h"
#include ".\npctableeditordlg.h"

//#include "SharedWindows.h"

// NPCTableEditorDlg dialog
CaveBitMap testBitmap;

IMPLEMENT_DYNAMIC(NPCTableEditorDlg, CDialog)
NPCTableEditorDlg::NPCTableEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NPCTableEditorDlg::IDD, pParent)
{
	record = 0;
	init = false;
	change = false;
	close = false;
	char temp[100];
	::GetCurrentDirectory(100, temp);
	table.load();//load data
}

NPCTableEditorDlg::~NPCTableEditorDlg()
{
}

void NPCTableEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NPCTableEditorDlg, CDialog)
	ON_BN_CLICKED(BUTTON_SAVE, OnBnClickedSave)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(COMBO_TILESET, OnCbnSelchangeTileset)
	ON_EN_CHANGE(EDIT_HIT_TOP, OnEnChangeHitTop)
	ON_EN_CHANGE(EDIT_HIT_RIGHT, OnEnChangeHitRight)
	ON_EN_CHANGE(EDIT_HIT_LEFT, OnEnChangeHitLeft)
	ON_EN_CHANGE(EDIT_HIT_BOTTOM, OnEnChangeHitBottom)
	ON_EN_CHANGE(EDIT_DISPLAY_TOP, OnEnChangeDisplayTop)
	ON_EN_CHANGE(EDIT_DISPLAY_LEFT, OnEnChangeDisplayLeft)
	ON_EN_CHANGE(EDIT_DISPLAY_BOTTOM, OnEnChangeDisplayBottom)
	ON_EN_CHANGE(EDIT_DISPLAY_RIGHT, OnEnChangeDisplayRight)
	ON_BN_CLICKED(BUTTON_CANCEL, OnBnClickedCancel)
	ON_WM_VSCROLL()
	ON_CBN_SELCHANGE(COMBO_HURT_SOUND, OnCbnSelchangeHurtSound)
	ON_CBN_SELCHANGE(COMBO_DEATH_SOUND, OnCbnSelchangeDeathSound)
	ON_CBN_SELCHANGE(COMBO_DEATH_GRAPHIC, OnCbnSelchangeDeathGraphic)
	ON_BN_CLICKED(CHECK_FLAG1, OnBnClickedFlag1)
	ON_BN_CLICKED(CHECK_FLAG2, OnBnClickedFlag2)
	ON_BN_CLICKED(CHECK_FLAG3, OnBnClickedFlag3)
	ON_BN_CLICKED(CHECK_FLAG4, OnBnClickedFlag4)
	ON_BN_CLICKED(CHECK_FLAG5, OnBnClickedFlag5)
	ON_BN_CLICKED(CHECK_FLAG6, OnBnClickedFlag6)
	ON_BN_CLICKED(CHECK_FLAG7, OnBnClickedFlag7)
	ON_BN_CLICKED(CHECK_FLAG8, OnBnClickedFlag8)
	ON_BN_CLICKED(CHECK_FLAG9, OnBnClickedFlag9)
	ON_BN_CLICKED(CHECK_FLAG10, OnBnClickedFlag10)
	ON_BN_CLICKED(CHECK_FLAG11, OnBnClickedFlag11)
	ON_BN_CLICKED(CHECK_FLAG12, OnBnClickedFlag12)
	ON_BN_CLICKED(CHECK_FLAG13, OnBnClickedFlag13)
	ON_BN_CLICKED(CHECK_FLAG14, OnBnClickedFlag14)
	ON_BN_CLICKED(CHECK_FLAG15, OnBnClickedFlag15)
	ON_BN_CLICKED(CHECK_FLAG16, OnBnClickedFlag16)
	ON_CBN_SELCHANGE(COMBO_TEST_TILESET, OnCbnSelchangeTestTileset)
END_MESSAGE_MAP()


// NPCTableEditorDlg message handlers

void NPCTableEditorDlg::OnBnClickedSave()
{
	GetDialogData();
	if (!table.save())
	{
		MessageBox("Error saving NPC table!");
		return;
	}
	change = false;
}


void NPCTableEditorDlg::OnOK()
{
	//CDialog::OnOK();//no [Enter] allowed
}

void NPCTableEditorDlg::OnCancel()
{
	if (close)
	{
		if (change)
		{
			switch(MessageBox("Do you want to save your changes?",0, MB_YESNOCANCEL))
			{
			case IDYES:
				OnBnClickedSave();
			case IDNO:
				break;
			case IDCANCEL:
				return;
				break;
			}
		}
		CDialog::OnCancel();//don't allow [Escape]
	}
}

void NPCTableEditorDlg::OnClose()
{
	close = true;
	CDialog::OnClose();
}

void NPCTableEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	Redraw(&dc);
}


void NPCTableEditorDlg::Redraw(CDC* cdc)
{
	CDC cdc2;
	CBitmap bmpFinal;
	RECT rect, rect2;
	GetDlgItem(FRAME_NPC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	cdc2.CreateCompatibleDC(cdc);
	bmpFinal.CreateBitmap(rect.right-rect.left, rect.bottom-rect.top, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp = (CBitmap *)(cdc2.SelectObject(&bmpFinal));

	CBrush brush;

	table.displaySprite((rect.right-rect.left)/2-16, (rect.bottom-rect.top)/2, record, 2, &cdc2, testBitmap, testBitmap, testBitmap);
/*/
	rect2.left = (rect.right-rect.left)/2-16;
	rect2.right = rect2.left + 32;
	rect2.top = (rect.bottom-rect.top)/2-16;
	rect2.bottom = rect2.top + 32;
	cdc2.FillSolidRect(&rect2, 0x555555);//base tile
//*/
	rect2.left = (rect.right-rect.left)/2 - GetDlgItemInt(EDIT_HIT_LEFT)*2;
	rect2.right = (rect.right-rect.left)/2 + GetDlgItemInt(EDIT_HIT_RIGHT)*2;
	rect2.top = (rect.bottom-rect.top)/2 - GetDlgItemInt(EDIT_HIT_TOP)*2+16;
	rect2.bottom = (rect.bottom-rect.top)/2 + GetDlgItemInt(EDIT_HIT_BOTTOM)*2+16;
	brush.CreateSolidBrush(0x0000ff);
	cdc2.FrameRect(&rect2, &brush);//Hit box
	brush.DeleteObject();

	rect2.left = (rect.right-rect.left)/2 - GetDlgItemInt(EDIT_DISPLAY_LEFT)*2;
	rect2.right = (rect.right-rect.left)/2 + GetDlgItemInt(EDIT_DISPLAY_RIGHT)*2;
	rect2.top = (rect.bottom-rect.top)/2 - GetDlgItemInt(EDIT_DISPLAY_TOP)*2+16;
	rect2.bottom = (rect.bottom-rect.top)/2 + GetDlgItemInt(EDIT_DISPLAY_BOTTOM)*2+16;
	brush.CreateSolidBrush(0x00ff00);
	cdc2.FrameRect(&rect2, &brush);//Hit box
	brush.DeleteObject();

	cdc->BitBlt(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, &cdc2, 0, 0, SRCCOPY);
	bmpFinal.DeleteObject();
	cdc2.DeleteDC();
}

void NPCTableEditorDlg::OnCbnSelchangeTileset()
{
	if (init)
	{
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

void NPCTableEditorDlg::OnEnChangeHitTop()
{
	if (init)
	{
		table.entity[record].hitTop = GetDlgItemInt(EDIT_HIT_TOP);
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

void NPCTableEditorDlg::OnEnChangeHitRight()
{
	if (init)
	{
		table.entity[record].hitRight = GetDlgItemInt(EDIT_HIT_RIGHT);
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

void NPCTableEditorDlg::OnEnChangeHitLeft()
{
	if (init)
	{
		table.entity[record].hitLeft = GetDlgItemInt(EDIT_HIT_LEFT);
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

void NPCTableEditorDlg::OnEnChangeHitBottom()
{
	if (init)
	{
		table.entity[record].hitBottom = GetDlgItemInt(EDIT_HIT_BOTTOM);
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

void NPCTableEditorDlg::OnEnChangeDisplayTop()
{
	if (init)
	{
		table.entity[record].hitX = GetDlgItemInt(EDIT_DISPLAY_TOP);
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

void NPCTableEditorDlg::OnEnChangeDisplayLeft()
{
	if (init)
	{
		table.entity[record].hitX = GetDlgItemInt(EDIT_DISPLAY_LEFT);
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

void NPCTableEditorDlg::OnEnChangeDisplayBottom()
{
	if (init)
	{
		table.entity[record].cenY = GetDlgItemInt(EDIT_DISPLAY_BOTTOM);
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

void NPCTableEditorDlg::OnEnChangeDisplayRight()
{
	if (init)
	{
		table.entity[record].cenX = GetDlgItemInt(EDIT_DISPLAY_RIGHT);
		change = true;
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}

BOOL NPCTableEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;
	char buffer[1024];
	char temp[32];
	SetScrollRange(SB_VERT, 0, NPC_TYPE_COUNT-1);
	SetScrollPos(SB_VERT, record);

	CComboBox* ptr;

	ptr = (CComboBox*)GetDlgItem(COMBO_TILESET);
	ptr->AddString("00 - ???");

	sprintf(buffer, "01 - %s.%s", exe.string[CS_NPC_NPCSYM_STRING], CS_DEFAULT_PBM_EXT);
	ptr->AddString(buffer);
	ptr->AddString("02 - Map Tileset");
	ptr->AddString("03 - ??? Map Tileset");
	ptr->AddString("04 - ???");
	ptr->AddString("05 - ???");
	sprintf(buffer, "06 - %s.%s", exe.string[CS_FADE_STRING], CS_DEFAULT_PBM_EXT);
	ptr->AddString(buffer);
	ptr->AddString("07 - ???");
	ptr->AddString("08 - ???");
	ptr->AddString("09 - ???");
	sprintf(buffer, "10 - %s.%s", exe.string[CS_MYCHAR_STRING], CS_DEFAULT_PBM_EXT);
	ptr->AddString(buffer);
	sprintf(buffer, "11 - %s.%s", exe.string[CS_BULLET_STRING], CS_DEFAULT_PBM_EXT);
	ptr->AddString(buffer);
	ptr->AddString("12 - ???");
	ptr->AddString("13 - ???");
	ptr->AddString("14 - ???");
	ptr->AddString("15 - ???");
	ptr->AddString("16 - ???");
	ptr->AddString("17 - ???");
	ptr->AddString("18 - ???");
	sprintf(buffer, "19 - %s.%s", exe.string[CS_CARET_STRING], CS_DEFAULT_PBM_EXT);
	ptr->AddString(buffer);
	sprintf(buffer, "20 - %s.%s", exe.string[CS_NPC_NPCSYM_STRING], CS_DEFAULT_PBM_EXT);
	ptr->AddString(buffer);
	ptr->AddString("21 - Map NPC Set 1");
	ptr->AddString("22 - Map NPC Set 2");
	sprintf(buffer, "23 - %s.%s", exe.string[CS_NPC_NPCREGU_STRING], CS_DEFAULT_PBM_EXT);
	ptr->AddString(buffer);
	ptr->AddString("24 - ???");
	ptr->AddString("25 - ???");
	ptr->AddString("26 - ???");

	//this will load both hurt and death sound effects lists
	for (i = 0; i < 2; i++)
	{
		ptr = (CComboBox*)GetDlgItem(COMBO_HURT_SOUND+i);
		ptr->AddString("00 - <nothing>");
		ptr->AddString("01 - [blip]");
		ptr->AddString("02 - Message typing");
		ptr->AddString("03 - Bonk");
		ptr->AddString("04 - Weapon switch");
		ptr->AddString("05 - ???");
		ptr->AddString("06 - Critter hop");
		ptr->AddString("07 - ???");
		ptr->AddString("08 - ???");
		ptr->AddString("09 - ???");
		ptr->AddString("10 - ???");
		ptr->AddString("11 - Door");
		ptr->AddString("12 - [*plsh*]");
		ptr->AddString("13 - ???");
		ptr->AddString("14 - Get weapon energy");
		ptr->AddString("15 - [click]");
		ptr->AddString("16 - Take damage");
		ptr->AddString("17 - Die");
		ptr->AddString("18 - [Menu?]");
		ptr->AddString("19 - ???");
		ptr->AddString("20 - Health/ammo refill");
		ptr->AddString("21 - [bubble]");
		ptr->AddString("22 - [Click]");
		ptr->AddString("23 - [Thud]");
		ptr->AddString("24 - [Tap] walking?");
		ptr->AddString("25 - Enemy killed?");
		ptr->AddString("26 - [Loud thud]");
		ptr->AddString("27 - Level up!");
		ptr->AddString("28 - [Thump]");
		ptr->AddString("29 - Teleport");
		ptr->AddString("30 - Jump");
		ptr->AddString("31 - [Ting!]");
		ptr->AddString("32 - Polar Star lvl");
		ptr->AddString("33 - Fireball");
		ptr->AddString("34 - Fireball bounce");
		ptr->AddString("35 - Explosion");
		ptr->AddString("36 - ???");
		ptr->AddString("37 - [click]");
		ptr->AddString("38 - Get item?");
		ptr->AddString("39 - [*bvng*]");
		ptr->AddString("40 - Water");
		ptr->AddString("41 - Water");
		ptr->AddString("42 - [Beep]");
		ptr->AddString("43 - [BEEP] computer");
		ptr->AddString("44 - [*KAPOW!*]");
		ptr->AddString("45 - [Ping] Weapon energy bounce");
		ptr->AddString("46 - [*ftt*] Out of ammo");
		ptr->AddString("47 - ???");
		ptr->AddString("48 - Bubble pop");
		ptr->AddString("49 - Spur level 1");
		ptr->AddString("50 - [Squeek!]");
		ptr->AddString("51 - [Squeal!]");
		ptr->AddString("52 - [ROAR!]");
		ptr->AddString("53 - [*bblblbl*]");
		ptr->AddString("54 - [Thud]");
		ptr->AddString("55 - [Squeek]");
		ptr->AddString("56 - [Splash]");
		ptr->AddString("57 - Little damage sound");
		ptr->AddString("58 - [*chlk*]");
		ptr->AddString("59 - ???");
		ptr->AddString("60 - Spur charge (lower)");
		ptr->AddString("61 - Spur charge (higher)");
		ptr->AddString("62 - Spur level 2");
		ptr->AddString("63 - Spur level 3");
		ptr->AddString("64 - Spur MAX");
		ptr->AddString("65 - Spur full?");
		ptr->AddString("66 - ???");
		ptr->AddString("67 - ???");
		ptr->AddString("68 - ???");
		ptr->AddString("69 - ???");
		ptr->AddString("70 - [Whud]");
		ptr->AddString("71 - Tiny explosion");
		ptr->AddString("72 - Small explosion");
		ptr->AddString("73 - ???");
		ptr->AddString("74 - ???");
		ptr->AddString("75 - ???");
		ptr->AddString("76 - ???");
		ptr->AddString("77 - ???");
		ptr->AddString("78 - ???");
		ptr->AddString("78 - ???");
		ptr->AddString("79 - ???");
		ptr->AddString("80 - ???");
		ptr->AddString("81 - ???");
		ptr->AddString("82 - ???");
		ptr->AddString("83 - ???");
		ptr->AddString("84 - ???");
		ptr->AddString("85 - ???");
		ptr->AddString("86 - ???");
		ptr->AddString("87 - ???");
		ptr->AddString("87 - ???");
		ptr->AddString("88 - ???");
		ptr->AddString("89 - ???");
		ptr->AddString("90 - ???");
		ptr->AddString("91 - ???");
		ptr->AddString("92 - ???"); 
		ptr->AddString("93 - ???");
		ptr->AddString("94 - ???");
		ptr->AddString("95 - ???");
		ptr->AddString("96 - ???");
		ptr->AddString("98 - ???");
		ptr->AddString("99 - ???");
		ptr->AddString("100 - ???");
		ptr->AddString("101 - ???");
		ptr->AddString("102 - ???");
		ptr->AddString("103 - ???");
		ptr->AddString("104 - ???");
		ptr->AddString("105 - ???");
		ptr->AddString("106 - ???");
		ptr->AddString("107 - ???");
		ptr->AddString("108 - ???");
		ptr->AddString("109 - ???");
		ptr->AddString("110 - ???");
		ptr->AddString("111 - ???");
		ptr->AddString("112 - ???");
		ptr->AddString("113 - ???");
		ptr->AddString("114 - ???");
		ptr->AddString("115 - ???");
		ptr->AddString("116 - ???");
		ptr->AddString("117 - ???");
		ptr->AddString("118 - ???");
		ptr->AddString("119 - ???");
		ptr->AddString("120 - ???");
		ptr->AddString("121 - ???");
		ptr->AddString("122 - ???");
		ptr->AddString("123 - ???");
		ptr->AddString("124 - ???");
		ptr->AddString("125 - ???");
		ptr->AddString("126 - ???");
		ptr->AddString("127 - ???");
		ptr->AddString("128 - ???");
		ptr->AddString("129 - ???");
		ptr->AddString("130 - ???");
		ptr->AddString("131 - ???");
		ptr->AddString("132 - ???");
		ptr->AddString("133 - ???");
		ptr->AddString("134 - ???");
		ptr->AddString("135 - ???");
		ptr->AddString("136 - ???");
		ptr->AddString("137 - ???");
		ptr->AddString("138 - ???");
		ptr->AddString("139 - ???");
		ptr->AddString("140 - ???");
		ptr->AddString("141 - ???");
		ptr->AddString("142 - ???");
		ptr->AddString("143 - ???");
		ptr->AddString("144 - ???");
		ptr->AddString("145 - ???");
		ptr->AddString("146 - ???");
		ptr->AddString("147 - ???");
		ptr->AddString("148 - ???");
		ptr->AddString("149 - ???");
		ptr->AddString("150 - ???");
		ptr->AddString("151 - ???");
		ptr->AddString("152 - ???");
		ptr->AddString("153 - ???");
		ptr->AddString("154 - ???");
		ptr->AddString("155 - ???");
		ptr->AddString("156 - ???");
		ptr->AddString("157 - ???");
		ptr->AddString("158 - ???");
		ptr->AddString("159 - ???");
		ptr->AddString("150 - ???");
		ptr->AddString("161 - ???");
		ptr->AddString("162 - ???");
		ptr->AddString("163 - ???");
		ptr->AddString("164 - ???");
		ptr->AddString("165 - ???");
		ptr->AddString("166 - ???");
		ptr->AddString("167 - ???");
		ptr->AddString("168 - ???");
		ptr->AddString("169 - ???");
		ptr->AddString("170 - ???");
		ptr->AddString("171 - ???");
		ptr->AddString("172 - ???");
		ptr->AddString("173 - ???");
		ptr->AddString("174 - ???");
		ptr->AddString("175 - ???");
		ptr->AddString("176 - ???");
		ptr->AddString("177 - ???");
		ptr->AddString("178 - ???");
		ptr->AddString("179 - ???");
		ptr->AddString("180 - ???");
		ptr->AddString("181 - ???");
		ptr->AddString("182 - ???");
		ptr->AddString("183 - ???");
		ptr->AddString("184 - ???");
		ptr->AddString("185 - ???");
		ptr->AddString("186 - ???");
		ptr->AddString("187 - ???");
		ptr->AddString("188 - ???");
		ptr->AddString("189 - ???");
		ptr->AddString("190 - ???");
		ptr->AddString("191 - ???");
		ptr->AddString("192 - ???");
		ptr->AddString("193 - ???");
		ptr->AddString("194 - ???");
		ptr->AddString("195 - ???");
		ptr->AddString("196 - ???");
		ptr->AddString("197 - ???");
		ptr->AddString("198 - ???");
		ptr->AddString("199 - ???");
		ptr->AddString("200 - ???");
		ptr->AddString("201 - ???");
		ptr->AddString("202 - ???");
		ptr->AddString("203 - ???");
		ptr->AddString("204 - ???");
		ptr->AddString("205 - ???");
		ptr->AddString("206 - ???");
		ptr->AddString("207 - ???");
		ptr->AddString("208 - ???");
		ptr->AddString("209 - ???");
		ptr->AddString("210 - ???");
		ptr->AddString("211 - ???");
		ptr->AddString("212 - ???");
		ptr->AddString("213 - ???");
		ptr->AddString("214 - ???");
		ptr->AddString("215 - ???");
		ptr->AddString("216 - ???");
		ptr->AddString("217 - ???");
		ptr->AddString("218 - ???");
		ptr->AddString("219 - ???");
		ptr->AddString("220 - ???");
		ptr->AddString("221 - ???");
		ptr->AddString("222 - ???");
		ptr->AddString("223 - ???");
		ptr->AddString("224 - ???");
		ptr->AddString("225 - ???");
		ptr->AddString("226 - ???");
		ptr->AddString("227 - ???");
		ptr->AddString("228 - ???");
		ptr->AddString("229 - ???");
		ptr->AddString("230 - ???");
		ptr->AddString("231 - ???");
		ptr->AddString("232 - ???");
		ptr->AddString("233 - ???");
		ptr->AddString("234 - ???");
		ptr->AddString("235 - ???");
		ptr->AddString("236 - ???");
		ptr->AddString("237 - ???");
		ptr->AddString("238 - ???");
		ptr->AddString("239 - ???");
		ptr->AddString("240 - ???");
		ptr->AddString("241 - ???");
		ptr->AddString("242 - ???");
		ptr->AddString("243 - ???");
		ptr->AddString("244 - ???");
		ptr->AddString("245 - ???");
		ptr->AddString("246 - ???");
		ptr->AddString("247 - ???");
		ptr->AddString("248 - ???");
		ptr->AddString("249 - ???");
		ptr->AddString("250 - ???");
		ptr->AddString("251 - ???");
		ptr->AddString("252 - ???");
		ptr->AddString("253 - ???");
		ptr->AddString("254 - ???");
		ptr->AddString("255 - ???");
	}

	ptr = (CComboBox*)GetDlgItem(COMBO_DEATH_GRAPHIC);
	ptr->AddString("00 - Nothing");
	ptr->AddString("01 - Small Cloud");
	ptr->AddString("02 - Medium Cloud");
	ptr->AddString("03 - Large Cloud");

	ptr = (CComboBox*)GetDlgItem(COMBO_TEST_TILESET);

	sprintf(buffer, "%sdata\\Npc\\", filePath);//which path to .\data\Stage\ 
	::SetCurrentDirectory(buffer);
	sprintf(temp, "Npc*.%s", CS_DEFAULT_PBM_EXT);
	ptr->Dir(0x0010, temp);//get list of tileset files
	i = ptr->GetCount();
	for (; i > 0; i--)//removes the "Prt" and ".pbm" from names in list
	{
		ptr->GetLBText(0, temp);
		temp[strlen(temp)-4] = '\0';
		strcpy(buffer, &(temp[3]));
		ptr->DeleteString(0);
		ptr->AddString(buffer);
	};
	ptr->SetCurSel(0);
	::SetCurrentDirectory(filePath);

	ptr->GetLBText(ptr->GetCurSel(), temp);
	sprintf(buffer, ".\\data\\Npc\\Npc%s.%s", temp, CS_DEFAULT_PBM_EXT);

	testBitmap.Load(buffer);

	DisplayRecord();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void NPCTableEditorDlg::DisplayRecord()
{
	init = false;
	char buffer[256];
	sprintf(buffer, "NPC Table Editor - Entity %i", record);
	SetWindowText(buffer);

	SetDlgItemInt(EDIT_HP, table.entity[record].hp, FALSE);
	SetDlgItemInt(EDIT_ATTACK, table.entity[record].attack, FALSE);
	SetDlgItemInt(EDIT_XP, table.entity[record].weaponDrop, FALSE);

	SetDlgItemInt(EDIT_HIT_TOP, table.entity[record].hitTop, FALSE);
	SetDlgItemInt(EDIT_HIT_BOTTOM, table.entity[record].hitBottom, FALSE);
	SetDlgItemInt(EDIT_HIT_LEFT, table.entity[record].hitLeft, FALSE);
	SetDlgItemInt(EDIT_HIT_RIGHT, table.entity[record].hitRight, FALSE);

	SetDlgItemInt(EDIT_DISPLAY_TOP, table.entity[record].hitY, FALSE);
	SetDlgItemInt(EDIT_DISPLAY_BOTTOM, table.entity[record].cenY, FALSE);
	SetDlgItemInt(EDIT_DISPLAY_LEFT, table.entity[record].hitX, FALSE);
	SetDlgItemInt(EDIT_DISPLAY_RIGHT, table.entity[record].cenX, FALSE);

	CheckDlgButton(CHECK_FLAG1, 0x0001 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG2, 0x0002 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG3, 0x0004 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG4, 0x0008 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG5, 0x0010 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG6, 0x0020 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG7, 0x0040 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG8, 0x0080 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG9, 0x0100 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG10, 0x0200 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG11, 0x0400 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG12, 0x0800 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG13, 0x1000 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG14, 0x2000 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG15, 0x4000 & table.entity[record].flags);
	CheckDlgButton(CHECK_FLAG16, 0x8000 & table.entity[record].flags);

	((CComboBox*)GetDlgItem(COMBO_TILESET))->SetCurSel(table.entity[record].tileSet);
	((CComboBox*)GetDlgItem(COMBO_HURT_SOUND))->SetCurSel(table.entity[record].hurtSound);
	((CComboBox*)GetDlgItem(COMBO_DEATH_SOUND))->SetCurSel(table.entity[record].deathSound);
	((CComboBox*)GetDlgItem(COMBO_DEATH_GRAPHIC))->SetCurSel(table.entity[record].deathPBM);
//	SetDlgItemInt(EDIT_, table.entity[record], FALSE);

	SetDlgItemText(EDIT_DESC, table.extra[record].desc.GetBuffer());
	SetDlgItemText(EDIT_DESC1, table.extra[record].shortDesc1.GetBuffer());
	SetDlgItemText(EDIT_DESC2, table.extra[record].shortDesc2.GetBuffer());
	SetDlgItemText(EDIT_OPTION1, table.extra[record].option1.GetBuffer());
	SetDlgItemText(EDIT_OPTION2, table.extra[record].option2.GetBuffer());

	CDC* cdc = GetDC();
	Redraw(cdc);
	ReleaseDC(cdc);
	init = true;
}


void NPCTableEditorDlg::OnBnClickedCancel()
{
	close = true;
	CDialog::OnClose();
	CDialog::OnCancel();
}

void NPCTableEditorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	GetDialogData();
	switch (nSBCode)
	{
/*
SB_BOTTOM
SB_ENDSCROLL
SB_TOP
*/
	case SB_LINEUP:
//	case SB_LEFT:
		record = max(0, record-1);
		SetScrollPos(SB_VERT, record);
		break;
	case SB_PAGEUP:
		record = max(0, record - 10);
		SetScrollPos(SB_VERT, record);
		break;
	case SB_PAGEDOWN:
		record = min(NPC_TYPE_COUNT-1, record + 10);
		SetScrollPos(SB_VERT, record);
		break;
	case SB_LINEDOWN:
//	case SB_RIGHT:
		record = min(NPC_TYPE_COUNT-1, record+1);
		SetScrollPos(SB_VERT, record);
		break;
	case SB_THUMBPOSITION:
		record = nPos;
		SetScrollPos(SB_VERT, record);
		break;
	case SB_THUMBTRACK:
		record = nPos;
		SetScrollPos(SB_VERT, record);
		break;
	}
	DisplayRecord();

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void NPCTableEditorDlg::GetDialogData()
{
	table.entity[record].hp = GetDlgItemInt(EDIT_HP, NULL, FALSE);
	table.entity[record].attack = GetDlgItemInt(EDIT_ATTACK, NULL, FALSE);
	table.entity[record].weaponDrop = GetDlgItemInt(EDIT_XP, NULL, FALSE);

	table.entity[record].hitBottom = GetDlgItemInt(EDIT_HIT_BOTTOM, NULL, FALSE);
	table.entity[record].hitLeft = GetDlgItemInt(EDIT_HIT_LEFT, NULL, FALSE);
	table.entity[record].hitRight = GetDlgItemInt(EDIT_HIT_RIGHT, NULL, FALSE);
	table.entity[record].hitTop = GetDlgItemInt(EDIT_HIT_TOP, NULL, FALSE);

	table.entity[record].hitX = GetDlgItemInt(EDIT_DISPLAY_LEFT, NULL, FALSE);
	table.entity[record].hitY = GetDlgItemInt(EDIT_DISPLAY_TOP, NULL, FALSE);
	table.entity[record].cenX = GetDlgItemInt(EDIT_DISPLAY_RIGHT, NULL, FALSE);
	table.entity[record].cenY = GetDlgItemInt(EDIT_DISPLAY_BOTTOM, NULL, FALSE);

	unsigned short flags;
	flags = 0;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG1))->GetState() & 0x0001) << 0;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG2))->GetState() & 0x0001) << 1;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG3))->GetState() & 0x0001) << 2;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG4))->GetState() & 0x0001) << 3;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG5))->GetState() & 0x0001) << 4;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG6))->GetState() & 0x0001) << 5;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG7))->GetState() & 0x0001) << 6;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG8))->GetState() & 0x0001) << 7;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG9))->GetState() & 0x0001) << 8;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG10))->GetState() & 0x0001) << 9;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG11))->GetState() & 0x0001) << 10;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG12))->GetState() & 0x0001) << 11;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG13))->GetState() & 0x0001) << 12;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG14))->GetState() & 0x0001) << 13;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG15))->GetState() & 0x0001) << 14;
	flags += (((CButton*)GetDlgItem(CHECK_FLAG16))->GetState() & 0x0001) << 15;

	table.entity[record].flags = flags;// = GetDlgItemInt(EDIT_, NULL, FALSE);

	table.entity[record].deathPBM = ((CComboBox*)GetDlgItem(COMBO_DEATH_GRAPHIC))->GetCurSel();
	table.entity[record].deathSound = ((CComboBox*)GetDlgItem(COMBO_DEATH_SOUND))->GetCurSel();
	table.entity[record].hurtSound = ((CComboBox*)GetDlgItem(COMBO_HURT_SOUND))->GetCurSel();
	table.entity[record].tileSet = ((CComboBox*)GetDlgItem(COMBO_TILESET))->GetCurSel();

	GetDlgItemText(EDIT_DESC, table.extra[record].desc.GetBuffer(), 255);
	GetDlgItemText(EDIT_DESC1, table.extra[record].shortDesc1.GetBuffer(), 15);
	GetDlgItemText(EDIT_DESC2, table.extra[record].shortDesc2.GetBuffer(), 15);
	GetDlgItemText(EDIT_OPTION1, table.extra[record].option1.GetBuffer(), 31);
	GetDlgItemText(EDIT_OPTION2, table.extra[record].option2.GetBuffer(), 31);
}


void NPCTableEditorDlg::OnCbnSelchangeHurtSound()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnCbnSelchangeDeathSound()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnCbnSelchangeDeathGraphic()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag1()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag2()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag3()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag4()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag5()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag6()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag7()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag8()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag9()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag10()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag11()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag12()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag13()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag14()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag15()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnBnClickedFlag16()
{
	if (init)
		change = true;
}

void NPCTableEditorDlg::OnCbnSelchangeTestTileset()
{
	CComboBox* ptr = (CComboBox*)GetDlgItem(COMBO_TEST_TILESET);
	
	char buffer[256], buf[32];
	ptr->GetLBText(ptr->GetCurSel(), buf);
	sprintf(buffer, ".\\data\\Npc\\Npc%s.%s", buf, CS_DEFAULT_PBM_EXT);

	testBitmap.Load(buffer);
	if (init)
	{
		CDC* cdc = GetDC();
		Redraw(cdc);
		ReleaseDC(cdc);
	}
}
