// CaveMapEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "General.h"
#include "CaveEditor.h"

//#include "SharedWindows.h"

#include "CaveMapInfoDlg.h"
#include "CaveMapPropertiesDlg.h"
#include "ScriptEditorDlg.h"

#include "CaveMapEditDlg.h"
#include ".\cavemapeditdlg.h"

#include "CaveEditorDlg.h"

#include "ResizeDlg.h"

#include "math.h"

extern CCaveEditorDlg* mainWindow;
extern ScriptEditorDlg scriptEditorDlg;

#define ENTITY_SORT_CATEGORY           0
#define ENTITY_SORT_ALPHA              1
#define ENTITY_SORT_ID                 2

#define ENTITY_COLOR            0x00ff00
#define ENTITY_SELECTED_COLOR   0xffff00
#define GRID_COLOR              0x888888
#define BORDER_COLOR            0xff00ff

#define COLOR_MASK              0xffffff

#define UNDO_ENTITY                    0
#define UNDO_MAP                       1

#define UNDO_ENTITY_XY                 0
#define UNDO_ENTITY_NPC                1
#define UNDO_ENTITY_FLAGS              2
#define UNDO_ENTITY_FLAGID             3
#define UNDO_ENTITY_EVENT              4
#define UNDO_ENTITY_DELETE             5
#define UNDO_ENTITY_INSERT             6

Stack::Stack()
{
	ePtr = NULL;
	mPtr = NULL;
}


Stack::~Stack()
{
	if (mPtr != NULL)
		clean(mPtr->tiles);

	clean(mPtr);
	clean(ePtr);
}


// CaveMapEditDlg dialog

IMPLEMENT_DYNAMIC(CaveMapEditDlg, CDialog)
CaveMapEditDlg::CaveMapEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CaveMapEditDlg::IDD, pParent),
	tileWindow(this),
	m_MapInfo(this),
	m_EntityInfo(this)
{
	scale = 1;
	offX = 0;
	offY = 0;
	init = false;
	changes = false;

	copyEntity.event = 0;
	copyEntity.flagID = 0;
	copyEntity.flags = 0;
	copyEntity.npc = 0;
	copyEntity.x = 0;
	copyEntity.y = 0;

	entitySort = ENTITY_SORT_CATEGORY;
}

CaveMapEditDlg::~CaveMapEditDlg()
{
	if (bmpMem[0].m_hObject != NULL)
			bmpMem[0].DeleteObject();
	if (bmpMem[1].m_hObject != NULL)
			bmpMem[1].DeleteObject();
	if (bmpMem[2].m_hObject != NULL)
			bmpMem[2].DeleteObject();
	if (bmpMem[3].m_hObject != NULL)
			bmpMem[3].DeleteObject();
	if (bmpMem[4].m_hObject != NULL)
			bmpMem[4].DeleteObject();

	popup.DestroyMenu();
}

void CaveMapEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, SCROLLBAR_VERT, m_ScrollVert);
	DDX_Control(pDX, SCROLLBAR_HORZ, m_ScrollHorz);
	DDX_Control(pDX, FRAME_MAP, m_ViewFrame);
}


BEGIN_MESSAGE_MAP(CaveMapEditDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(CHECK_FRONT_TILES, OnBnClickedFrontTiles)
	ON_BN_CLICKED(CHECK_BACK_TILES, OnBnClickedBackTiles)
	ON_BN_CLICKED(CHECK_BACKGROUND, OnBnClickedBackground)
	ON_BN_CLICKED(RADIO_ENTITY, OnBnClickedEntity)
	ON_BN_CLICKED(RADIO_MAP, OnBnClickedMap)
	ON_BN_CLICKED(CHECK_ENTITIES, OnBnClickedEntities)
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(BUTTON_PROPERTIES, OnBnClickedProperties)
	ON_BN_CLICKED(CHECK_TILE_GRID, OnBnClickedTileGrid)
	ON_BN_CLICKED(CHECK_TILE_TYPES, OnBnClickedTileTypes)
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(BUTTON_MAP_SCRIPT, OnBnClickedMapScript)
	ON_BN_CLICKED(BUTTON_SAVE, OnBnClickedSave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MAP_SHIFTMAP, OnMapShiftmap)
	ON_COMMAND(ID_ENTITY_ADDENTITY, OnEntityAddentity)
	ON_COMMAND(ID_ENTITY_DELETEENTITY, OnEntityDeleteentity)
	ON_COMMAND(ID_ENTITY_COPYENTITY, OnEntityCopyentity)
	ON_COMMAND(ID_ENTITY_PASTEENTITY, OnEntityPasteentity)
	ON_COMMAND(ID_ZOOM_05X, OnZoom05x)
	ON_COMMAND(ID_ZOOM_1X, OnZoom1x)
	ON_COMMAND(ID_ZOOM_2X, OnZoom2x)
	ON_COMMAND(ID_ZOOM_3X, OnZoom3x)
	ON_COMMAND(ID_ZOOM_4X, OnZoom4x)
	ON_COMMAND(ID_MAP_EDITSCRIPT, OnMapEditscript)
	ON_COMMAND(ID_MAP_PROPERTIES, OnMapProperties)
	ON_COMMAND(ID_MAP_SAVE, OnMapSave)
	ON_WM_MOVE()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_COMMAND(ID_MAP_SETSTARTLOCATION, OnMapSetstartlocation)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(CHECK_ENTITY_SPRITES, OnBnClickedEntitySprites)
	ON_BN_CLICKED(CHECK_ENTITY_TEXT, OnBnClickedEntityText)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_SORTENTITIIESBY_IDNUMBER, OnSortentitiiesbyIdnumber)
	ON_COMMAND(ID_SORTENTITIIESBY_CATEGORY, OnSortentitiiesbyCategory)
	ON_COMMAND(ID_SORTENTITIIESBY_NAME, OnSortentitiiesbyName)
END_MESSAGE_MAP()


// CaveMapEditDlg message handlers
void CaveMapEditDlg::OnPaint() 
{
	int x, y;
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		x = (rect.Width() - cxIcon + 1) / 2;
		y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		return;
	}

	CPaintDC dc(this); // device context for painting

	RedrawMap(dc);

	CDialog::OnPaint();
}

void CaveMapEditDlg::Refresh(bool regen)
{
	CDC* cdc = GetDC();
	RedrawMap(*cdc, regen);
	ReleaseDC(cdc);
}


void CaveMapEditDlg::RedrawMap(CDC &dc, bool regen)
{
	int i, j, w, h;
	RECT rect;

	CDC cdc;
	cdc.CreateCompatibleDC(&dc);
	CDC cdc2;
	cdc2.CreateCompatibleDC(&dc);

	CBitmap* pOldBmp;
	CBitmap* pOldBmp2;
	CBitmap bmpFinal;

	GetDlgItem(FRAME_MAP)->GetClientRect(&rect);
	GetDlgItem(FRAME_MAP)->ClientToScreen(&rect);
	ScreenToClient(&rect);

	//used for triple buffering.  I think that's what this is...
	bmpFinal.CreateBitmap(rect.right-rect.left, rect.bottom-rect.top, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);

	pOldBmp2 = (CBitmap *)(cdc2.SelectObject(&bmpFinal));


	w = min(int((rect.right-rect.left)/scale), caveMap.width*16);
	h = min(int((rect.bottom-rect.top)/scale), caveMap.height*16);
	if (w + offX > caveMap.width*16)
		w = caveMap.width*16 - offX;
	if (h + offY > caveMap.height*16)
		h = caveMap.height*16 - offY;

	cdc2.FillSolidRect(0, 0, rect.right-rect.left, rect.bottom-rect.top, 0x000000);
	cdc2.FillSolidRect(0, 0, int(w*scale+1), int(h*scale+1), BORDER_COLOR);
	cdc2.FillSolidRect(0, 0, int(w*scale), int(h*scale), 0x000000);

	BLENDFUNCTION blend;

	if (CS_GRAPHIC_COLOR_DEPTH == 32)
		blend.AlphaFormat = AC_SRC_ALPHA;
	else
		blend.AlphaFormat = 0;

	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = CS_DEFAULT_TILETYPE_ALPHA;

	for (i = 0; i < MAP_DISPLAY_LAST; i++)
	{
		// select the bitmap to memory dc
		pOldBmp = (CBitmap *)(cdc.SelectObject(&bmpMem[i]));

		// ***Start Drawing***
		if (regen)
		{
			switch (i)
			{
			case MAP_BACKGROUND:
				cdc.FillSolidRect(0, 0, caveMap.width*16, caveMap.height*16, 0x000000);
				caveMap.displayBackground(&cdc);
				break;
			case MAP_BACK_TILES:
				cdc.FillSolidRect(0, 0, caveMap.width*16, caveMap.height*16, 0x000000);
				caveMap.displayBackMap(&cdc);
				break;
			case MAP_FRONT_TILES:
				cdc.FillSolidRect(0, 0, caveMap.width*16, caveMap.height*16, 0x000000);
				caveMap.displayFrontMap(&cdc);
				break;
			case MAP_TILE_TYPES:
				cdc.FillSolidRect(0, 0, caveMap.width*16, caveMap.height*16, 0x000000);
				caveMap.displayTileTypes(&cdc);
				break;
			}
		}

		if (i == 3 && IsDlgButtonChecked(CHECK_TILE_TYPES) && caveMap.tileSet.typeMap.GetBPP() == 32)
			cdc2.AlphaBlend(0, 0, int(w*scale), int(h*scale), &cdc, offX, offY, w, h, blend);
		else
		if (i == 0 && IsDlgButtonChecked(CHECK_BACKGROUND)
			|| i == 1 && IsDlgButtonChecked(CHECK_BACK_TILES)
			|| i == 2 && IsDlgButtonChecked(CHECK_FRONT_TILES)
			|| i == 3 && IsDlgButtonChecked(CHECK_TILE_TYPES))
		{
			//places various parts of the map together
			cdc2.TransparentBlt(0, 0, int(w*scale), int(h*scale), &cdc, offX, offY, w, h, 0);
		}

		//remove created bitmap and release memory
		cdc.SelectObject(pOldBmp);

	}//end for loop

	RECT entRect;
	CBrush brush;
	if (IsDlgButtonChecked(CHECK_TILE_GRID) && scale >= 1)//don't show grid when zoomed far out
	{
		brush.CreateSolidBrush(GRID_COLOR);
		for (i = 0; i < caveMap.width; i++)
			for (j = 0; j < caveMap.height; j++)
			{
				entRect.left = int((i*16-offX)*scale);
				entRect.top = int((j*16-offY)*scale);
				entRect.right = int((i*16+16-offX)*scale+1);
				entRect.bottom = int((j*16+16-offY)*scale+1);
				cdc2.FrameRect(&entRect, &brush);
			}
		brush.DeleteObject();
	}

//*
	// *** Draw Entity Sprites *** 
	if (IsDlgButtonChecked(CHECK_ENTITY_SPRITES))
	{
		for (j = 0; j < caveMap.entityNum; j++)
		{
			DisplayEntitySprite(cdc2, j);
		}
	}
//*/

	// *** Draw Entities *** 
	if (IsDlgButtonChecked(CHECK_ENTITIES) || IsDlgButtonChecked(CHECK_ENTITY_TEXT))
	{
		CFont font;
		font.CreatePointFont(scale*40, "Arial Narrow", &cdc2);
		CFont* oldFont = cdc2.SelectObject(&font);
		npcTable;
		//then show entities
		cdc2.SetBkMode(TRANSPARENT);//this way there's no box around the text
		cdc2.SetTextColor(0xcccccc);
		brush.CreateSolidBrush(ENTITY_COLOR);
		for (j = 0; j < caveMap.entityNum; j++)
		{
			DisplayEntity(cdc2, brush, j);
		}
		oldFont = cdc2.SelectObject(oldFont);
		font.DeleteObject();

		if (selectedEntity != -1)
		{
			//then highlight selected entity. This prevents selected entity from being hidden under an unselected one
			brush.DeleteObject();
			brush.CreateSolidBrush(ENTITY_SELECTED_COLOR);
			entRect.left = int(caveMap.entity[selectedEntity].x*scale*16 - offX*scale);
			entRect.top = int(caveMap.entity[selectedEntity].y*scale*16 - offY*scale);
			entRect.right = int(entRect.left+scale*16);
			entRect.bottom = int(entRect.top+scale*16);
			cdc2.FrameRect(&entRect, &brush);
		}
		brush.DeleteObject();
	}

	//finally draw everything to the screen
	blend.AlphaFormat = 0;
	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	dc.AlphaBlend(rect.left,rect.top, rect.right-rect.left, rect.bottom-rect.top, &cdc2, 0,0, rect.right-rect.left, rect.bottom-rect.top, blend);

	redraw = true;

	//final cleanup
	(cdc2.SelectObject(&pOldBmp2));
	bmpFinal.DeleteObject();
	cdc.DeleteDC();
	cdc2.DeleteDC();
}

void CaveMapEditDlg::DisplayEntitySprite(CDC& cdc2, int index)
{
	int type = caveMap.entity[index].npc;
	if (exe.eRectCount[type] == 0)
		return;
	if (exe.eRectDisplay[type][0].bottom == 0 || exe.eRectDisplay[type][0].right == 0)
		return;

	CDC cdc;
	cdc.CreateCompatibleDC(&cdc2);
	CBitmap* pOldBmp;
	CBitmap bmpEntity;

	RECT rect;
	RECT rect2;

	bmpEntity.CreateBitmap(512, 512, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	pOldBmp = (CBitmap *)(cdc.SelectObject(&bmpEntity));

	int x = int((caveMap.entity[index].x*16+8-npcTable.entity[type].hitX - offX)*scale);
	int y = int((caveMap.entity[index].y*16+8-npcTable.entity[type].hitY - offY)*scale);

	rect.left = x-(0)*scale;
	rect.top = y-(0)*scale;
	rect.right = x+(exe.eRectDisplay[type][0].right-exe.eRectDisplay[type][0].left)*scale;
	rect.bottom = y+(exe.eRectDisplay[type][0].bottom-exe.eRectDisplay[type][0].top)*scale;

	rect2.left = 0;
	rect2.top = 0;
	rect2.right = rect.right-rect.left;
	rect2.bottom = rect.bottom-rect.top;

	switch (npcTable.entity[type].tileSet)
	{
	case 0:
	case 1://Title.pbm
		exe.spriteSheet[1].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 2://Map Tileset
		caveMap.tileSet.tileData.DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
//		exe.spriteSheet[2].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 3://Map Tileset
		caveMap.tileSet.tileData.DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
//		exe.spriteSheet[3].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 4:
	case 5:
	case 6://Fade.pbm
		exe.spriteSheet[6].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 7:
	case 8:
	case 9:
	case 10://MyChar.pbm
		exe.spriteSheet[10].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 11://Bullet.pbm
		exe.spriteSheet[11].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19://Caret.pbm
		exe.spriteSheet[19].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 20://NpcSym.pbm
		exe.spriteSheet[20].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 21://Map NPC Set 1
		caveMap.NPCset1.DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 22://Map NPC Set 2
		caveMap.NPCset2.DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 23://NpcRegu.pbm
		exe.spriteSheet[23].DrawPart(exe.eRectDisplay[type][0], &cdc, rect2);
		break;
	case 24:
	case 25:
	case 26:
		break;
	};

	//finally draw everything to cdc2
	BLENDFUNCTION blend;

	blend.AlphaFormat = 0;
	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	cdc2.TransparentBlt(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, &cdc,
			    rect2.left, rect2.top, rect2.right-rect2.left, rect2.bottom-rect2.top, 0);
//	cdc2.AlphaBlend(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, &cdc, 0,0, rect2.right, rect2.bottom, blend);

	(cdc.SelectObject(&pOldBmp));
	bmpEntity.DeleteObject();
}

void CaveMapEditDlg::DisplayEntity(CDC& cdc2, CBrush& brush, int index)
{
	RECT entRect;

	npcTable.entity[index];

	int x = int((caveMap.entity[index].x*16+8 - offX)*scale);
	int y = int((caveMap.entity[index].y*16+8 - offY)*scale);

	entRect.left = x-8*scale;
	entRect.top = y-8*scale;
	entRect.right = x+8*scale;
	entRect.bottom = y+8*scale;;
	if (IsDlgButtonChecked(CHECK_ENTITIES))
		cdc2.FrameRect(&entRect, &brush);
	entRect.right--;
	entRect.bottom--;

	if (caveMap.entity[index].npc >= 0 && npcTable.extra[caveMap.entity[index].npc].loaded && IsDlgButtonChecked(CHECK_ENTITY_TEXT))
	{
		//add a slight shadow effect to added readability
		cdc2.SetTextColor(0x000000);
		cdc2.ExtTextOut(entRect.left+0.5*scale, entRect.top+0.5*scale, ETO_CLIPPED, &entRect, npcTable.extra[caveMap.entity[index].npc].shortDesc1, strlen(npcTable.extra[caveMap.entity[index].npc].shortDesc1), NULL);
		entRect.top += scale*8;//start halfway down entity box
		cdc2.ExtTextOut(entRect.left+0.5*scale, entRect.top+0.5*scale, ETO_CLIPPED, &entRect, npcTable.extra[caveMap.entity[index].npc].shortDesc2, strlen(npcTable.extra[caveMap.entity[index].npc].shortDesc2), NULL);

		//write text
		entRect.top -= scale*8;//back to top of entity box
		cdc2.SetTextColor(0xcccccc);
		cdc2.ExtTextOut(entRect.left, entRect.top, ETO_CLIPPED, &entRect, npcTable.extra[caveMap.entity[index].npc].shortDesc1, strlen(npcTable.extra[caveMap.entity[index].npc].shortDesc1), NULL);
		entRect.top += scale*8;//start halfway down entity box
		cdc2.ExtTextOut(entRect.left, entRect.top, ETO_CLIPPED, &entRect, npcTable.extra[caveMap.entity[index].npc].shortDesc2, strlen(npcTable.extra[caveMap.entity[index].npc].shortDesc2), NULL);
	}
}


void CaveMapEditDlg::reloadMap()
{
	char buffer[100];

	sprintf(buffer, "Map Editor - %s (%s)", caveMap.mapInfo.caption, caveMap.mapInfo.fileName);
	SetWindowText(buffer);

	caveMap.reload();
	offX = 0;
	offY = 0;

	CRect rect;
	GetDlgItem(FRAME_MAP)->GetClientRect(&rect);

	UpdateScrollBars();

	// create a compatible bitmap and select it to memory DC
	// so that we can draw on compatible DC, create a bitmap
	CDC* cdc;
	cdc = GetDC();
	if (bmpMem[0].m_hObject != NULL)
			bmpMem[0].DeleteObject();
	if (bmpMem[1].m_hObject != NULL)
			bmpMem[1].DeleteObject();
	if (bmpMem[2].m_hObject != NULL)
			bmpMem[2].DeleteObject();
	if (bmpMem[3].m_hObject != NULL)
			bmpMem[3].DeleteObject();
	if (bmpMem[4].m_hObject != NULL)
			bmpMem[4].DeleteObject();

	bmpMem[0].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[1].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[2].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[3].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[4].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);

	tileWindow.loadTiles(&caveMap.tileSet);
	RedrawMap(*cdc, true);
	ReleaseDC(cdc);
}

bool CaveMapEditDlg::loadMap(CaveMapINFO* info, int index)
{
	if (!init)
	{
		if (!npcTable.load())
		{
			MessageBox("Error Loading CE_NPC.txt or npc.tbl!", "Error!");
			return false;
		}
	}
	if (!caveMap.load(info))
	{
		MessageBox("Error Loading map!", "Error!");
		return false;
	};


	init = true;
	scale = CS_DEFAULT_SCALE;
	mapIndex = index;
	rclick = false;
	lclick = false;
	movingEntity = false;
	selectedEntity = -1;
	m_EntityInfo.clear();
	input = false;
	esc = true;
	move = false;
	changes = false;
	oldXC = -1;
	oldYC = -1;

	undoPtr = 0;
	undoStart = 0;
	undoEnd = 0;

	GetMenu()->EnableMenuItem(ID_EDIT_UNDO, MF_BYCOMMAND | MF_GRAYED);
	GetMenu()->EnableMenuItem(ID_EDIT_REDO, MF_BYCOMMAND | MF_GRAYED);
	//for some reason the "Entity - " text is appearing when its not supposed to during loadMap()
	if (IsDlgButtonChecked(RADIO_MAP))
		SetDlgItemText(TEXT_INFO, "Info");
	else
		SetDlgItemText(TEXT_INFO, "Entity - ");

	char buffer[100];
	SetIcon(m_hIcon, TRUE);		 // Set big icon
	SetIcon(m_hIcon, TRUE);	   // Set small icon

	sprintf(buffer, "Map Editor - %s (%s)", info->caption, info->fileName);
	SetWindowText(buffer);

	offX = 0;
	offY = 0;

	CRect rect;

	GetDlgItem(FRAME_MAP)->GetClientRect(&rect);

	UpdateScrollBars();

	// create a compatible bitmap and select it to memory DC
	// so that we can draw on compatible DC, create a bitmap
	CDC* cdc;
	cdc = GetDC();
	if (bmpMem[0].m_hObject != NULL)
			bmpMem[0].DeleteObject();
	if (bmpMem[1].m_hObject != NULL)
			bmpMem[1].DeleteObject();
	if (bmpMem[2].m_hObject != NULL)
			bmpMem[2].DeleteObject();
	if (bmpMem[3].m_hObject != NULL)
			bmpMem[3].DeleteObject();
	if (bmpMem[4].m_hObject != NULL)
			bmpMem[4].DeleteObject();

	bmpMem[0].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[1].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[2].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[3].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[4].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);

//	bmpMem[4].CreateBitmap(2600, 1200, 1, GRAPHIC_COLOR_DEPTH, NULL);
//	view.SetBitmap((HBITMAP)bmpMem[4]);

	tileWindow.loadTiles(&caveMap.tileSet);
	RedrawMap(*cdc, true);
	ReleaseDC(cdc);
	return true;
}


void CaveMapEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect rect;
	GetWindowRect(&rect);

	if (init)
	{
		GetDlgItem(FRAME_MAP)->GetWindowRect(&rect);
		ScreenToClient(&rect);//gets map display rectangle relative to main window area
		rect.right = cx-18;
		rect.bottom = cy-18;
		GetDlgItem(FRAME_MAP)->MoveWindow(&rect);

		GetDlgItem(SCROLLBAR_VERT)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.left = cx-18;
		rect.right = cx;
		rect.bottom = cy-18;
		GetDlgItem(SCROLLBAR_VERT)->MoveWindow(&rect);

		GetDlgItem(SCROLLBAR_HORZ)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right = cx-18;
		rect.top = cy-18;
		rect.bottom = cy;
		GetDlgItem(SCROLLBAR_HORZ)->MoveWindow(&rect);
		UpdateScrollBars();
	}
}

void CaveMapEditDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	int i = 0;
	int j = 0;
	char buffer [256];
	char buffer2 [256];
	CRect rect;
	CRgn rgn, rgn2;
	CDC* cdc;


	if ((nFlags & MK_LBUTTON) == MK_LBUTTON)//do mouse down first
	{
		move = true;
		OnLButtonDown(nFlags, point);
		move = false;
	}
	else
		input = true;

	//First draw a custom cursor to show which tile the mouse is over
	cdc = view.GetDC();//everthing is done with view's DC to prevent drawing out of bounds
	//first remove old cursor if it's still on screen
	if (!redraw)
	{

		if (IsDlgButtonChecked(RADIO_MAP))//ignore if in entity edit mode
		{
			switch (m_MapInfo.GetCheckedRadioButton(RADIO_DRAW_TILES, RADIO_REPLACE_TILES))
			{
			case RADIO_DRAW_TILES:
				i = tileWindow.x2 - tileWindow.x;//account for tile block
				j = tileWindow.y2 - tileWindow.y;//account for tile block
				break;
			case RADIO_FILL_TILES:
				i = tileWindow.x2 - tileWindow.x;//account for tile block
				j = tileWindow.y2 - tileWindow.y;//account for tile block
				break;
			case RADIO_RECT_TILES:
				if (oldXC < 0)
					i = 0;
				else
					i = (oldXC - xCursor);
				if (oldYC < 0)
					j = 0;
				else
					j = (oldYC - yCursor);
				break;
			case RADIO_REPLACE_TILES:
				i = tileWindow.x2 - tileWindow.x;//account for tile block
				j = tileWindow.y2 - tileWindow.y;//account for tile block
				break;
			};
		}

		if (i < 0)
		{
			rect.left = ((xCursor+i)*16-offX)*scale;
			rect.right = ((xCursor+1)*16-offX)*scale;
		}
		else
		{
			rect.left = (xCursor*16-offX)*scale;
			rect.right = ((xCursor+i+1)*16-offX)*scale;
		}

		if (j < 0)
		{
			rect.top = ((yCursor+j)*16-offY)*scale;
			rect.bottom = ((yCursor+1)*16-offY)*scale;
		}
		else
		{
			rect.top = (yCursor*16-offY)*scale;
			rect.bottom = ((yCursor+j+1)*16-offY)*scale;
		}

		rgn.CreateRectRgnIndirect(&rect);

		rect.bottom--;
		rect.top++;
		rect.left++;
		rect.right--;
		rgn2.CreateRectRgnIndirect(&rect);
		rgn.CombineRgn(&rgn, &rgn2, RGN_XOR);

		cdc->InvertRgn(&rgn);
		rgn.DeleteObject();
		rgn2.DeleteObject();
	}

	//then draw new one
	GetDlgItem(FRAME_MAP)->GetWindowRect(&rect);
	ScreenToClient(&rect);//gets map display rectangle relative to main window area

	xCursor = floor((point.x-rect.left)/16.0/scale+offX/16.0);
	yCursor = floor((point.y-rect.top)/16.0/scale+offY/16.0);


	if (IsDlgButtonChecked(RADIO_MAP))//ignore if in entity edit mode
	{
		switch (m_MapInfo.GetCheckedRadioButton(RADIO_DRAW_TILES, RADIO_REPLACE_TILES))
		{
		case RADIO_DRAW_TILES:
			i = tileWindow.x2 - tileWindow.x;//account for tile block
			j = tileWindow.y2 - tileWindow.y;//account for tile block
			break;
		case RADIO_FILL_TILES:
			i = tileWindow.x2 - tileWindow.x;//account for tile block
			j = tileWindow.y2 - tileWindow.y;//account for tile block
			break;
		case RADIO_RECT_TILES:
			if (oldXC < 0)
				i = 0;
			else
				i = (oldXC - xCursor);
			if (oldYC < 0)
				j = 0;
			else
				j = (oldYC - yCursor);
			break;
		case RADIO_REPLACE_TILES:
			i = tileWindow.x2 - tileWindow.x;//account for tile block
			j = tileWindow.y2 - tileWindow.y;//account for tile block
			break;
		};
	}

	if (i < 0)
	{
		rect.left = ((xCursor+i)*16-offX)*scale;
		rect.right = ((xCursor+1)*16-offX)*scale;
	}
	else
	{
		rect.left = (xCursor*16-offX)*scale;
		rect.right = ((xCursor+i+1)*16-offX)*scale;
	}

	if (j < 0)
	{
		rect.top = ((yCursor+j)*16-offY)*scale;
		rect.bottom = ((yCursor+1)*16-offY)*scale;
	}
	else
	{
		rect.top = (yCursor*16-offY)*scale;
		rect.bottom = ((yCursor+j+1)*16-offY)*scale;
	}

	rgn.CreateRectRgnIndirect(&rect);

	rect.bottom--;
	rect.top++;
	rect.left++;
	rect.right--;
	rgn2.CreateRectRgnIndirect(&rect);

	//this makes a 1 pixel thick rectangle
	rgn.CombineRgn(&rgn, &rgn2, RGN_XOR);

	cdc->InvertRgn(&rgn);
	redraw = false;

	view.ReleaseDC(cdc);

	GetDlgItem(FRAME_MAP)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	HCURSOR hCursor;
	//gets map display rectangle relative to main window area
	if (rect.PtInRect(point) && xCursor < caveMap.width && yCursor < caveMap.height)
	{
		if (GetFocus() != NULL)
			if (GetFocus() == &tileWindow || GetFocus() == &m_MapInfo || GetFocus() == &m_EntityInfo//claim focus if a child window has it
			|| GetFocus()->GetParent() == &m_MapInfo || GetFocus()->GetParent() == &m_EntityInfo)//claim focus if a child window has it
				SetFocus();

		sprintf(buffer, "Map Editor - %s (%s) (%i, %i)", caveMap.mapInfo.caption, caveMap.mapInfo.fileName, xCursor, yCursor);

		if (GetCheckedRadioButton(RADIO_ENTITY, RADIO_MAP) == RADIO_MAP)
		{
			switch (m_MapInfo.GetCheckedRadioButton(RADIO_DRAW_TILES, RADIO_REPLACE_TILES))
			{
			case RADIO_DRAW_TILES:
				hCursor = AfxGetApp()->LoadCursor(CURSOR_PIXEL);
				break;
			case RADIO_FILL_TILES:
				hCursor = AfxGetApp()->LoadCursor(CURSOR_FILL);
				break;
			case RADIO_RECT_TILES:
				hCursor = AfxGetApp()->LoadCursor(CURSOR_RECT);
				break;
			case RADIO_REPLACE_TILES:
				hCursor = AfxGetApp()->LoadCursor(CURSOR_REPLACE);
				break;
			};

			ASSERT(hCursor);
			::SetCursor(hCursor);
		};

	}
	else
	{
		sprintf(buffer, "Map Editor - %s (%s)", caveMap.mapInfo.caption, caveMap.mapInfo.fileName);

		hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		ASSERT(hCursor);
		::SetCursor(hCursor);
	}

	GetWindowText(buffer2, 255);
	if (strcmp(buffer, buffer2) != 0)
		SetWindowText(buffer);

	CDialog::OnMouseMove(nFlags, point);
}

void CaveMapEditDlg::UpdateScrollBars()
{
	RECT rect;
	GetDlgItem(FRAME_MAP)->GetClientRect(&rect);

	m_ScrollVert.SetScrollRange(0, caveMap.height*16+1, false);
	m_ScrollHorz.SetScrollRange(0, caveMap.width*16+1, false);

	tagSCROLLINFO sInfo;
	sInfo.cbSize = sizeof(sInfo);
	sInfo.fMask = SIF_PAGE;

	sInfo.nPage = rect.right/scale;
	m_ScrollHorz.SetScrollInfo(&sInfo, false);

	sInfo.nPage = rect.bottom/scale;
	m_ScrollVert.SetScrollInfo(&sInfo, false);

	m_ScrollVert.SetScrollPos(offY, false);
	offY = m_ScrollVert.GetScrollPos();
	m_ScrollHorz.SetScrollPos(offX, false);
	offX = m_ScrollHorz.GetScrollPos();

	if (caveMap.height*16+1 <= rect.bottom/scale)
		m_ScrollVert.EnableWindow(false);
	else
	{
		m_ScrollVert.EnableWindow();
		m_ScrollVert.SetScrollPos(offY);
		offY = m_ScrollVert.GetScrollPos();
	}

	if (caveMap.width*16+1 <= rect.right/scale)
		m_ScrollHorz.EnableWindow(false);
	else
	{
		m_ScrollHorz.EnableWindow();
		m_ScrollHorz.SetScrollPos(offX);
		offX = m_ScrollHorz.GetScrollPos();
	}
}


//zooming in and out
BOOL CaveMapEditDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect rect;
	double ds = scale;//get old scale
	if (CS_DEFAULT_WHEEL_ZOOM)
	{
		if (zDelta > 0)//zoom in
		{
			if (scale > 4)//only allow so much zooming
				return CDialog::OnMouseWheel(nFlags, zDelta, pt);;
			if (scale < 1)
				scale = 1/(1/scale-1);
			else
				scale++;
		}
		else//zoom out
		{
			if (scale < 0.25)//only allow so much zooming
				return CDialog::OnMouseWheel(nFlags, zDelta, pt);;
			if (scale < 1.5)
				scale = 1/(1/scale+1);
			else
				scale--;
		};


		GetDlgItem(FRAME_MAP)->GetWindowRect(&rect);
		GetDlgItem(FRAME_MAP)->ScreenToClient(&rect);//force both rect and point in to relative coords

		GetDlgItem(FRAME_MAP)->ScreenToClient(&pt);

		double x, y;
		//make zooming in and out center on mouse location
		x = (rect.right-rect.left)/2.0;
		y = (rect.bottom-rect.top)/2.0;

		//force mouse point inside rectangle
		pt.x = min(max(pt.x, rect.left), rect.right);
		pt.y = min(max(pt.y, rect.top), rect.bottom);

		offX += (pt.x-x)/ds;
		offY += (pt.y-y)/ds;

		offX += x/ds-x/scale;
		offY += y/ds-y/scale;

		UpdateScrollBars();

		CDC* cdc;
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
	}
	else
	{
		if (nFlags == MK_SHIFT && zDelta > 0)
			OnHScroll(SB_LINELEFT, 0, (CScrollBar*)GetDlgItem(SCROLLBAR_HORZ));
		else
		if (nFlags == MK_SHIFT)
			OnHScroll(SB_LINERIGHT, 0, (CScrollBar*)GetDlgItem(SCROLLBAR_HORZ));
		else
		if (zDelta > 0)
			OnVScroll(SB_LINELEFT, 0, (CScrollBar*)GetDlgItem(SCROLLBAR_VERT));
		else
			OnVScroll(SB_LINERIGHT, 0, (CScrollBar*)GetDlgItem(SCROLLBAR_VERT));
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CaveMapEditDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect rect;
	GetDlgItem(FRAME_MAP)->GetWindowRect(&rect);

	CDC* cdc;
	switch (nSBCode)
	{
	case SB_LINELEFT:
	case SB_LEFT:
		offX = max(0, offX-16*scale-(offX%int(scale*16)));
		pScrollBar->SetScrollPos(offX);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_PAGELEFT:
		offX = max(0, offX - rect.right/2);
		pScrollBar->SetScrollPos(offX);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_ENDSCROLL:
		break;
	case SB_PAGERIGHT:
		offX = min(m_ScrollHorz.GetScrollLimit(), offX + rect.right/2);
		pScrollBar->SetScrollPos(offX);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_LINERIGHT:
	case SB_RIGHT:
		offX = min(m_ScrollHorz.GetScrollLimit(), offX+(16*scale-(offX%int(scale*16))));
		pScrollBar->SetScrollPos(offX);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_THUMBPOSITION:
		offX = nPos;
		pScrollBar->SetScrollPos(offX);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_THUMBTRACK:
		offX = nPos;
		pScrollBar->SetScrollPos(offX);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CaveMapEditDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect rect;
	GetDlgItem(FRAME_MAP)->GetWindowRect(&rect);

	CDC* cdc;
	switch (nSBCode)
	{
/*
SB_BOTTOM
SB_ENDSCROLL
SB_TOP
*/
	case SB_LINEUP:
//	case SB_LEFT:
		offY = max(0, offY-16*scale-(offY%int(scale*16)));
		pScrollBar->SetScrollPos(offY);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_PAGEUP:
		offY = max(0, offY - rect.bottom/2);
		pScrollBar->SetScrollPos(offY);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_PAGEDOWN:
		offY = min(pScrollBar->GetScrollLimit(), offY + rect.bottom/2);
		pScrollBar->SetScrollPos(offY);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_LINEDOWN:
//	case SB_RIGHT:
		offY = min(pScrollBar->GetScrollLimit(), offY+(16*scale-(offY%int(scale*16))));
		pScrollBar->SetScrollPos(offY);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_THUMBPOSITION:
		offY = nPos;
		pScrollBar->SetScrollPos(offY);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_THUMBTRACK:
		offY = nPos;
		pScrollBar->SetScrollPos(offY);
		cdc = GetDC();
		RedrawMap(*cdc);
		ReleaseDC(cdc);
		break;
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CaveMapEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);		 // Set big icon
	SetIcon(m_hIcon, TRUE);	   // Set small icon

	//completments of http://www.codeguru.com/Cpp/W-D/dislog/tutorials/article.php/c4965
	//keyboard shortcuts!
	m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), m_lpszTemplateName);
	if (!m_hAccel)
		MessageBox("The hotkey table was not loaded");

	RECT rect;

	GetWindowRect(&rect);
	oldX = rect.left;
	oldY = rect.top;

	changes = false;

	GetMenu()->CheckMenuRadioItem(ID_SORTENTITIIESBY_CATEGORY, ID_SORTENTITIIESBY_IDNUMBER, ID_SORTENTITIIESBY_CATEGORY+entitySort, MF_BYCOMMAND);

	CheckDlgButton(CHECK_BACKGROUND, 1);
	CheckDlgButton(CHECK_BACK_TILES, 1);
	CheckDlgButton(CHECK_FRONT_TILES, 1);
	CheckDlgButton(CHECK_ENTITIES, 1);
	CheckDlgButton(CHECK_ENTITY_TEXT, 1);
	CheckRadioButton(RADIO_ENTITY, RADIO_MAP, RADIO_MAP);

	tileWindow.showTileTypes = false;

	xCursor = -10;
	yCursor = -10;

	tileWindow.Create(DIALOG_TILE_SELECT, this);
	GetDlgItem(FRAME_TILESET)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top -= 16;
	tileWindow.MoveWindow(&rect);

	VERIFY(conMenu.LoadMenu(MENU_MAP_EDIT_CONTEXT));//load context menu now
	popup.CreatePopupMenu();//create blank entity selection menu

	m_ViewFrame.GetClientRect(&rect);

	view.Create("", WS_CHILD | WS_VISIBLE | SS_BITMAP, rect, &m_ViewFrame);
	view.ShowWindow(SW_SHOW);

	GetDlgItem(FRAME_INFO)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	m_MapInfo.Create(DIALOG_MAP_INFO, this);//GetDlgItem(FRAME_INFO));
	m_MapInfo.MoveWindow(&rect);
	m_MapInfo.ShowWindow(SW_SHOW);
	m_MapInfo.CheckRadioButton(RADIO_DRAW_TILES, RADIO_REPLACE_TILES, RADIO_DRAW_TILES);

	m_EntityInfo.Create(DIALOG_ENTITY_INFO, this);//GetDlgItem(FRAME_INFO));
	m_EntityInfo.MoveWindow(&rect);
	m_EntityInfo.ShowWindow(SW_HIDE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CaveMapEditDlg::OnBnClickedFrontTiles()
{
	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::OnBnClickedBackTiles()
{
	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::OnBnClickedBackground()
{
	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::OnBnClickedEntities()
{
	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::OnBnClickedEntityText()
{
	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::OnBnClickedEntitySprites()
{
	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::OnBnClickedEntity()
{
	char temp[300];
	m_MapInfo.ShowWindow(SW_HIDE);
	m_EntityInfo.ShowWindow(SW_SHOW);
	tileWindow.ShowWindow(SW_HIDE);

	int i;

	if (!m_EntityInfo.init)
	{
		switch (entitySort)
		{
		case ENTITY_SORT_CATEGORY:
			OnSortentitiiesbyCategory();
			break;
		case ENTITY_SORT_ID:
			OnSortentitiiesbyIdnumber();
			break;
		case ENTITY_SORT_ALPHA:
			OnSortentitiiesbyName();
			break;
		};
	}
	m_EntityInfo.init = true;

	//prevent unchecking entity button while in entity edit mode
	GetDlgItem(CHECK_ENTITIES)->EnableWindow(false);
	if (!IsDlgButtonChecked(CHECK_ENTITIES))
	{
		CheckDlgButton(CHECK_ENTITIES, true);
		Refresh();
	}

	if (selectedEntity >= 0)
	{
		sprintf(temp, "Entity - %i", selectedEntity);
		SetDlgItemText(TEXT_INFO, temp); 
	}
	else
		SetDlgItemText(TEXT_INFO, "Entity - "); 
}

void CaveMapEditDlg::OnBnClickedMap()
{
	m_MapInfo.ShowWindow(SW_SHOW);
	m_EntityInfo.ShowWindow(SW_HIDE);
	tileWindow.ShowWindow(SW_SHOW);
	SetDlgItemText(TEXT_INFO, "Info"); 
	GetDlgItem(CHECK_ENTITIES)->EnableWindow(true);
}

void CaveMapEditDlg::OnBnClickedTileGrid()
{
	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::OnBnClickedTileTypes()
{
	tileWindow.showTileTypes = IsDlgButtonChecked(CHECK_TILE_TYPES);
	tileWindow.Redraw();

	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::OnCancel()
{
	if (!esc)
	{
		esc = true;
		CDialog::OnCancel();
	}

}

void CaveMapEditDlg::OnClose()
{
	if (changes)//is there are changes ask if they should be saved
	{
		switch(MessageBox("Do you want to save your changes?",0, MB_YESNOCANCEL))
		{
		case IDYES:
			OnBnClickedSave();
		case IDNO:
			changes = false;

			esc = false;
			CDialog::OnClose();
			tileWindow.ShowWindow(SW_HIDE);
//			GetDlgItem(BUTTON_SAVE)->EnableWindow(changes);
			break;
		case IDCANCEL:
			break;
		}
	}
	else
	{
		esc = false;
		CDialog::OnClose();
		tileWindow.ShowWindow(SW_HIDE);
	}
}

void CaveMapEditDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		if (IsDlgButtonChecked(RADIO_MAP))
		{
			tileWindow.ShowWindow(SW_SHOW);
			SetFocus();
		}
	}
	else
		tileWindow.ShowWindow(SW_HIDE);

}

void CaveMapEditDlg::OnBnClickedProperties()
{
	CaveMapPropertiesDlg propDlg;
	propDlg.caveInfo = &caveMap.mapInfo;//set mapInfo pointer

	propDlg.oldFileName = caveMap.oldFileName;
	propDlg.width = caveMap.width;
	propDlg.height = caveMap.height;

	char buffer[256];

	if (propDlg.DoModal() == IDOK)
	{
		if (propDlg.redraw)
			reloadMap();
		if (propDlg.changes)
			changes = true;
		//if resize edit boxes are not enabled then called without map being loaded
		if (propDlg.width != caveMap.width || propDlg.height != caveMap.height)
		{
			if (propDlg.width > 0 && propDlg.height > 0)
			{
				changes = true;
				AddUndo(UNDO_MAP, "Map Resize");
				caveMap.resize(propDlg.width, propDlg.height);

				//new bitmaps are needed for the new maps
				if (bmpMem[0].m_hObject != NULL)
						bmpMem[0].DeleteObject();
				if (bmpMem[1].m_hObject != NULL)
						bmpMem[1].DeleteObject();
				if (bmpMem[2].m_hObject != NULL)
						bmpMem[2].DeleteObject();
				if (bmpMem[3].m_hObject != NULL)
						bmpMem[3].DeleteObject();
				if (bmpMem[4].m_hObject != NULL)
						bmpMem[4].DeleteObject();

				bmpMem[0].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
				bmpMem[1].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
				bmpMem[2].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
				bmpMem[3].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
				bmpMem[4].CreateBitmap(caveMap.width*16, caveMap.height*16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);

				UpdateScrollBars();
				Refresh(true);
			}
		}
		caveMap.oldFileName = propDlg.oldFileName;

		sprintf(buffer, ".\\data\\Npc\\Npc%s.%s", caveMap.mapInfo.NPCset1, CS_DEFAULT_PBM_EXT);
		caveMap.NPCset1.Load(buffer);
		sprintf(buffer, ".\\data\\Npc\\Npc%s.%s", caveMap.mapInfo.NPCset2, CS_DEFAULT_PBM_EXT);
		caveMap.NPCset2.Load(buffer);
	}
}

void CaveMapEditDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 525;
	lpMMI->ptMinTrackSize.y = 200;
}

void CaveMapEditDlg::OnBnClickedMapScript()
{
	char temp[64];
	sprintf(temp, ".\\data\\Stage\\%s.tsc", caveMap.mapInfo.fileName);

	scriptEditorDlg.Open(temp);
	scriptEditorDlg.ShowWindow(SW_SHOW);
}

void CaveMapEditDlg::OnBnClickedSave()
{
	if (!changes)
		return;

	int sel;
	char buffer [100];
	CString fileName;
	CFile file;

	if (!caveMap.save())
	{
		MessageBox("Error Saving map data!");
		return;
	}
	exe.mapInfo[mapIndex] = caveMap.mapInfo;

	sprintf(buffer, "[%.3i] %s (%s)", mapIndex, exe.mapInfo[mapIndex].caption, exe.mapInfo[mapIndex].fileName);
	sel = mainWindow->m_ListMaps.GetCurSel();
	mainWindow->m_ListMaps.DeleteString(mapIndex);
	mainWindow->m_ListMaps.InsertString(mapIndex, buffer);
	mainWindow->m_ListMaps.SetCurSel(sel);

	mainWindow->changes = true;

	changes = false;
}


void CaveMapEditDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!input)//prevents drawing during window opening
		return;

	//this will prevent mouse dragging over the tile window from activating it.
	tileWindow.EnableWindow(false);

	bool lc = lclick;//get old value to see if this is the "first" click message

	CRect rect;
	GetDlgItem(FRAME_MAP)->GetWindowRect(&rect);
	ScreenToClient(&rect);//gets map display rectangle relative to main window area

	if (!move)
	{
		oldXC = int((point.x-rect.left)/16.0/scale+offX/16.0);//find cursor's tile x, y coord
		oldYC = int((point.y-rect.top)/16.0/scale+offY/16.0);
		OnMouseMove(nFlags, point);
		return;
	}
	lclick = true;


	//if clicked outside map area ignore click
	if (!rect.PtInRect(point))
		return;

	int xC = int((point.x-rect.left)/16.0/scale+offX/16.0);//find cursor's tile x, y coord
	int yC = int((point.y-rect.top)/16.0/scale+offY/16.0);

	//if clicked outside map area ignore click
	if (xC >= caveMap.width || yC >= caveMap.height)
		return;

	if (GetCheckedRadioButton(RADIO_ENTITY, RADIO_MAP) == RADIO_MAP)
	{
		switch (m_MapInfo.GetCheckedRadioButton(RADIO_DRAW_TILES, RADIO_FILL_TILES))
		{
		case RADIO_DRAW_TILES:
			if (!lc)//this is the "first click" in the draw tile, set undo marker
			{
				AddUndo(UNDO_MAP, "Draw Tiles");
			}
			PaintTiles(xC, yC);
			break;
		case RADIO_FILL_TILES://this is done on the end of the click (OnLButtonUp)
			break;
		}
	}
	else//entity selection and movement
	{
		if (selectedEntity == -1)
			return;

		//if clicking on a selected entity, it can be moved
		if (!lc && caveMap.entity[selectedEntity].x == xC && caveMap.entity[selectedEntity].y == yC)
			movingEntity = true;
		else
		if (movingEntity)
		{
			//if actually moved
			if (caveMap.entity[selectedEntity].x != xC || caveMap.entity[selectedEntity].y != yC)
			{
				if (!entityMoved)
				{
					entityMoved = true;

					AddUndo(UNDO_ENTITY, "Move Entity");
					undoStack[undoPtr].ePtr->index = selectedEntity;
					undoStack[undoPtr].ePtr->item = UNDO_ENTITY_XY;
					undoStack[undoPtr].ePtr->enity.x = caveMap.entity[selectedEntity].x;
					undoStack[undoPtr].ePtr->enity.y = caveMap.entity[selectedEntity].y;
				}
				caveMap.entity[selectedEntity].x = xC;
				caveMap.entity[selectedEntity].y = yC;
				Refresh();
				changes = true;
			}
		}
	}
}

void CaveMapEditDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	//this will prevent mouse dragging over the tile window from activating it.
	tileWindow.EnableWindow();

	lclick = false;
	movingEntity = false;
	CRect rect;

	GetDlgItem(FRAME_MAP)->GetWindowRect(&rect);
	ScreenToClient(&rect);//gets map display rectangle relative to main window area

	//if clicked outside map area ignore click
	if (!rect.PtInRect(point))
		return;

	int xC = int((point.x-rect.left)/16.0/scale+offX/16.0);
	int yC = int((point.y-rect.top)/16.0/scale+offY/16.0);

	//if clicked outside map area ignore click
	if (xC >= caveMap.width || yC >= caveMap.height)
		return;

	if (GetCheckedRadioButton(RADIO_ENTITY, RADIO_MAP) == RADIO_MAP)
	{
		switch (m_MapInfo.GetCheckedRadioButton(RADIO_DRAW_TILES, RADIO_REPLACE_TILES))
		{
		case RADIO_DRAW_TILES://this is done on mouse down
			break;
		case RADIO_FILL_TILES:
			oldXC = -1;
			oldYC = -1;
			if (caveMap.getTile(xC, yC) == tileWindow.selected)
				return;
			AddUndo(UNDO_MAP, "Fill Tiles");
			FillTiles(xC, yC, caveMap.getTile(xC, yC));
			Refresh(true);
			break;
		case RADIO_RECT_TILES:
			AddUndo(UNDO_MAP, "Block Fill Tiles");
			ReplaceTileBlock(xC, yC, oldXC, oldYC);
			Refresh(true);
			break;
		case RADIO_REPLACE_TILES:
			AddUndo(UNDO_MAP, "Replace Tiles");
			ReplaceTiles(xC, yC, caveMap.getTile(xC, yC));
			Refresh(true);
			break;
		}
	}
	else//entity select
	{
		if (!entityMoved)//if no entity was moved then find what we stopped on
			FindEntity(point);
		entityMoved = false;
	}
	oldXC = -1;
	oldYC = -1;
}


//used to find entity that has been clicked on
void CaveMapEditDlg::FindEntity(CPoint point)
{
	CDC* cdc;
	RECT rect;
	CBrush brush;
	char buffer[256];
	int index[256];//up to 256 entities on the same square
	int count = 0;
	int i;

	for (i = 0; i < caveMap.entityNum; i++)
	{
		if (caveMap.entity[i].x == xCursor && caveMap.entity[i].y == yCursor)
			index[count++] = i;
	}

	if (selectedEntity == -1)//nothing currently selected
	{
		switch (count)
		{
		case 0://nothing clicked on
			return;
		case 1://one thing click on
			//replace regular entity box with selected entity box
			selectedEntity = index[0];

			rect.left = int(caveMap.entity[selectedEntity].x*scale*16 - offX*scale);
			rect.top = int(caveMap.entity[selectedEntity].y*scale*16 - offY*scale);
			rect.right = int(rect.left+scale*16);
			rect.bottom = int(rect.top+scale*16);

			cdc = view.GetDC();
			//inverted since the frame is currently inverted from mouse over tile box
			brush.CreateSolidBrush(COLOR_MASK ^ ENTITY_SELECTED_COLOR);
			cdc->FrameRect(&rect, &brush);
			view.ReleaseDC(cdc);

			brush.DeleteObject();

			m_EntityInfo.load(&(caveMap.entity[selectedEntity]), selectedEntity);
			return;
		default://more than one entity clicked
			while (popup.GetMenuItemCount() > 0)
				popup.RemoveMenu(0, MF_BYPOSITION);
			for (i = 0; i < count; i++)
			{
				sprintf(buffer, "Entity %i - %i %s", index[i], caveMap.entity[index[i]].npc, npcTable.extra[caveMap.entity[index[i]].npc].desc);
				popup.AppendMenu(MF_ENABLED, 2000+index[i], buffer);
			}
			ClientToScreen(&point);
			popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x+1, point.y, this);

			return;
			return;
		}
	}
	else//something selected
	{
		switch (count)
		{
		case 0://nothing clicked on	     
			//replace selected entity box with regular entity box
			rect.left = int(caveMap.entity[selectedEntity].x*scale*16 - offX*scale);
			rect.top = int(caveMap.entity[selectedEntity].y*scale*16 - offY*scale);
			rect.right = int(rect.left+scale*16);
			rect.bottom = int(rect.top+scale*16);

			cdc = view.GetDC();
			brush.CreateSolidBrush(ENTITY_COLOR);
			cdc->FrameRect(&rect, &brush);
			view.ReleaseDC(cdc);

			brush.DeleteObject();

			selectedEntity = -1;
			m_EntityInfo.clear();
			return;
		case 1://one thing clicked on
			if (selectedEntity == index[0])//if same thing selected
				return;

			cdc = view.GetDC();
			//replace selected entity box with regular entity box
			rect.left = int(caveMap.entity[selectedEntity].x*scale*16 - offX*scale);
			rect.top = int(caveMap.entity[selectedEntity].y*scale*16 - offY*scale);
			rect.right = int(rect.left+scale*16);
			rect.bottom = int(rect.top+scale*16);

			brush.CreateSolidBrush(ENTITY_COLOR);
			cdc->FrameRect(&rect, &brush);
			brush.DeleteObject();

			//replace regular entity box with selected entity box
			selectedEntity = index[0];

			rect.left = int(caveMap.entity[selectedEntity].x*scale*16 - offX*scale);
			rect.top = int(caveMap.entity[selectedEntity].y*scale*16 - offY*scale);
			rect.right = int(rect.left+scale*16);
			rect.bottom = int(rect.top+scale*16);

			//inverted since the frame is currently inverted from mouse over tile box
			brush.CreateSolidBrush(COLOR_MASK ^ ENTITY_SELECTED_COLOR);
			cdc->FrameRect(&rect, &brush);
			brush.DeleteObject();

			view.ReleaseDC(cdc);
			m_EntityInfo.load(&(caveMap.entity[selectedEntity]), selectedEntity);
			return;
		default:
			while (popup.GetMenuItemCount() > 0)
				popup.RemoveMenu(0, MF_BYPOSITION);
			for (i = 0; i < count; i++)
			{
				sprintf(buffer, "Entity %i - %i %s", index[i], caveMap.entity[index[i]].npc, npcTable.extra[caveMap.entity[index[i]].npc].desc);
				popup.AppendMenu(MF_ENABLED, 2000+index[i], buffer);
			}
			ClientToScreen(&point);
			popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x+1, point.y, this);

			return;
		}
	}
}


void CaveMapEditDlg::PaintTiles(int xC, int yC)
{
	bool change = false;
	int selected;
	int i, j, k, l;
	CBitmap* pOldBmp;
	CDC* cdc;
	CDC cdc2;

	//if not the same as old tile then replace and redraw

	cdc = GetDC();
	cdc2.CreateCompatibleDC(cdc);
	for (i = xC; i < xC+tileWindow.x2-tileWindow.x+1; i++)
	{
		for (j = yC; j < yC+tileWindow.y2-tileWindow.y+1; j++)
		{
			selected = (tileWindow.selected+i-xC)+(j-yC)*(tileWindow.tileSet->tileData.GetWidth()/16);
			if (caveMap.setTile(i, j, selected) != selected)
			{
				for (k = 0; k < MAP_DISPLAY_LAST; k++)
				{
					pOldBmp = (CBitmap *)(cdc2.SelectObject(&bmpMem[k]));
					switch (k)
					{
					case MAP_BACKGROUND:
						break;
					case MAP_BACK_TILES:
						cdc2.FillSolidRect(i*16,j*16, 16, 16, 0x000000);
						caveMap.displayBackMap(&cdc2, i*16, j*16, i, j, i, j);
						break;
					case MAP_FRONT_TILES:
						cdc2.FillSolidRect(i*16,j*16, 16, 16, 0x000000);
						caveMap.displayFrontMap(&cdc2, i*16, j*16, i, j, i, j);
						break;
					case MAP_TILE_TYPES:
						cdc2.FillSolidRect(i*16,j*16, 16, 16, 0x000000);
						caveMap.displayTileTypes(&cdc2, i*16, j*16, i, j, i, j);
						break;
					}
					//remove created bitmap and release memory
					cdc2.SelectObject(pOldBmp);
					change = true;
				}//end for loop
			}//end if
		}//end for loop
	}//end for loop

	if (change)
	{
		RedrawMap(*cdc, false);
		redraw = true;
		ReleaseDC(cdc);
		changes = true;
	}
	ReleaseDC(cdc);
	cdc2.DeleteDC();
}

void CaveMapEditDlg::ReplaceTileBlock(int x, int y, int x2, int y2)
{
	if (x < 0 || y < 0 || x2 < 0 || y2 < 0)
		return;
	if (x >= caveMap.width || x2 >= caveMap.width || y >= caveMap.height || y2 >= caveMap.height)
		return;

	int selected;
	int i, j;

	//first we'll make sure x < x2 and y < y2
	if (x2 < x)
	{
		i = x;
		x = x2;
		x2 = i;
	}
	if (y2 < y)
	{
		i = y;
		y = y2;
		y2 = i;
	}

	int oX = x;
	int oY = y;
	for (i = x; i <= x2; i++)
	{
		for (j = y; j <= y2; j++)
		{
			oX = ((tileWindow.x2-tileWindow.x+1)*256+i-x) % (tileWindow.x2-tileWindow.x+1);
			oY = ((tileWindow.y2-tileWindow.y+1)*256+j-y) % (tileWindow.y2-tileWindow.y+1);
			selected = (tileWindow.selected+oX)+oY*(tileWindow.tileSet->tileData.GetWidth()/16);
			caveMap.setTile(i, j, selected);
		}
	}
	changes = true;
}

//helper for tile replacing
void CaveMapEditDlg::ReplaceTiles(int x, int y, char oldTile)
{
	int selected;
	int oX = x;
	int oY = y;

	int i, j;
	for (i = 0; i < caveMap.width; i++)
	{
		for (j = 0; j < caveMap.height; j++)
		{
			if (caveMap.getTile(i,j) == oldTile)
			{
				oX = ((tileWindow.x2-tileWindow.x+1)*256+i-x) % (tileWindow.x2-tileWindow.x+1);
				oY = ((tileWindow.y2-tileWindow.y+1)*256+j-y) % (tileWindow.y2-tileWindow.y+1);
				selected = (tileWindow.selected+oX)+oY*(tileWindow.tileSet->tileData.GetWidth()/16);
				if (caveMap.getTile(i ,j) != selected)
				{
					caveMap.setTile(i, j, selected);
					changes = true;
				}
			}
		}
	}
}

//helper for tile painting
void CaveMapEditDlg::FillTiles(int x, int y, char match, int oX, int oY)
{
	//shift offset to be within selected tiles
	oX = ((tileWindow.x2-tileWindow.x+1)+oX) % (tileWindow.x2-tileWindow.x+1);
	oY = ((tileWindow.y2-tileWindow.y+1)+oY) % (tileWindow.y2-tileWindow.y+1);
	int selected = (tileWindow.selected+oX)+oY*(tileWindow.tileSet->tileData.GetWidth()/16);

	if (caveMap.getTile(x, y) != match || caveMap.getTile(x, y) == selected)
		return;
	changes = true;

	caveMap.setTile(x, y, selected);

	if (x+1 < caveMap.width)
		FillTiles(x+1, y, match, oX+1, oY);
	if (y+1 < caveMap.height)
	FillTiles(x, y+1, match, oX, oY+1);
	if (x-1 >= 0)
		FillTiles(x-1, y, match, oX-1, oY);
	if (y-1 >= 0)
		FillTiles(x, y-1, match, oX, oY-1);
}

//used to handle popup menus only
BOOL CaveMapEditDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0 && wParam >= 2000 && wParam < 3000)//from right-click contex menu
	{
		selectedEntity = wParam-2000;
		m_EntityInfo.load(&(caveMap.entity[selectedEntity]), selectedEntity);
		Refresh();
	}

	return CDialog::OnCommand(wParam, lParam);
}


//called by child to update entity info that's being changed
void CaveMapEditDlg::UpdateEntity()
{
	if (caveMap.entity[m_EntityInfo.index].npc != m_EntityInfo.type)
	{
		AddUndo(UNDO_ENTITY, "Change Entity Type");
		undoStack[undoPtr].ePtr->item = UNDO_ENTITY_NPC;
		undoStack[undoPtr].ePtr->index = selectedEntity;
		undoStack[undoPtr].ePtr->enity.npc = caveMap.entity[m_EntityInfo.index].npc;

		caveMap.entity[m_EntityInfo.index].npc = m_EntityInfo.type;
		changes = true;
	}
	if (caveMap.entity[m_EntityInfo.index].event != m_EntityInfo.GetDlgItemInt(EDIT_EVENT_ID))
	{
		AddUndo(UNDO_ENTITY, "Change Entity Event");
		undoStack[undoPtr].ePtr->item = UNDO_ENTITY_EVENT;
		undoStack[undoPtr].ePtr->index = selectedEntity;
		undoStack[undoPtr].ePtr->enity.event = caveMap.entity[m_EntityInfo.index].event;

		caveMap.entity[m_EntityInfo.index].event = m_EntityInfo.GetDlgItemInt(EDIT_EVENT_ID);
		changes = true;
	}
	if (caveMap.entity[m_EntityInfo.index].flagID != m_EntityInfo.GetDlgItemInt(EDIT_FLAG_ID))
	{
		AddUndo(UNDO_ENTITY, "Change Entity Flag ID");
		undoStack[undoPtr].ePtr->item = UNDO_ENTITY_FLAGID;
		undoStack[undoPtr].ePtr->index = selectedEntity;
		undoStack[undoPtr].ePtr->enity.flagID = caveMap.entity[m_EntityInfo.index].flagID;

		caveMap.entity[m_EntityInfo.index].flagID = m_EntityInfo.GetDlgItemInt(EDIT_FLAG_ID);
		changes = true;
	}
	char buffer[10];
	int temp;
	m_EntityInfo.GetDlgItemText(EDIT_FLAGS, buffer, 9);
	sscanf(buffer, "%x", &temp);

	if (caveMap.entity[m_EntityInfo.index].flags != temp)
	{
		AddUndo(UNDO_ENTITY, "Change Entity Flags");
		undoStack[undoPtr].ePtr->item = UNDO_ENTITY_FLAGS;
		undoStack[undoPtr].ePtr->index = selectedEntity;
		undoStack[undoPtr].ePtr->enity.flags = caveMap.entity[m_EntityInfo.index].flags;

		caveMap.entity[m_EntityInfo.index].flags = temp;
		changes = true;
	}
}



void CaveMapEditDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int i = -1;
	RECT rect;
	CMenu* pPopup;

	//select context specific menu
	GetDlgItem(FRAME_MAP)->GetClientRect(&rect);
	GetDlgItem(FRAME_MAP)->ClientToScreen(&rect);

	if (!PtInRect(&rect, point))
		return;//no context menu except inside view

	if (IsDlgButtonChecked(RADIO_MAP))//is the map edit or entity edit button checked
	   pPopup = conMenu.GetSubMenu(0);//Map menu is the second one
	else
	{
		if (selectedEntity != -1)//something is selected
			pPopup = conMenu.GetSubMenu(1);
		else
			pPopup = conMenu.GetSubMenu(2);
	}

   ASSERT(pPopup != NULL);
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x+1, point.y, this);
}

void CaveMapEditDlg::OnMapShiftmap()
{
	ResizeDlg resize;
	resize.width = 0;
	resize.height = 0;
	resize.hName = "Y Shift";
	resize.wName = "X Shift";
	resize.name = "Shift Map";
	resize.c1Name = "Wrap?";
	resize.check1 = false;
	if (resize.DoModal() == IDOK)
	{
		caveMap.shift(resize.width, resize.height, resize.check1);
		Refresh(true);
		changes = true;
	}
}

void CaveMapEditDlg::OnEntityAddentity()
{
	selectedEntity = caveMap.addEntity(xCursor, yCursor);
	m_EntityInfo.load(&(caveMap.entity[selectedEntity]), selectedEntity);

	AddUndo(UNDO_ENTITY, "Add Entity");
	undoStack[undoPtr].ePtr->item = UNDO_ENTITY_INSERT;
	undoStack[undoPtr].ePtr->index = caveMap.entityNum-1;

	changes = true;
	Refresh();
}

void CaveMapEditDlg::OnEntityDeleteentity()
{
	if (selectedEntity == -1)
		return;
	AddUndo(UNDO_ENTITY, "Delete Entity");
	undoStack[undoPtr].ePtr->item = UNDO_ENTITY_DELETE;
	undoStack[undoPtr].ePtr->enity = caveMap.entity[selectedEntity];
	undoStack[undoPtr].ePtr->index = selectedEntity;


	caveMap.deleteEntity(selectedEntity);
	selectedEntity = -1;
	m_EntityInfo.clear();
	changes = true;
	Refresh();
}

void CaveMapEditDlg::OnEntityCopyentity()
{
	copyEntity = caveMap.entity[selectedEntity];
}

void CaveMapEditDlg::OnEntityPasteentity()
{
	copyEntity.x = xCursor;
	copyEntity.y = yCursor;
	selectedEntity = caveMap.addEntity(copyEntity);
	m_EntityInfo.load(&(caveMap.entity[selectedEntity]), selectedEntity);

	AddUndo(UNDO_ENTITY, "Paste Entity");
	undoStack[undoPtr].ePtr->item = UNDO_ENTITY_INSERT;
	undoStack[undoPtr].ePtr->index = caveMap.entityNum-1;

	changes = true;
	Refresh();
}

void CaveMapEditDlg::OnZoom05x()
{
	scale = 0.5;
	Refresh();
}

void CaveMapEditDlg::OnZoom1x()
{
	scale = 1;
	Refresh();
}

void CaveMapEditDlg::OnZoom2x()
{
	scale = 2;
	Refresh();
}

void CaveMapEditDlg::OnZoom3x()
{
	scale = 3;
	Refresh();
}

void CaveMapEditDlg::OnZoom4x()
{
	scale = 4;
	Refresh();
}

void CaveMapEditDlg::OnMapEditscript()
{
	OnBnClickedMapScript();
}

void CaveMapEditDlg::OnMapProperties()
{
	OnBnClickedProperties();
}

void CaveMapEditDlg::OnMapSave()
{
	OnBnClickedSave();
}

void CaveMapEditDlg::OnOK()
{
	//CDialog::OnOK(); [Enter] ot allowed
}

void CaveMapEditDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	RECT rect;
	tileWindow.GetWindowRect(&rect);
	rect.left += x-oldX;
	rect.right += x-oldX;
	rect.top += y-oldY;
	rect.bottom += y-oldY;
	oldY = y;
	oldX = x;
	tileWindow.MoveWindow(&rect);
}

BOOL CaveMapEditDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST && m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CaveMapEditDlg::OnMapSetstartlocation()
{
	*exe.startX = xCursor;
	*exe.startY = yCursor;
	*exe.startMap = mapIndex;
	mainWindow->changes = true;
}

BOOL CaveMapEditDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest != HTCLIENT)
		return CDialog::OnSetCursor(pWnd, nHitTest, message);

/*/
	HCURSOR hCursor = AfxGetApp()->LoadCursor(CURSOR_PIXEL);

	ASSERT(hCursor);
   ::SetCursor(hCursor);
   return TRUE;
//*/
	return FALSE;
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}


void CaveMapEditDlg::OnEditUndo()
{
	if (!CanUndo() || movingEntity)
		return;
	Undo();
}

void CaveMapEditDlg::OnEditRedo()
{
	if (!CanRedo() || movingEntity)
		return;
	Redo();
}

/*
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 
  S		   S		   S		   S
		    P		   P		   P		   P
		    E		   E		   E		   E
//*/
void CaveMapEditDlg::AddUndo(int type, CString desc)
{
	undoPtr = (undoPtr+1)%MAX_UNDO;
	undoEnd = undoPtr;

	clean(undoStack[undoPtr].mPtr);
	clean(undoStack[undoPtr].ePtr);
	undoStack[undoPtr].mPtr = NULL;
	undoStack[undoPtr].ePtr = NULL;

	undoStack[undoPtr].desc = desc;

	if (undoPtr == undoStart)
	{
		undoStart = (undoStart+1)%MAX_UNDO;
		if (undoStack[undoPtr].mPtr != NULL)
			clean(undoStack[undoPtr].mPtr->tiles);
		clean(undoStack[undoPtr].mPtr);
		clean(undoStack[undoPtr].ePtr);
	}
	if (type == UNDO_ENTITY)
		undoStack[undoPtr].ePtr = new EntityStack;
	else
	{
		undoStack[undoPtr].mPtr = new MapStack;
		undoStack[undoPtr].mPtr->height = caveMap.height;
		undoStack[undoPtr].mPtr->width = caveMap.width;
		undoStack[undoPtr].mPtr->tiles = new unsigned char[caveMap.height*caveMap.width];
		memcpy(undoStack[undoPtr].mPtr->tiles, caveMap.tiles, caveMap.height*caveMap.width);
	}

	FixUndoMenu();
}

void CaveMapEditDlg::Undo()
{
	if (!CanUndo())
		return;

	clean(redoStack[undoPtr].mPtr);
	clean(redoStack[undoPtr].ePtr);
	redoStack[undoPtr].mPtr = NULL;
	redoStack[undoPtr].ePtr = NULL;

	//map undo
	if (undoStack[undoPtr].mPtr != NULL)
	{
		redoStack[undoPtr].mPtr = new MapStack;

		redoStack[undoPtr].mPtr->height = caveMap.height;
		redoStack[undoPtr].mPtr->width = caveMap.width;
		redoStack[undoPtr].mPtr->tiles = new unsigned char[caveMap.height*caveMap.width];
		memcpy(redoStack[undoPtr].mPtr->tiles, caveMap.tiles, caveMap.height*caveMap.width);

		caveMap.width = undoStack[undoPtr].mPtr->width;
		caveMap.height = undoStack[undoPtr].mPtr->height;

		delete[] caveMap.tiles;
		caveMap.tiles = new unsigned char[caveMap.height*caveMap.width];
		memcpy(caveMap.tiles, undoStack[undoPtr].mPtr->tiles, caveMap.height*caveMap.width);

	}
	else//entity undo
	{
		redoStack[undoPtr].ePtr = new EntityStack;
		redoStack[undoPtr].ePtr->enity = undoStack[undoPtr].ePtr->enity;

		switch (undoStack[undoPtr].ePtr->item)
		{
		case UNDO_ENTITY_XY:
			redoStack[undoPtr].ePtr->item = undoStack[undoPtr].ePtr->item;
			redoStack[undoPtr].ePtr->index = undoStack[undoPtr].ePtr->index;
			redoStack[undoPtr].ePtr->enity.x = caveMap.entity[undoStack[undoPtr].ePtr->index].x;
			redoStack[undoPtr].ePtr->enity.y = caveMap.entity[undoStack[undoPtr].ePtr->index].y;

			caveMap.entity[undoStack[undoPtr].ePtr->index].x = undoStack[undoPtr].ePtr->enity.x;
			caveMap.entity[undoStack[undoPtr].ePtr->index].y = undoStack[undoPtr].ePtr->enity.y;
			break;
		case UNDO_ENTITY_NPC:
			redoStack[undoPtr].ePtr->item = undoStack[undoPtr].ePtr->item;
			redoStack[undoPtr].ePtr->index = undoStack[undoPtr].ePtr->index;
			redoStack[undoPtr].ePtr->enity.npc = caveMap.entity[undoStack[undoPtr].ePtr->index].npc;

			caveMap.entity[undoStack[undoPtr].ePtr->index].npc = undoStack[undoPtr].ePtr->enity.npc;
			break;
		case UNDO_ENTITY_FLAGS:
			redoStack[undoPtr].ePtr->item = undoStack[undoPtr].ePtr->item;
			redoStack[undoPtr].ePtr->index = undoStack[undoPtr].ePtr->index;
			redoStack[undoPtr].ePtr->enity.flags = caveMap.entity[undoStack[undoPtr].ePtr->index].flags;

			caveMap.entity[undoStack[undoPtr].ePtr->index].flags = undoStack[undoPtr].ePtr->enity.flags;
			break;
		case UNDO_ENTITY_FLAGID:
			redoStack[undoPtr].ePtr->item = undoStack[undoPtr].ePtr->item;
			redoStack[undoPtr].ePtr->index = undoStack[undoPtr].ePtr->index;
			redoStack[undoPtr].ePtr->enity.flagID = caveMap.entity[undoStack[undoPtr].ePtr->index].flagID;

			caveMap.entity[undoStack[undoPtr].ePtr->index].flagID = undoStack[undoPtr].ePtr->enity.flagID;
			break;
		case UNDO_ENTITY_EVENT:
			redoStack[undoPtr].ePtr->item = undoStack[undoPtr].ePtr->item;
			redoStack[undoPtr].ePtr->index = undoStack[undoPtr].ePtr->index;
			redoStack[undoPtr].ePtr->enity.event = caveMap.entity[undoStack[undoPtr].ePtr->index].event;

			caveMap.entity[undoStack[undoPtr].ePtr->index].event = undoStack[undoPtr].ePtr->enity.event;
			break;
		case UNDO_ENTITY_DELETE:
			redoStack[undoPtr].ePtr->item = UNDO_ENTITY_INSERT;
			redoStack[undoPtr].ePtr->index = undoStack[undoPtr].ePtr->index;

			caveMap.addEntity(undoStack[undoPtr].ePtr->enity, undoStack[undoPtr].ePtr->index);
			break;
		case UNDO_ENTITY_INSERT:
			redoStack[undoPtr].ePtr->item = UNDO_ENTITY_DELETE;
			redoStack[undoPtr].ePtr->index = undoStack[undoPtr].ePtr->index;
			redoStack[undoPtr].ePtr->enity = caveMap.entity[undoStack[undoPtr].ePtr->index];

			caveMap.deleteEntity(undoStack[undoPtr].ePtr->index);
			if (selectedEntity == caveMap.entityNum)
				selectedEntity = -1;
			break;
		};
		m_EntityInfo.load(&(caveMap.entity[selectedEntity]), selectedEntity);
	}
	undoPtr = (MAX_UNDO + undoPtr - 1)%MAX_UNDO;

	FixUndoMenu();

	CDC* cdc = GetDC();
	RedrawMap(*cdc, true);
	ReleaseDC(cdc);
}

void CaveMapEditDlg::Redo()
{
	if (!CanRedo())
		return;

	undoPtr = (undoPtr + 1)%MAX_UNDO;
	//map undo
	if (redoStack[undoPtr].mPtr != NULL)
	{
		caveMap.width = redoStack[undoPtr].mPtr->width;
		caveMap.height = redoStack[undoPtr].mPtr->height;

		delete[] caveMap.tiles;
		caveMap.tiles = new unsigned char[caveMap.height*caveMap.width];
		memcpy(caveMap.tiles, redoStack[undoPtr].mPtr->tiles, caveMap.height*caveMap.width);

	}
	else//entity undo
	{
		switch (redoStack[undoPtr].ePtr->item)
		{
		case UNDO_ENTITY_XY:
			caveMap.entity[redoStack[undoPtr].ePtr->index].x = redoStack[undoPtr].ePtr->enity.x;
			caveMap.entity[redoStack[undoPtr].ePtr->index].y = redoStack[undoPtr].ePtr->enity.y;
			break;
		case UNDO_ENTITY_NPC:
			caveMap.entity[redoStack[undoPtr].ePtr->index].npc = redoStack[undoPtr].ePtr->enity.npc;
			break;
		case UNDO_ENTITY_FLAGS:
			caveMap.entity[redoStack[undoPtr].ePtr->index].flags = redoStack[undoPtr].ePtr->enity.flags;
			break;
		case UNDO_ENTITY_FLAGID:
			caveMap.entity[redoStack[undoPtr].ePtr->index].flagID = redoStack[undoPtr].ePtr->enity.flagID;
			break;
		case UNDO_ENTITY_EVENT:
			caveMap.entity[redoStack[undoPtr].ePtr->index].event = redoStack[undoPtr].ePtr->enity.event;
			break;
		case UNDO_ENTITY_DELETE:
			caveMap.addEntity(redoStack[undoPtr].ePtr->enity, redoStack[undoPtr].ePtr->index);
			break;
		case UNDO_ENTITY_INSERT:
			caveMap.deleteEntity(redoStack[undoPtr].ePtr->index);
			if (selectedEntity == caveMap.entityNum)
				selectedEntity = -1;
			break;
		};
		m_EntityInfo.load(&(caveMap.entity[selectedEntity]), selectedEntity);
	}

	FixUndoMenu();

	CDC* cdc = GetDC();
	RedrawMap(*cdc, true);
	ReleaseDC(cdc);
}

bool CaveMapEditDlg::CanUndo()
{
	if (undoPtr == undoStart)
		return false;
	return true;
}

bool CaveMapEditDlg::CanRedo()
{
	if (undoPtr == undoEnd)
		return false;
	return true;
}

void CaveMapEditDlg::FixUndoMenu()
{
	char buffer[256];
	if (CanUndo())
	{
		sprintf(buffer, "Undo %s (Ctrl+Z)", undoStack[undoPtr].desc);
		GetMenu()->ModifyMenu(ID_EDIT_UNDO, MF_BYCOMMAND, ID_EDIT_UNDO, buffer);
		GetMenu()->EnableMenuItem(ID_EDIT_UNDO, MF_BYCOMMAND | MF_ENABLED);
	}
	else
	{
		sprintf(buffer, "Undo (Ctrl+Z)");
		GetMenu()->ModifyMenu(ID_EDIT_UNDO, MF_BYCOMMAND, ID_EDIT_UNDO, buffer);
		GetMenu()->EnableMenuItem(ID_EDIT_UNDO, MF_BYCOMMAND | MF_GRAYED);
	}

	if (CanRedo())
	{
		sprintf(buffer, "Redo %s (Ctrl+Y)", undoStack[(MAX_UNDO+undoPtr+1)%MAX_UNDO].desc);
		GetMenu()->ModifyMenu(ID_EDIT_REDO, MF_BYCOMMAND, ID_EDIT_REDO, buffer);
		GetMenu()->EnableMenuItem(ID_EDIT_REDO, MF_BYCOMMAND | MF_ENABLED);
	}
	else
	{
		sprintf(buffer, "Redo (Ctrl+Y)");
		GetMenu()->ModifyMenu(ID_EDIT_REDO, MF_BYCOMMAND, ID_EDIT_REDO, buffer);
		GetMenu()->EnableMenuItem(ID_EDIT_REDO, MF_BYCOMMAND | MF_GRAYED);
	}
};

void CaveMapEditDlg::OnSortentitiiesbyIdnumber()
{
	entitySort = ENTITY_SORT_ID;
	GetMenu()->CheckMenuRadioItem(ID_SORTENTITIIESBY_CATEGORY, ID_SORTENTITIIESBY_IDNUMBER, ID_SORTENTITIIESBY_CATEGORY+entitySort, MF_BYCOMMAND);

	//stupid buble sort, slow but easy to code
	int i, j;
	NPC_extras temp;
	m_EntityInfo.init = true;
	for (i = 0; i < NPC_TYPE_COUNT*2; i++)
	{
		for (j = 0; j < NPC_TYPE_COUNT*2-1; j++)
		{
			if (m_EntityInfo.npcTable.extra[j].ID > m_EntityInfo.npcTable.extra[j+1].ID)
			{
				temp = m_EntityInfo.npcTable.extra[j];
				m_EntityInfo.npcTable.extra[j] = m_EntityInfo.npcTable.extra[j+1];
				m_EntityInfo.npcTable.extra[j+1] = temp;
			}
		}
	}
	ListEntityTypes();
}

void CaveMapEditDlg::OnSortentitiiesbyCategory()
{
	entitySort = ENTITY_SORT_CATEGORY;
	GetMenu()->CheckMenuRadioItem(ID_SORTENTITIIESBY_CATEGORY, ID_SORTENTITIIESBY_IDNUMBER, ID_SORTENTITIIESBY_CATEGORY+entitySort, MF_BYCOMMAND);

	//stupid buble sort, slow but easy to code
	int i, j;
	NPC_extras temp;
	m_EntityInfo.init = true;
	for (i = 0; i < NPC_TYPE_COUNT*2; i++)
	{
		for (j = 0; j < NPC_TYPE_COUNT*2-1; j++)
		{
			if (m_EntityInfo.npcTable.extra[j].type > m_EntityInfo.npcTable.extra[j+1].type)
			{
				temp = m_EntityInfo.npcTable.extra[j];
				m_EntityInfo.npcTable.extra[j] = m_EntityInfo.npcTable.extra[j+1];
				m_EntityInfo.npcTable.extra[j+1] = temp;
			}
		}
	}
	ListEntityTypes();
}

void CaveMapEditDlg::OnSortentitiiesbyName()
{
	entitySort = ENTITY_SORT_ALPHA;
	GetMenu()->CheckMenuRadioItem(ID_SORTENTITIIESBY_CATEGORY, ID_SORTENTITIIESBY_IDNUMBER, ID_SORTENTITIIESBY_CATEGORY+entitySort, MF_BYCOMMAND);

	//stupid buble sort, slow but easy to code
	int i, j;
	NPC_extras temp;
	m_EntityInfo.init = true;
	for (i = 0; i < NPC_TYPE_COUNT*2; i++)
	{
		for (j = 0; j < NPC_TYPE_COUNT*2-1; j++)
		{
			if (m_EntityInfo.npcTable.extra[j].desc > m_EntityInfo.npcTable.extra[j+1].desc)
			{
				temp = m_EntityInfo.npcTable.extra[j];
				m_EntityInfo.npcTable.extra[j] = m_EntityInfo.npcTable.extra[j+1];
				m_EntityInfo.npcTable.extra[j+1] = temp;
			}
		}
	}
	ListEntityTypes();
}


void CaveMapEditDlg::ListEntityTypes()
{
	((CComboBox*)m_EntityInfo.GetDlgItem(COMBO_ENTITY_TYPE))->ResetContent();

	CString type;
	char temp[512];
	int i;

	for (i = 0; i < NPC_TYPE_COUNT*2; i++)
	{
		if (m_EntityInfo.npcTable.extra[i].loaded)
		{
			if (entitySort == ENTITY_SORT_CATEGORY && type != m_EntityInfo.npcTable.extra[i].type)//list the new Category
			{
				type = m_EntityInfo.npcTable.extra[i].type;
				sprintf(temp, "****** %s ******************************", m_EntityInfo.npcTable.extra[i].type.GetBuffer());
				((CComboBox*)m_EntityInfo.GetDlgItem(COMBO_ENTITY_TYPE))->AddString(temp);
			}
			sprintf(temp, "%i - %s", m_EntityInfo.npcTable.extra[i].ID, m_EntityInfo.npcTable.extra[i].desc.GetBuffer());
			((CComboBox*)m_EntityInfo.GetDlgItem(COMBO_ENTITY_TYPE))->AddString(temp);
		}
	}
	m_EntityInfo.init = true;
	sprintf(temp, "%i - ", m_EntityInfo.type);
	((CComboBox*)m_EntityInfo.GetDlgItem(COMBO_ENTITY_TYPE))->SetCurSel(
		((CComboBox*)m_EntityInfo.GetDlgItem(COMBO_ENTITY_TYPE))->FindString(-1, temp));
}

