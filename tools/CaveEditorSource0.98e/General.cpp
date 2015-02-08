//File created by David Shafer (C) 2006
#include "stdafx.h"
#include <fstream>

#include "General.h"
#include "GeneralDefines.h"

CString CS_TILE_TYPE_NAME = "CE_Tiles.png";

#define CS_TILES_INI_FILE "CE_Tiles.ini"
#define CS_NPC_INI_FILE   "CE_NPC.ini"
#define CS_NPC_FILE       "CE_NPC.txt"
#define CS_TSC_INFO_FILE  "CE_TSC_Info.txt"
#define ERROR_FILE        "CE_error.log"

//************ Begin Global Variables *******************
//exe offsets
int CAVE_MAP_INFO = 0x0937B0;//this is where the map info is stored in file

CString filePath;
CString progPath;
CString EXEname;

CaveStoryEXE exe;


int mapCount = 95;

//semi constants
int  CS_DEFAULT_SCALE = 2;
char* CS_DEFAULT_PBM_EXT;
bool CS_DEFAULT_TILESET_WINDOW = true;
bool CS_DEFAULT_WHEEL_ZOOM = true;
int  CS_DEFAULT_TILESET_SCALE = 1;
BYTE CS_DEFAULT_TILETYPE_ALPHA = 128;
bool CS_DEFUALT_READ_ONLY = false;
int CS_GRAPHIC_COLOR_DEPTH = 32;

//************* End Global Variables ********************

//REG_NAME has been created to reduce the number of switch statements in the decoding fuctions
//the coding is slightly more complicated but it reduces several lines of code to a single line
char* REG_NAME[5][8] = {
{"0AX", "0CX", "0DX", "0BX", "0SP", "0BP", "0SI", "0DI"},//0 byte registers (don't exist)
{ "AL",  "CL",  "DL",  "BL",  "AH",  "CH",  "DH",  "BH"},//1 byte registers
{ "AX",  "CX",  "DX",  "BX",  "SP",  "BP",  "SI",  "DI"},//2 byte registers
{"3AX", "3CX", "3DX", "3BX", "3SP", "3BP", "3SI", "3DI"},//3 byte registers (don't exist)
{"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"} //4 byte registers
};


extern CString filePath;

using namespace std;

TileKey tileKey;

char CaveBitMap::extra8[8];

int CaveTileSet::count = 0;
CaveBitMap CaveTileSet::typeMap;

CaveMapINFO& CaveMapINFO::operator=(CaveMapINFO& info)
{
	strcpy(background, info.background);
	backType = info.backType;
	bossNum = info.bossNum;
	strcpy(caption, info.caption);
	strcpy(fileName, info.fileName);
	strcpy(NPCset1, info.NPCset1);
	strcpy(NPCset2, info.NPCset2);
	strcpy(tileset, info.tileset);
	strcpy(unknown, info.unknown);
	return info;
};


CaveBitMap::CaveBitMap()
{
	file[0] = '\0';
}

CaveBitMap::~CaveBitMap()
{
	if (!IsNull())//if bitmap already loaded destroy it first
		Destroy();
}


void CaveBitMap::Resize(int w, int h)
{
	CImage temp;//new image

	RGBQUAD RGBquad[256];

	temp.Create(w, h, GetBPP());

	GetColorTable(0, GetMaxColorTableEntries(), RGBquad);//copy palette
	temp.SetColorTable(0, GetMaxColorTableEntries(), RGBquad);

	BitBlt(temp.GetDC(), 0, 0);//copy image
	temp.ReleaseDC();

	Destroy();//destroy old bitmap and attach new one
	Attach(temp.Detach());
}


//loads Bitmap replacing any existing one
bool CaveBitMap::Load(const char* f)
{
	if (!IsNull())//if bitmap already loaded destroy it first
		Destroy();

	strcpy(file, f);
	HRESULT h = CImage::Load(f);
	if (h != S_OK)
		return false;
	return true;
}

//saves BitMap to specified file
bool CaveBitMap::Save(const char* newfile)
{
	CFile f;
	CFileException e;

	CFileStatus status;

	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(newfile))
	{
		CFile::GetStatus(newfile, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(newfile, status);
	}

	CImage::Save(newfile, ImageFormatBMP);//regular image save

	if (!f.Open(newfile, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, &e))
		return false;
	f.Seek(f.GetLength(),0);
	f.Write(extra8, 8);//add extra "(C)Pixel" data
	f.Close();

	return true;
};

//saves BitMap to file it was loaded from
bool CaveBitMap::Save()
{
	return Save(file);
};


//draws Part of Bitmap to Device Context
void CaveBitMap::DrawPart(int x, int y, int w, int h, CDC* cdc, int xCDC, int yCDC, bool trans, int color)
{
	BLENDFUNCTION blend;
	if (trans)
	{
		switch (GetBPP())
		{
		case 4:
		case 8:
			SetTransparentColor(0);
			Draw(cdc->m_hDC, xCDC, yCDC, w, h, x, y, w, h);
			break;
		case 32:
			blend.AlphaFormat = AC_SRC_ALPHA;
			blend.BlendFlags = 0;
			blend.BlendOp = AC_SRC_OVER;
			blend.SourceConstantAlpha = 255;

			AlphaBlend(cdc->m_hDC, xCDC, yCDC, w, h, GetDC(), x, y, w, h, blend);
			ReleaseDC();
//			TransparentBlt(cdc->m_hDC, xCDC, yCDC, w, h, GetDC(), x, y, w, h, color);
//			ReleaseDC();
			break;
		default:
			Draw(cdc->m_hDC, xCDC, yCDC, w, h, x, y, w, h);
			break;
		};
	}
	else
	if (GetBPP() == 32)//this should mean alpha channel
	{
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.BlendFlags = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = 255;
		cdc->FillSolidRect(xCDC, yCDC, w, h, 0);
		AlphaBlend(cdc->m_hDC, xCDC, yCDC, w, h, GetDC(), x, y, w, h, blend);
		ReleaseDC();
	}
	else
		BitBlt(cdc->m_hDC, xCDC, yCDC, w, h, x, y);
};

//draws Part of Bitmap to Device Context
void CaveBitMap::DrawPart(RECT rectSrc, CDC* cdc, RECT rectDest, bool trans, int color)
{
	int xCDC = rectDest.left;
	int yCDC = rectDest.top;
	int wCDC = rectDest.right-rectDest.left;
	int hCDC = rectDest.bottom-rectDest.top;

	int x = rectSrc.left;
	int y = rectSrc.top;
	int w = rectSrc.right-rectSrc.left;
	int h = rectSrc.bottom-rectSrc.top;

	BLENDFUNCTION blend;
	if (trans)
	{
		switch (GetBPP())
		{
		case 4:
		case 8:
			SetTransparentColor(0);
			Draw(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, x, y, w, h);
			break;
		case 32:
			blend.AlphaFormat = AC_SRC_ALPHA;
			blend.BlendFlags = 0;
			blend.BlendOp = AC_SRC_OVER;
			blend.SourceConstantAlpha = 255;

			AlphaBlend(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, GetDC(), x, y, w, h, blend);
			ReleaseDC();
			break;
//			TransparentBlt(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, GetDC(), x, y, w, h, color);
//			ReleaseDC();
		default:
			Draw(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, x, y, w, h);
			break;
		};
	}
	else
	if (GetBPP() == 32)//this should mean alpha channel
	{
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.BlendFlags = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = 255;
		cdc->FillSolidRect(xCDC, yCDC, w, h, 0);
		AlphaBlend(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, GetDC(), x, y, w, h, blend);
		ReleaseDC();
	}
	else
		BitBlt(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, x, y);
}

TileKey::TileKey()
{
	ifstream file;
	char buffer[256];

	//this preserves pathing and loads from program directory
	::GetCurrentDirectory(255, buffer);
	::SetCurrentDirectory(progPath.GetString());
	file.open(CS_TILES_INI_FILE);
	::SetCurrentDirectory(buffer);

	int i, temp;

	file.getline(buffer, 256); //remove header line

	for (i = 0; i < 256; i++)
	{
		file >> temp;
		file >> buffer;
		file >> temp;
		file >> temp;
		file >> temp;

		file >> temp;
		file >> temp;
		file >> temp;
		file >> temp;
		file >> temp;//ignore first ten entries

		file >> temp;
		sPC[i] = temp;
		file >> temp;
		sNPC[i] = temp;
		file >> temp;
		water[i] = temp;
		file >> temp;
		front[i] = temp;
		file >> temp;
		seen[i] = temp;
		file.getline(buffer, 256); //remove description
	}

	file.close();
}

NPC_extras::NPC_extras()
{
	 loaded = false;
}


NPC_extras& NPC_extras::operator=(NPC_extras& assign)
{
	loaded = assign.loaded;
	ID = assign.ID;
	shortDesc1 = assign.shortDesc1;
	shortDesc2 = assign.shortDesc2;
	type = assign.type;
	option1 = assign.option1;
	option2 = assign.option2;
	desc = assign.desc;
	notes = assign.notes;
	return assign;
}


NPCtable::NPCtable()
{
	ifstream file;

	//this preserves pathing and loads from program directory
	char buffer[255];
	::GetCurrentDirectory(255, buffer);
	::SetCurrentDirectory(progPath.GetString());
	file.open(CS_NPC_FILE);
	::SetCurrentDirectory(buffer);

	int id;
	file.getline(buffer, 256);
	while (file)
	{
		file >> id;
		file.get();
		if (!file)
			break;
		extra[id].ID = id;
		file.getline(buffer, 256, '\t');
		extra[id].shortDesc1 = buffer;
		file.getline(buffer, 256, '\t');
		extra[id].shortDesc2 = buffer;
		file.getline(buffer, 256, '\t');
		extra[id].type = buffer;
		file.getline(buffer, 256, '\t');
		extra[id].option1 = buffer;
		file.getline(buffer, 256, '\t');
		extra[id].option2 = buffer;
		file.getline(buffer, 256, '\t');
		extra[id].desc = buffer;
		file.getline(buffer, 256);
		extra[id].notes = buffer;

		extra[id].loaded = true;
	}

	file.close();


}


NPCtable::~NPCtable()
{
}

//loads NPC table
bool NPCtable::load(const char* npcName)
{
	int i;

	fileName = npcName;
	char temp[100];
	sprintf(temp, ".\\data\\%s", npcName);

	CFile file;
	if (!file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
		return false;

	//unlike other files, each individual value is stored sequentially instead of in structures
	for (i = 0; i < NPC_TYPE_COUNT; i++)//read flags
	{
		file.Read(&(entity[i].flags), 2);
	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//read hp
	{
		file.Read(&(entity[i].hp), 2);
	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//tileset data ???
	{
		file.Read(&(entity[i].tileSet), 1);
    	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//graphic and death sound
	{
//		file.Read(&(entity[i].hurtSound), 1);
		file.Read(&(entity[i].deathSound), 1);
	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//graphic and death sound
	{
		file.Read(&(entity[i].hurtSound), 1);
//		file.Read(&(entity[i].deathSound), 1);
	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//death graphic
	{
		file.Read(&(entity[i].deathPBM), 1);
	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//???
	{
		file.Read(&(entity[i].weaponDrop), 4);
	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//attack power
	{
		file.Read(&(entity[i].attack), 4);
	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//position data
	{
		file.Read(&(entity[i].hitLeft), 1);
		file.Read(&(entity[i].hitTop), 1);
		file.Read(&(entity[i].hitRight), 1);
		file.Read(&(entity[i].hitBottom), 1);
	}
	for (i = 0; i < NPC_TYPE_COUNT; i++)//hit box???
	{
		file.Read(&(entity[i].hitX), 1);
		file.Read(&(entity[i].hitY), 1);
		file.Read(&(entity[i].cenX), 1);
		file.Read(&(entity[i].cenY), 1);
	}
	file.Close();

	return true;
}


//saves NPC table
bool NPCtable::save()
{
	return save("npc.tbl");
}

//saves to new name
bool NPCtable::save(const char* newName)
{
	int i;
	char temp[256];
	sprintf(temp, ".\\data\\%s", newName);

	CFile file;

	CFileStatus status;
	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(newName))
	{
		CFile::GetStatus(temp, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(temp, status);
	}

	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		return false;

	for (i = 0; i < NPC_TYPE_COUNT; i++)//write flags
		file.Write(&(entity[i].flags), 2);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//write hp
		file.Write(&(entity[i].hp), 2);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//tileset data ???
		file.Write(&(entity[i].tileSet), 1);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//graphic and death sound
		file.Write(&(entity[i].deathSound), 1);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//graphic and death sound
		file.Write(&(entity[i].hurtSound), 1);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//death graphic
		file.Write(&(entity[i].deathPBM), 1);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//???
		file.Write(&(entity[i].weaponDrop), 4);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//attack power
		file.Write(&(entity[i].attack), 4);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//position data
	{
		file.Write(&(entity[i].hitLeft), 1);
		file.Write(&(entity[i].hitTop), 1);
		file.Write(&(entity[i].hitRight), 1);
		file.Write(&(entity[i].hitBottom), 1);
	}

	for (i = 0; i < NPC_TYPE_COUNT; i++)//hit box???
	{
		file.Write(&(entity[i].hitX), 1);
		file.Write(&(entity[i].hitY), 1);
		file.Write(&(entity[i].cenX), 1);
		file.Write(&(entity[i].cenY), 1);
	}

	file.Close();
/*
	CFile f;
	::GetCurrentDirectory(255, temp);
	::SetCurrentDirectory(progPath.GetString());

	f.Open(CS_NPC_INI_FILE, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
	f.Write(extra, sizeof(NPC_extras)*NPC_TYPE_COUNT);
	f.Close();
//*/
	::SetCurrentDirectory(temp);
	return true;
}

//displays NPC sprite based on upper left corner of tile
void NPCtable::displaySprite(int x, int y, int index, double scale, CDC* cdc2, CaveBitMap& tileset, CaveBitMap& NPCset1, CaveBitMap& NPCset2)
{
	if (cdc2 == NULL)
		return;
	if (exe.eRectCount[index] == 0)
		return;
	if (exe.eRectDisplay[index][0].bottom == 0 || exe.eRectDisplay[index][0].right == 0)
		return;

	CDC cdc;
	cdc.CreateCompatibleDC(cdc2);
	CBitmap* pOldBmp;
	CBitmap bmpEntity;

	RECT rect;
	RECT rect2;

	bmpEntity.CreateBitmap(512, 512, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	pOldBmp = (CBitmap *)(cdc.SelectObject(&bmpEntity));

//	int x = int((caveMap.entity[index].x*16+8-npcTable.entity[index].hitX - offX)*scale);
//	int y = int((caveMap.entity[index].y*16+8-npcTable.entity[index].hitY - offY)*scale);

	rect.left = x+(8-entity[index].hitX)*scale;
	rect.top = y+(8-entity[index].hitY)*scale;
	rect.right = rect.left+(exe.eRectDisplay[index][0].right-exe.eRectDisplay[index][0].left)*scale;
	rect.bottom = rect.top+(exe.eRectDisplay[index][0].bottom-exe.eRectDisplay[index][0].top)*scale;

	rect2.left = 0;
	rect2.top = 0;
	rect2.right = rect.right-rect.left;
	rect2.bottom = rect.bottom-rect.top;

	switch (entity[index].tileSet)
	{
	case 0:
	case 1://Title.pbm
		exe.spriteSheet[1].DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 2://Map Tileset
		tileset.DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 3://Map Tileset
		tileset.DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 4:
	case 5:
	case 6://Fade.pbm
		exe.spriteSheet[6].DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 7:
	case 8:
	case 9:
	case 10://MyChar.pbm
		exe.spriteSheet[10].DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 11://Bullet.pbm
		exe.spriteSheet[11].DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19://Caret.pbm
		exe.spriteSheet[19].DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 20://NpcSym.pbm
		exe.spriteSheet[20].DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 21://Map NPC Set 1
		NPCset1.DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 22://Map NPC Set 2
		NPCset2.DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
		break;
	case 23://NpcRegu.pbm
		exe.spriteSheet[23].DrawPart(exe.eRectDisplay[index][0], &cdc, rect2);
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
	cdc2->TransparentBlt(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, &cdc,
			    rect2.left, rect2.top, rect2.right-rect2.left, rect2.bottom-rect2.top, 0);
//	cdc2.AlphaBlend(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, &cdc, 0,0, rect2.right, rect2.bottom, blend);

	(cdc.SelectObject(&pOldBmp));
	bmpEntity.DeleteObject();
}

CaveTileSet::CaveTileSet()
{
	typData = NULL;
	tileNum = 0;
	tileName[0] = '\0';
	if (count == 0)
		typeMap.Load(CS_TILE_TYPE_NAME);
	count++;
}

CaveTileSet::~CaveTileSet()
{
	count--;

	clean(typData);
}


//loads tileset
bool CaveTileSet::load(const char* tileSetName)
{
	int i;
	char temp[256];

	if (typData != NULL)
		delete[] typData;

	strcpy(tileName, tileSetName);

	//loads bitmap data
	sprintf(temp, ".\\data\\Stage\\Prt%s.%s", tileName, CS_DEFAULT_PBM_EXT);
	if (!tileData.Load(temp))
		return false;

	tileNum = (tileData.GetWidth()/16)*(tileData.GetHeight()/16);//retrieves number of tiles from bitmap data

	//load tile type data
	typData = new unsigned char[tileNum];
	sprintf(temp, ".\\data\\Stage\\%s.pxa", tileName);
	CFile file;
	if (!file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
		return false;

	for (i = 0; i < tileNum; i++)//read the data
		file.Read(typData+i, 1);

	file.Close();

	return true;
}

//saves tileset
bool CaveTileSet::save()
{
	return save(tileName);
}

//saves tileset with new name
bool CaveTileSet::save(const char* tileSetName)
{
	int i;
	char temp[256];
	
	strcpy(tileName, tileSetName);

	//saves bitmap data
	if (!tileData.Save())
		return false;

	//save tile type data
	sprintf(temp, ".\\data\\Stage\\%s.pxa", tileName);

	CFileStatus status;
	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(tileSetName))
	{
		CFile::GetStatus(temp, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(temp, status);
	}

	CFile file;
	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		return false;

	for (i = 0; i < tileNum; i++)//write the data
		file.Write(typData+i, 1);

	file.Close();

	return true;
}



//displays tile to specified coordinates on device context
void CaveTileSet::displayTile(int index, int x, int y, CDC* cdc, bool trans)
{
	int tileWide = tileData.GetWidth() >> 4;// get how many tiles wide the pbm data is
	double tileHigh = tileData.GetHeight() / 16.0; //for some reason a tileset pbm might not be a multiple of 16 (like PrtBarr.pbm)
	int _x, _y;

	_x = index%tileWide;
	_y = index/tileWide;

	tileData.DrawPart(_x*16, _y*16, 16, 16, cdc, x, y, trans, 0x000);
}

//displays tile type as tile
void CaveTileSet::displayTileType(int index, int x, int y, CDC* cdc, bool trans)
{
	int tileWide = 16;//tile.bmp is 16x16 tiles
	int tileHigh = 16;
	int _x, _y;

	index = typData[index];

	_x = index%tileWide;
	_y = index/tileWide;

/*
	if (trans)
		typeMap.Draw(*cdc, x, y, 16, 16, _x*16, _y*16, 16, 16);
	else
//*/
		typeMap.DrawPart(_x*16, _y*16, 16, 16, cdc, x, y, trans, 0x000);
}

//resizes tile set to hold at least size number of tiles
void CaveTileSet::resize(int size)
{
	int newSize, i;
	unsigned char* newType;

	newSize = size % (tileData.GetWidth() >> 4);
	if (newSize > 0)
		newSize = size + (tileData.GetWidth() >> 4) - newSize;//round up to nearest complete tile row
	else
		newSize = size;

	newType = new unsigned char[newSize];

	for (i = 0; i < newSize; i++)
	{
		if (i >= tileNum)
			newType[i] = 0;
		else
			newType[i] = typData[i];
	}

	delete [] typData;
	typData = newType;
	tileNum = newSize;

	tileData.Resize(tileData.GetWidth(), (tileNum << 8)/tileData.GetWidth());

}


CaveMap::CaveMap()
{
	tiles = NULL;
	entity = NULL;
	width = 0;
	height = 0;
}


CaveMap::~CaveMap()
{
	clean(entity);
	clean(tiles);
}

//reloads map files
bool CaveMap::reload()
{
	char temp[1024];
	//read tileset data
	if (!tileSet.load(mapInfo.tileset))
		return false;

	//read background
	sprintf(temp, ".\\data\\%s.%s", mapInfo.background, CS_DEFAULT_PBM_EXT);
	return background.Load(temp);
}

//loads map
bool CaveMap::load(CaveMapINFO* info)
{
	oldFileName = info->fileName;
	char temp[1024];
	if (entity != NULL)
		delete[] entity;
	if (tiles != NULL)
		delete[] tiles;

	entity = NULL;
	tiles = NULL;

	mapInfo = *info;

	//Start with tile data
	sprintf(temp, ".\\data\\Stage\\%s.pxm", mapInfo.fileName);
	CFile file;
	if (!file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
		return false;

	file.Read(temp, 4);//remove useless header data

	width = 0;
	file.Read(&width, 2);
	height = 0;
	file.Read(&height, 2);

	tiles = new unsigned char[width*height];

	file.Read(tiles, width*height);
	file.Close();//finished reading tile data

	//begin reading entity data
	sprintf(temp, ".\\data\\Stage\\%s.pxe", mapInfo.fileName);
	if (!file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
		return false;

	file.Read(temp, 4);//remove useless header data

	entityNum = 0;
	file.Read(&entityNum, 2);
	file.Read(temp, 2);//remove more useless header data

	entity = new Entity[entityNum];

	file.Read(entity, entityNum*sizeof(Entity));//read it all at once
	file.Close();

	//read tileset data
	if (!tileSet.load(mapInfo.tileset))
		return false;


	//read background
	sprintf(temp, ".\\data\\%s.%s", mapInfo.background, CS_DEFAULT_PBM_EXT);
	if (!background.Load(temp))
		return false;

	sprintf(temp, ".\\data\\Npc\\Npc%s.%s", mapInfo.NPCset1, CS_DEFAULT_PBM_EXT);
	if (!NPCset1.Load(temp))
		return false;

	sprintf(temp, ".\\data\\Npc\\Npc%s.%s", mapInfo.NPCset2, CS_DEFAULT_PBM_EXT);
	if (!NPCset2.Load(temp))
		return false;

	return true;
}

//creates a new map
bool CaveMap::create(CaveMapINFO* info, int w, int h)
{
	int i;
	oldFileName = info->fileName;
	char temp[1024];

	clean(entity);
	clean(tiles);

	entity = NULL;
	tiles = NULL;

	mapInfo = *info;

	width = w;
	height = h;
	tiles = new unsigned char[width*height];

	//clear new map's tile data
	for (i = 0; i < width*height; i++)
		tiles[i] = 0;
	
	entityNum = 1;//we'll start with one dummy entity @ 0,0
	entity = new Entity[entityNum];
	entity[0].x = 0;
	entity[0].y = 0;
	entity[0].event = 0;
	entity[0].flagID = 0;
	entity[0].flags = 0;
	entity[0].npc = 0;

	return save();
}

//saves map
bool CaveMap::save()
{
	return save(mapInfo.fileName);
}

//saves map with new name
bool CaveMap::save(const char* map)
{
	char temp[1024];
	char temp2[1024];

	//Start with tile data
	sprintf(temp, ".\\data\\Stage\\%s.pxm", map);
	CFile file;

	CFileStatus status;
	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(map))
	{
		CFile::GetStatus(temp, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(temp, status);
	}

	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		return false;

	strcpy(temp, "PXM");
	temp[3] = 0x10;
	file.Write(temp, 4);//add header data

	file.Write(&width, 2);
	file.Write(&height, 2);

	file.Write(tiles, width*height);
	file.Close();//finish writing tile data

	//begin reading entity data
	sprintf(temp, ".\\data\\Stage\\%s.pxe", map);

	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(map))
	{
		CFile::GetStatus(temp, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(temp, status);
	}

	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		return false;

	strcpy(temp, "PXE");
	temp[3] = 0x00;
	file.Write(temp, 4);//add header data

	file.Write(&entityNum, 2);

	temp[0] = 0x00;
	temp[1] = 0x00;
	file.Write(temp, 2);//add more header data

	file.Write(entity, entityNum*sizeof(Entity));//write it all at once
	file.Close();

	if (oldFileName != map)//new file name is different from old file name
	{
		sprintf(temp, ".\\data\\Stage\\%s.pxm", oldFileName);
		file.Remove(temp);
		sprintf(temp, ".\\data\\Stage\\%s.pxe", oldFileName);
		file.Remove(temp);
		sprintf(temp, ".\\data\\Stage\\%s.tsc", oldFileName);
		sprintf(temp2, ".\\data\\Stage\\%s.tsc", map);
		file.Rename(temp, temp2);
	}
	oldFileName = map;//this is the new "old" name

	return true;
}

//set tile to newTile and returns old tile value
char CaveMap::setTile(int x, int y, char newTile)
{
	char temp = tiles[(x+y*width)%(width*height)];
	tiles[(x+y*width)%(width*height)] = newTile;
	return temp;
}

//returns tile value
char CaveMap::getTile(int x, int y)
{
	return tiles[(x+y*width)%(width*height)];
}

//resizes the map to the new width and height, buffering with tile 0 where needed
void CaveMap::resize(int w, int h)
{
	int i, j;
	unsigned char* newTiles;
	newTiles = new unsigned char[w*h];

	for (i = 0; i < w; i++)
	{
		for (j = 0; j < h; j++)
		{
			if (i >= width || j >= height)//if outside current bounds
				newTiles[i + j*w] = 0;
			else
				newTiles[i + j*w] = tiles[i + j*width];
		}
	}

	if (tiles != NULL)
		delete[] tiles;
	tiles = newTiles;

	width = w;
	height = h;
}

//shifts map by dx and dy, filling edges with tile 0 if wrap is turned off
void CaveMap::shift(int dx, int dy, bool wrap)
{
	//I'm being lazy here, using memory to make all the math go away
	int i, j;
	unsigned char* newTiles;
	newTiles = new unsigned char[width*height];

	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			if (wrap)//well some math anyways...
				newTiles[(i+dx+width*100)%width + ((j+dy+height*100)%height)*width] = tiles[i + j*width];
			else
			if (i+dx < 0 || i+dx >= width || j+dy < 0 || j+dy >= height) 
				newTiles[(i+dx+width*100)%width + ((j+dy+height*100)%height)*width] = 0;
			else
				newTiles[(i+dx+width*100)%width + ((j+dy+height*100)%height)*width] = tiles[i + j*width];
		}
	}
	for (i = 0; i < entityNum; i++)
	{
		if (wrap)
		{
			entity[i].x = (entity[i].x+dx+width*100)%width;
			entity[i].y = (entity[i].y+dy+height*100)%height;
		}
		else
		{
			if (entity[i].x+dx < 0)
				entity[i].x = 0;
			else
			if (entity[i].x+dx >= width)
				entity[i].x = width-1;
			else
				entity[i].x += dx;

			if (entity[i].y+dy < 0)
				entity[i].y = 0;
			else
			if (entity[i].y+dy >= height)
				entity[i].y = height-1;
			else
				entity[i].y += dy;
		}
	}
	delete[] tiles;
	tiles = newTiles;
}

//all addEntity functions return index of added entity or -1 on failure
int CaveMap::addEntity()
{
	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum].x = 0;
	newEntity[entityNum].y = 0;
	newEntity[entityNum].event = 0;
	newEntity[entityNum].flagID = 0;
	newEntity[entityNum].flags = 0;
	newEntity[entityNum].npc = 0;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all addEntity functions return index of added entity or -1 on failure
int CaveMap::addEntity(Entity e)
{
	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum] = e;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all addEntity functions return index of added entity or -1 on failure
int CaveMap::addEntity(Entity e, int index)
{
	Entity* newEntity = new Entity[entityNum+1];
	if (index > entityNum)
		index = entityNum;
	if (index < 0)
		index = 0;

	for (int i = 0; i < entityNum; i++)
	{
		if (i < index)
			newEntity[i] = entity[i];
		else
			newEntity[i+1] = entity[i];
	}
	newEntity[index] = e;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all addEntity functions return index of added entity or -1 on failure
int CaveMap::addEntity(WORD x, WORD y, WORD npcType, WORD event, WORD flagID, WORD flags)
{
	if (x >= width || y >= height)
		return -1;

	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum].x = x;
	newEntity[entityNum].y = y;
	newEntity[entityNum].event = event;
	newEntity[entityNum].flagID = flagID;
	newEntity[entityNum].flags = flags;
	newEntity[entityNum].npc = npcType;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all copyEntity functions return index of added entity or -1 on failure
int CaveMap::copyEntity(int index)
{
	if (index < 0 || index >= entityNum)
		return -1;

	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum] = entity[index];

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all copyEntity functions return index of added entity or -1 on failure
int CaveMap::copyEntity(int index, int x, int y)
{
	if (index < 0 || index >= entityNum || x >= width || y >= height)
		return -1;

	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum] = entity[index];
	newEntity[entityNum].x = x;
	newEntity[entityNum].y = y;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//returns number of entities after delete
int CaveMap::deleteEntity(int index)
{
	if (index < 0 || index >= entityNum)
		return -1;

	Entity* newEntity = new Entity[entityNum-1];

	int i;
	for (i = 0; i < index; i++)
		newEntity[i] = entity[i];

	for (i = index+1; i < entityNum; i++)
		newEntity[i-1] = entity[i];

	delete[] entity;
	entity = newEntity;

	entityNum--;
	return entityNum;
}


//displays part of the map to device context, includes transparency
void CaveMap::displayMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j;
	for (i = beginX; i < endX; i++)
	{
		for (j = beginY; j < endY; j++)
		{
			tileSet.displayTile(tiles[i+j*width], (i-beginX)*16+x, (j-beginY)*16+y, cdc, true);
		}
	}
}

void CaveMap::displayTile(CDC* cdc, int x, int y, int tx, int ty, double scale)
{
	int index = tiles[tx+ty*width];

	int tileWide = tileSet.tileData.GetWidth() >> 4;// get how many tiles wide the pbm data is
	double tileHigh = tileSet.tileData.GetHeight() / 16.0; //for some reason a tileset pbm might not be a multiple of 16 (like PrtBarr.pbm)
	int _x, _y;

	_x = index%tileWide;
	_y = index/tileWide;

	tileSet.tileData.StretchBlt(cdc->m_hDC, x, y, 16*scale, 16*scale, _x*16, _y*16, 16, 16);
};

//displays map to device context, includes transparency
void CaveMap::displayMap(CDC* cdc, int x, int y)
{
	int i, j;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			tileSet.displayTile(tiles[i+j*width], i*16+x, j*16+y, cdc, true);
		}
	}
}

//displays part of background map tiles to device context
void CaveMap::displayBackMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j;
	for (i = beginX; i < endX; i++)
	{
		for (j = beginY; j < endY; j++)
		{
			//if not a "front" tile and graphic are "seen" then display
			if (!tileKey.front[tileSet.typData[tiles[i+j*width]]] && tileKey.seen[tileSet.typData[tiles[i+j*width]]])
				tileSet.displayTile(tiles[i+j*width], (i-beginX)*16+x, (j-beginY)*16+y, cdc, true);
		}
	}
}

//displays background map tiles to device context
void CaveMap::displayBackMap(CDC* cdc)
{
	int i, j;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			//if not a "front" tile and graphic are "seen" then display
			if (!tileKey.front[tileSet.typData[tiles[i+j*width]]] && tileKey.seen[tileSet.typData[tiles[i+j*width]]])
				tileSet.displayTile(tiles[i+j*width], i*16, j*16, cdc, true);
		}
	}
}

//displays part of background map tiles to device context
void CaveMap::displayFrontMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j;
	for (i = beginX; i < endX; i++)
	{
		for (j = beginY; j < endY; j++)
		{
			//if not a "front" tile and graphic are "seen" then display
			if (tileKey.front[tileSet.typData[tiles[i+j*width]]] && tileKey.seen[tileSet.typData[tiles[i+j*width]]])
				tileSet.displayTile(tiles[i+j*width], (i-beginX)*16+x, (j-beginY)*16+y, cdc, true);
		}
	}
}

//displays foreground map tiles to device context
void CaveMap::displayFrontMap(CDC* cdc)
{
	int i, j;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			//if a "front" tile and graphic are "seen" then display
			if (tileKey.front[tileSet.typData[tiles[i+j*width]]] && tileKey.seen[tileSet.typData[tiles[i+j*width]]])
				tileSet.displayTile(tiles[i+j*width], i*16, j*16, cdc, true);
		}
	}
}


//displays part of background map tiles to device context
void CaveMap::displayTileTypes(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j;
	for (i = beginX; i < endX; i++)
		for (j = beginY; j < endY; j++)
			tileSet.displayTileType(tiles[i+j*width], (i-beginX)*16+x, (j-beginY)*16+y, cdc, true);
}

//displays foreground map tiles to device context
void CaveMap::displayTileTypes(CDC* cdc)
{
	int i, j;
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			tileSet.displayTileType(tiles[i+j*width], i*16, j*16, cdc, true);
}

//displays background image to device context
void CaveMap::displayBackground(CDC* cdc)
{
	int i, j, w, h;
	//Note we're not starting at 0,0 because the background begins centered on the first tile (8,8)
	for (i = 8-background.GetWidth(); i < width*16; i += background.GetWidth())
		for (j = 8-background.GetHeight(); j < height*16; j += background.GetHeight())
		{
			w = min(background.GetWidth(), width*16-i);
			h = min(background.GetHeight(), height*16-j);
			background.Draw(*cdc, i, j, w, h, 0, 0, w, h);
		}
}

TSC_Info::TSC_Info()
{
	ifstream file;

	char buffer[256];
	::GetCurrentDirectory(255, buffer);
	::SetCurrentDirectory(progPath.GetString());
	file.open(CS_TSC_INFO_FILE);
	::SetCurrentDirectory(buffer);

	count = -1;
	while (file)
	{
		count++;
		file.getline(cmd[count].command, 5, '\t');
		file >> cmd[count].args;
		file.get();
		file.getline(cmd[count].name, 32, '\t');
		file.getline(cmd[count].desc, 256, '\n');
	}

	file.close();
	
}

//returns offset up to count-1 of matching command
int TSC_Info::Find(const char* command)
{
	for (int i = 0; i < count; i++)
		if (strcmp(command, cmd[i].command) == 0)
			return i;

	return -1;
}

//returns offset up to count-1 of matching command
int TSC_Info::Find(CString command)
{
	for (int i = 0; i < count; i++)
		if (command == cmd[i].command)
			return i;

	return -1;
}

CaveStoryEXE::CaveStoryEXE()
{
	prevSize = -1;

	//Header Info
	DOSptr = NULL;

	//PE Header Info
	symbolTable = NULL;     //pointer to symbol tables

	header.NumSections = 0;
	header.NumSymbols = 0;
	mapCount = 0;
	mapInfo = NULL;
}

CaveStoryEXE::~CaveStoryEXE()
{
	int i;
	clean(DOSptr);
	clean(symbolTable);

	for (i = 0; i < header.NumSections; i++)
		delete[] section[i];
}

//this function read the entire EXE and groups data according to what it is.
bool CaveStoryEXE::Open(const char* file)
{
	CFile f;
	int i, j;
	char buffer[1024];
	unsigned short PEptr;
	int temp = 0;
	rsrcSection = -1;
	mapSection = -1;
	rdataSection = -1;
	bool SW = false;//set to true if reading a file edited by Sue's Workshop

	fileName = file;



	//can't open the file to edit it if its read only or in use
	if (!f.Open(file, CFile::modeRead | CFile::shareDenyNone))
		return false;

	prevSize = f.GetLength();

	f.Seek(PE_OFFSET, 0);//seek to PE offset
	f.Read(&PEptr, 2);//get offset value

	//Header Info
	f.Seek(0, 0);//back to begining of file
	DOSptr = new char[PEptr];
	f.Read(DOSptr, PEptr);


	f.Read(buffer, 4);//read PE signature
	PEptr += 4;//add signature to offset

	//PE Header Info
	f.Read(&header, sizeof(header));

	f.Read(&optHeader, sizeof(optHeader));

	for(i = 0; i < header.NumSections; i++)
	{
		f.Read(&(sectHeader[i]), sizeof(COFF_SectionHeader));

		//I just made this up to stand for the map section header for cave story map data
		if (strcmp(sectHeader[i].Name, ".csmap") == 0)
		{
			mapSection = i;
			mapCount = sectHeader[i].SizeOfRawData/sizeof(CaveMapINFO);
		}
		else //this is Sue's Workshop data
		if (strcmp(sectHeader[i].Name, ".swdata") == 0)
			mapSection = i;
		else//it's the resource data
		if(strcmp(sectHeader[i].Name, ".rsrc") == 0)
			rsrcSection = i;
		else//it's the read only data section
		if(strcmp(sectHeader[i].Name, ".rdata") == 0)
			rdataSection = i;
		else//it's the actual code
		if(strcmp(sectHeader[i].Name, ".data") == 0)
			dataSection = i;
	}

	//skipping symbol tables they will disappear in final exe file

	//Read Sections
	for(i = 0; i < header.NumSections; i++)
	{
		if (strcmp(sectHeader[i].Name, ".swdata") == 0)//special case for Sue's Workshop
		{
			sectHeader[i].SizeOfRawData -= 16;
			sectHeader[i].VirtualSize -= 16;
			sectHeader[i].PointerToRawData += 16;
			sectHeader[i].VirtualAddress += 16;
		}

		section[i] = new BYTE[sectHeader[i].SizeOfRawData];//allocate memory for section

		f.Seek(sectHeader[i].PointerToRawData, 0);
		f.Read(section[i], sectHeader[i].SizeOfRawData);
	}

	//Ok now we need to find out how many maps we actually have in the Sue Workshop edited file
	if (strcmp(sectHeader[mapSection].Name, ".swdata") == 0)
	{
		strcpy(sectHeader[mapSection].Name, ".csmap");
		i = 32;
		while (strcmp(&((char*)section[mapSection])[i], "\\empty") != 0 && ((char*)section[mapSection])[i] != 0xFF && i < sectHeader[mapSection].SizeOfRawData)
			i += 200;
		mapCount = i/sizeof(CaveMapINFO);

		sectHeader[mapSection].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
		sectHeader[mapSection].VirtualSize = mapCount*sizeof(CaveMapINFO);
		sectHeader[mapSection].SizeOfRawData = mapCount*sizeof(CaveMapINFO);
	}

	//map section doesn't exist in exe file so we'll next check in the default place
	if (mapSection == -1)
	{
		mapSection = i;
		mapCount = 95;//default stuff

		strcpy(sectHeader[i].Name, ".csmap");
		sectHeader[i].Name[7] = 0;//make sure all extra bytes in name are set to zero

		sectHeader[i].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;

		sectHeader[i].NumberOfLineNumbers = 0;
		sectHeader[i].PointerToLineNumbers = 0;

		sectHeader[i].NumberOfRelocations = 0;
		sectHeader[i].PointerToRelocations = 0;

		//this will be fixed later
		sectHeader[i].SizeOfRawData = mapCount*sizeof(CaveMapINFO);
		sectHeader[i].PointerToRawData = sectHeader[i-1].PointerToRawData+sectHeader[i-1].SizeOfRawData;

		sectHeader[i].VirtualSize = mapCount*sizeof(CaveMapINFO);
		sectHeader[i].VirtualAddress = sectHeader[i-1].VirtualAddress+sectHeader[i-1].VirtualSize;

		section[i] = new char[sectHeader[i].SizeOfRawData];//allocate memory for section

		f.Seek(CAVE_MAP_INFO, 0);
		f.Read((section[i]), sectHeader[i].SizeOfRawData);

		header.NumSections++;
	}
	f.Close();//done loading data out of file

	//Now we need to swap the locations of the map data and resource data to make the ORGs happy
	//This is the MOST critical part of  the exe modification.  Without this the exe is techinally
	//not a valid exe file because the header info is wrong.  That is what causes problems with
	//resource editing software.  If only SW did this...
	if (mapSection == header.NumSections-1)
	{
		sectHeader[header.NumSections] = sectHeader[rsrcSection];
		sectHeader[rsrcSection] = sectHeader[mapSection];
		sectHeader[mapSection] = sectHeader[header.NumSections];

		section[header.NumSections] = section[rsrcSection];
		section[rsrcSection] = section[mapSection];
		section[mapSection] = section[header.NumSections];

		mapSection = rsrcSection;
		rsrcSection = header.NumSections-1;
		//final offset checks are preformed during save
	}
	//Now you can use a resource editor to change ORGs without breaking the editor!

	int voffset = sectHeader[0].PointerToRawData+sectHeader[0].SizeOfRawData;
	int foffset = sectHeader[0].PointerToRawData+sectHeader[0].SizeOfRawData;

	//time to recalculate all the virtual and file points for the sections
	//start at 1 because section zero is NOT being changed (actually, neither is section 2 or 3)
	RSRC.header = (COFF_rsrcSectionHeader*) section[rsrcSection];
	RSRC.entry = (COFF_rsrcEntry*) &RSRC.header[1];

	for (i = 1; i < header.NumSections; i++)
	{
		//this will force the size to a multiple of the file alignment
		//(to help with making the exe data match PE spec)
		foffset = ((foffset - 1 + optHeader.FileAlignment) / optHeader.FileAlignment) * optHeader.FileAlignment;
		voffset = ((voffset - 1 + optHeader.FileAlignment) / optHeader.FileAlignment) * optHeader.FileAlignment;

		if (i == rsrcSection)//shift all resource entries by same amount as section shift
		{
			RSRC.down = new COFF_rsrcSection[RSRC.header->NumNamedEntries+RSRC.header->NumIDEntries];
			for (j = 0; j < RSRC.header->NumNamedEntries+RSRC.header->NumIDEntries; j++)
				ResourceShifter(&RSRC, j, voffset-sectHeader[rsrcSection].VirtualAddress);
		}
		sectHeader[i].PointerToRawData = foffset;
		sectHeader[i].VirtualAddress = voffset;

		foffset += sectHeader[i].SizeOfRawData;
		voffset += sectHeader[i].VirtualSize;
	}

	//Now that we have the basic file data it's time to get the meat of the exe
	mapInfo = (CaveMapINFO*) section[mapSection];//map info set

	//all offsets need to be cast to char or BYTE so that the offset works
	//((int*)A)[4] != (int) ((char*)A)[4]
	//
	//((int*)A)[4] is the forth integer starting from the address of A
	//
	//(int) ((char*)A)[4] is the forth byte from address A shown as an int
	//in this case (int) ((char*)A)[4] == ((int*)A)[1] since each int is 4 bytes long

	eJT = (int*)&((char*)section[dataSection])[SC_ENTITY_JUMP_TABLE_OFFSET];

	//changes the maximum script size to an much larger number
	//I don't know why it works but I'm not complaining (probably a mem allocation call)
	*((int*)&((char*)section[0])[CS_SCRIPT_LIMT]) = 500000;

	titleX = (unsigned char*)&((char*)section[0])[CS_TITLE_MAP_X_OFFSET];
	titleY = (unsigned char*)&((char*)section[0])[CS_TITLE_MAP_Y_OFFSET];
	titleMapEvent = (unsigned char*)&((char*)section[0])[CS_TITLE_MAP_EVENT_OFFSET];
	titleMap = (unsigned char*)&((char*)section[0])[CS_TITLE_MAP_OFFSET];

	startX = (unsigned char*)&((char*)section[0])[CS_START_PLAYER_X_OFFSET];
	startY = (unsigned char*)&((char*)section[0])[CS_START_PLAYER_Y_OFFSET];
	startMapEvent = (int*)&((char*)section[0])[CS_START_EVENT_OFFSET];
	startMap = (unsigned char*)&((char*)section[0])[CS_START_MAP_OFFSET];

	startMapEvent = (int*)&((char*)section[0])[CS_START_EVENT_OFFSET];
	startCurrentHP = (unsigned short*)&((char*)section[0])[CS_START_HEALTH_CURRENT_OFFSET];
	startMaxHP = (unsigned short*)&((char*)section[0])[CS_START_HEALTH_MAX_OFFSET];

	startFlags = (unsigned char*)&((char*)section[0])[CS_START_UNKOWN1];
	startFacing = (int*)&((char*)section[0])[CS_START_PLAYER_FACING];

	shotInfo = (ShotData*) ((char*)section[dataSection]+CS_SHOT_DATA_OFFSET);

	wepEnergy = (int*) ((char*)section[dataSection]+CS_WEAPON_LEVEL_OFFSET);


//0x9828c possible graphic data
//0x8F078 Shot data (+120 to .data section)
	SetStrings();

	sprintf(buffer, ".\\data\\%s.%s",string[CS_TITLE_STRING],CS_DEFAULT_PBM_EXT);
	spriteSheet[1].Load(buffer);

	sprintf(buffer, ".\\data\\%s.%s",string[CS_NPC_NPCSYM_STRING],CS_DEFAULT_PBM_EXT);
	spriteSheet[2].Load(buffer);

	sprintf(buffer, ".\\data\\%s.%s",string[CS_FADE_STRING],CS_DEFAULT_PBM_EXT);
	spriteSheet[6].Load(buffer);

	sprintf(buffer, ".\\data\\%s.%s",string[CS_MYCHAR_STRING],CS_DEFAULT_PBM_EXT);
	spriteSheet[10].Load(buffer);

	sprintf(buffer, ".\\data\\%s.%s",string[CS_BULLET_STRING],CS_DEFAULT_PBM_EXT);
	spriteSheet[11].Load(buffer);

	sprintf(buffer, ".\\data\\%s.%s",string[CS_CARET_STRING],CS_DEFAULT_PBM_EXT);
	spriteSheet[19].Load(buffer);

	sprintf(buffer, ".\\data\\%s.%s",string[CS_NPC_NPCSYM_STRING],CS_DEFAULT_PBM_EXT);
	spriteSheet[20].Load(buffer);

	sprintf(buffer, ".\\data\\%s.%s",string[CS_NPC_NPCREGU_STRING],CS_DEFAULT_PBM_EXT);
	spriteSheet[23].Load(buffer);

	memcpy(CaveBitMap::extra8, string[CS_C_PIXEL_STRING], 8);//this way bitmaps attach the right info to the end of images

	//this establishes the rectangles of the entities for display purposes
	IdentifyEntityRects();


	return true;
}

void CaveStoryEXE::SetStrings()
{
	//assign string pointers for use in reading external files
	int i = 0;
	string[i++] = &((char*)section[rdataSection])[CS_ARMSITEM_TSC_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PBM_FOLDER_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_DOUKUTSU_TIMESTAMP_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_CONFIG_DAT_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_VERSION_FORMAT_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_000_DAT_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PBM_FOLDER_STRING_OFFSET2];
	string[i++] = &((char*)section[rdataSection])[CS_SURFACE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PBM_FOLDER_STRING_OFFSET3];
	string[i++] = &((char*)section[rdataSection])[CS_DEBUG_OUT_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_CREDIT_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_CREDIT_TSC_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_CASTS_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_NPC_TBL_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_XXX_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_C_PIXEL_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PIXEL_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_MYCHAR_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_TITLE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_ARMSIMAGE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_ARMS_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_ITEMIMAGE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_STAGEIMAGE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_NPC_NPCSYM_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_NPC_NPCREGU_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_TEXTBOX_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_CARET_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_BULLET_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_FACE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_FADE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_CREDIT01_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PIXTONE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_DOUKUTSU_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_DATA_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_0_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PXM_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PROFILE_DAT_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_STAGESELECT_TSC_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_STAGE_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PRT_FOLDER_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PXA_FOLDER_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PXM_FOLDER_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_PXE_FOLDER_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_TSC_FOLDER_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_NPC_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_NPC_FOLDER_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_NPC_FOLDER_STRING_OFFSET2];
	string[i++] = &((char*)section[rdataSection])[CS_XXXX_STRING_OFFSET];

	// ORG name offsets here

	string[i++] = &((char*)section[rdataSection])[CS_HEAD_TSC_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_WINDOW_STRING_OFFSET];
	string[i++] = &((char*)section[rdataSection])[CS_LOADING_STRING_OFFSET];

	CS_DEFAULT_PBM_EXT = (string[CS_PBM_FOLDER_STRING])+(strlen(string[CS_PBM_FOLDER_STRING])-3);
}

void CaveStoryEXE::ResourceShifter(COFF_rsrcSection* rsrc, int offset, int shift)
{
	int i;

	if (rsrc->entry[offset].Dir != 0)//this is a directory address
	{
		rsrc->down[offset].header = (COFF_rsrcSectionHeader*) ((char*)section[rsrcSection]+rsrc->entry[offset].RVA);
		rsrc->down[offset].entry = (COFF_rsrcEntry*) &rsrc->down[offset].header[1];
		rsrc->down[offset].down = new COFF_rsrcSection[rsrc->down[offset].header->NumNamedEntries+rsrc->down[offset].header->NumIDEntries];
		for (i = 0; i < rsrc->down[offset].header->NumIDEntries + rsrc->down[offset].header->NumNamedEntries; i++)
		{
			rsrc->down[offset].up = rsrc;
			ResourceShifter(&(rsrc->down[offset]), i, shift);
		}
		return;
	}
	COFF_rsrcDataEntry* data;

	data = (COFF_rsrcDataEntry*) ((char*)section[rsrcSection]+rsrc->entry[offset].RVA);
	data->OffsetToData += shift;
}

//clears file from memory (please save first)
void CaveStoryEXE::Clear()
{
	int i;
	if (DOSptr != NULL)
		delete[] DOSptr;
	if (symbolTable != NULL)
		delete[] symbolTable;

	for (i = 0; i < header.NumSections; i++)
		delete[] section[i];

	prevSize = -1;

	//Header Info
	DOSptr = NULL;

	//PE Header Info
	symbolTable = NULL;     //pointer to symbol tables

	header.NumSections = 0;
	header.NumSymbols = 0;
	mapCount = 0;
	mapInfo = NULL;
}

bool CaveStoryEXE::Save()
{
	if(fileName.IsEmpty())//don't try to save if fileName is empty
		return false;
//*
	CFile f;
//	fileName += ".exe";

	CFileStatus status;
	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(fileName))
	{
		CFile::GetStatus(fileName, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(fileName, status);
	}

	if (!f.Open(fileName, CFile::modeCreate | CFile::modeReadWrite))
		return false;
//*/

	int i;
	unsigned short PEptr;
	char buffer[1000];

	PEptr = *((unsigned short*)(&(DOSptr[PE_OFFSET])));

	f.Write(DOSptr, PEptr);//write the DOS header
	buffer[0] = 'P';
	buffer[1] = 'E';
	buffer[2] = '\0';
	buffer[3] = '\0';
	f.Write(buffer, 4);//write the PE signature

	f.Write(&header, sizeof(header));//write header

	//time to correct some data in the optional header changed due to edits to the file structure
	i = sectHeader[rsrcSection].VirtualAddress+sectHeader[rsrcSection].VirtualSize;
	optHeader.SizeOfImage = ((i - 1 + optHeader.FileAlignment) / optHeader.FileAlignment) * optHeader.FileAlignment;
	optHeader.dataDir.ResourceTable.VirtualAddress = sectHeader[rsrcSection].VirtualAddress;

	f.Write(&optHeader, sizeof(optHeader));//write optional header

	for(i = 0; i < header.NumSections; i++)//write all section headers
		f.Write(&(sectHeader[i]), sizeof(COFF_SectionHeader));

	//set the data section references to map to new map location 
	i = sectHeader[mapSection].VirtualAddress+optHeader.ImageBase;
	*((int*) &(((char*) (section[0]))[CS_MAP_JMP1_OFFSET1-sectHeader[0].PointerToRawData])) = i;
	*((int*) &(((char*) (section[0]))[CS_MAP_JMP1_OFFSET2-sectHeader[0].PointerToRawData])) = i;

	*((int*) &(((char*) (section[0]))[CS_MAP_JMP2_OFFSET1-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), fileName);
	*((int*) &(((char*) (section[0]))[CS_MAP_JMP2_OFFSET2-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), fileName);
	*((int*) &(((char*) (section[0]))[CS_MAP_JMP2_OFFSET3-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), fileName);

	*((int*) &(((char*) (section[0]))[CS_MAP_JMP3_OFFSET1-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), background);

	*((int*) &(((char*) (section[0]))[CS_MAP_JMP8_OFFSET1-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), backType);

	*((int*) &(((char*) (section[0]))[CS_MAP_JMP4_OFFSET1-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), NPCset1);

	*((int*) &(((char*) (section[0]))[CS_MAP_JMP5_OFFSET1-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), NPCset2);

	*((int*) &(((char*) (section[0]))[CS_MAP_JMP6_OFFSET1-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), caption);

	*((int*) &(((char*) (section[0]))[CS_MAP_JMP7_OFFSET1-sectHeader[0].PointerToRawData])) = i+offset((*mapInfo), bossNum);

	*((int*) &(((char*) (section[0]))[CS_MAXIMUM_MAP_COUNT_OFFSET])) = mapCount;


	//set map count number equal to total number of maps


	for(i = 0; i < header.NumSections; i++)
	{
		f.Seek(sectHeader[i].PointerToRawData, 0);
		f.Write(section[i], sectHeader[i].SizeOfRawData);
	}

	f.Close();//done writing data

	return true;
}

//save file with .bak added to the end
bool CaveStoryEXE::Backup()
{
	::CopyFile(fileName, fileName+".bak", FALSE);
	return true;
}

//restores the .exe file from the .bak file
bool CaveStoryEXE::RestoreBackup()
{
	::CopyFile(fileName+".bak", fileName, FALSE);
	return true;
}

bool CaveStoryEXE::AddMap(const char* name, int width, int height, int index)
{
	CaveMapINFO info;

	strcpy(info.fileName, name);
	strcpy(info.background, "bk0");
	strcpy(info.NPCset1, "0");
	strcpy(info.NPCset2, "0");
	strcpy(info.caption, name);
	info.backType = 0;
	info.bossNum = 0;

	return AddMap(&info, width, height, index);
}

bool CaveStoryEXE::AddMap(CaveMapINFO* info, int width, int height, int index)
{
	if (index < 0 || index > mapCount)
		index = mapCount;

	int i;
	if (width < 2 || height < 2 || info->fileName[0] == '\0')
		return false;

	for (i = 0; i < mapCount; i++)
		if (strcmp(mapInfo[i].fileName, info->fileName) == 0)
			return false;//no duplicate names

	if (index == mapCount)//we're adding a new map where there isn't place for one so resize the map space
	{
		mapInfo = new CaveMapINFO[mapCount+1];
		memcpy(mapInfo, section[mapSection], sizeof(CaveMapINFO)*mapCount);

		delete[] section[mapSection];
		section[mapSection] = mapInfo;

		mapCount++;
		sectHeader[mapSection].SizeOfRawData = mapCount*sizeof(CaveMapINFO);
		sectHeader[mapSection].VirtualSize = mapCount*sizeof(CaveMapINFO);
	}

	//set map properties for new map
	mapInfo[index] = *info;

	CaveMap map;
	map.width = 0;
	map.height = 0;
	map.oldFileName = info->fileName;
	map.mapInfo = mapInfo[index];
	map.entityNum = 0;
	map.resize(width, height);
	map.save();

	char temp[1024];

	CaveMap testing;

	testing.create(info, width, height);

	//add tsc file
	sprintf(temp, ".\\data\\Stage\\%s.tsc", info->fileName);
	CFile file;
	file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	file.Write("\n\r\r\n", 4);
	file.Close();

	return true;
}

bool CaveStoryEXE::CopyMap(int index, const char* name)
{
	if (index < 0 || index >= mapCount)
		return false;
	if (strlen(name) >= 32 || name[0] == '\0')//name must fit in structure
		return false;

	int i;
	for (i = 0; i < mapCount; i++)
		if (strcmp(mapInfo[i].fileName, name) == 0)
			return false;//no duplicate names

	mapInfo = new CaveMapINFO[mapCount+1];
	memcpy(mapInfo, section[mapSection], sizeof(CaveMapINFO)*mapCount);

	delete[] section[mapSection];
	section[mapSection] = mapInfo;

	mapInfo[mapCount] = mapInfo[index];
	strcpy(mapInfo[mapCount].fileName, name);

	char temp[1024];
	char temp2[1024];

	sprintf(temp, ".\\data\\Stage\\%s.pxm", mapInfo[index].fileName);
	sprintf(temp2, ".\\data\\Stage\\%s.pxm", name);
	::CopyFile(temp, temp2, FALSE);

	sprintf(temp, ".\\data\\Stage\\%s.pxe", mapInfo[index].fileName);
	sprintf(temp2, ".\\data\\Stage\\%s.pxe", name);
	::CopyFile(temp, temp2, FALSE);

	sprintf(temp, ".\\data\\Stage\\%s.tsc", mapInfo[index].fileName);
	sprintf(temp2, ".\\data\\Stage\\%s.tsc", name);
	::CopyFile(temp, temp2, FALSE);

	mapCount++;
	sectHeader[mapSection].SizeOfRawData = mapCount*sizeof(CaveMapINFO);
	sectHeader[mapSection].VirtualSize = mapCount*sizeof(CaveMapINFO);

	return true;
}

bool CaveStoryEXE::DeleteMap(int index)
{
	if (index < 0 || index >= mapCount)
		return false;

	char temp[1024];
	sprintf(temp, ".\\data\\Stage\\%s.pxm", mapInfo[index].fileName);
	::DeleteFile(temp);

	sprintf(temp, ".\\data\\Stage\\%s.pxe", mapInfo[index].fileName);
	::DeleteFile(temp);

	sprintf(temp, ".\\data\\Stage\\%s.tsc", mapInfo[index].fileName);
	::DeleteFile(temp);

	if (index < mapCount-1)
	{
		strcpy(mapInfo[index].fileName, "\\empty");
		return true;
	}

	mapCount--;

	mapInfo = new CaveMapINFO[mapCount];

	if (index > 0)
		memcpy(mapInfo, section[mapSection], sizeof(CaveMapINFO)*index);
	if (index < mapCount)
		memcpy(&(mapInfo[index]), &(((CaveMapINFO*)section[mapSection])[index+1]), sizeof(CaveMapINFO)*(mapCount-index));

	delete[] section[mapSection];
	section[mapSection] = mapInfo;

	sectHeader[mapSection].SizeOfRawData = mapCount*sizeof(CaveMapINFO);
	sectHeader[mapSection].VirtualSize = mapCount*sizeof(CaveMapINFO);
	return true;
}

void CaveStoryEXE::DecodeSegment(BYTE* code, int size, char* buffer, unsigned char stopCode)
{
	int chrPtr = 0;//pointer to buffer
	int oldPtr;
	int ptr = 0;//pointer to data
	int i, j, k;
	int index;
	bool OF;
	bool stop = false;

	while (ptr < size)//check to see if we've decoded up to size requested
	{
		for (i = 0; i < 4; i++)
		{
			switch (code[ptr])//I don't think any of these are used in the weapon code but just in case
			{
				//LOCK and Repeat Prefixes
			case 0xF0://LOCK prefix
			case 0xF2://REPNE prefix
			case 0xF3://REP and REPE prefix

				//Segment Override Prefixes
			case 0x2E://CS override prefix
			case 0x36://SS override prefix
			case 0x3E://DS override prefix
			case 0x26://ES override prefix
			case 0x64://FS override prefix
			case 0x65://GS override prefix

			case 0x66://Opperand size override prefix
			case 0x67://Address size override prefix
				ptr++;//skip byte for later use in prefix code (not yet implemented)
				break;
			};
		}
		oldPtr = ptr;
		OF = false;

		if (code[ptr] == 0x0F)//check for 0x0F extension first
		{
			ptr++;
			OF = true;
		}


		index = -1;
		for (i = 0; i < OPCODE_COUNT; i++)//find opcode in list of known codes and keep track of the index
		{
			if (disAsm.opList[i].code == code[ptr] && OF == disAsm.opList[i]._0F)
			{
				if (disAsm.opList[i].modRM == 0
				  || (disAsm.opList[i].modRM && (disAsm.opList[i].regCode == RM_UNDEF
				  || disAsm.opList[i].regCode == ((ModRM*)(&code[ptr+1]))->reg)
					)
					)
//				if ((opList[i].modRM >= 0 && opList[i].modRM == ((ModRM*)(&code[ptr+1]))->reg) || opList[i].modRM < 0)
				{
					index = i;//keep track of index
					i = OPCODE_COUNT;//no need to look any further so...
				}
			}
		}
		if (index == -1)//unknown opcode
		{
			sprintf(&buffer[chrPtr], "0x%X", code[ptr]);
			chrPtr += strlen(&buffer[chrPtr]);
			ptr++;
		}
		else //known opcode to display (this SHOULD be the only one reached)
		{
			if (stopCode == disAsm.opList[index].code)//not currently used
				stop = true;

			sprintf(&buffer[chrPtr], "%s", disAsm.opList[index].name);
			chrPtr += strlen(&buffer[chrPtr]);
			ptr++;

			ptr += DecodeArguments(&code[ptr], &buffer[chrPtr], index);
			chrPtr += strlen(&buffer[chrPtr]);

		}
		buffer[chrPtr++] = '\r';
		buffer[chrPtr++] = '\n';

		if (stop)
		{
			buffer[chrPtr-2] = '\0';
			return;
		}
	}
	buffer[chrPtr-2] = '\0';
}


void CaveStoryEXE::IdentifyEntityRects()
{
	int oldPtr;
	int ptr = 0;//pointer to data
	int i, j, k, ent;
	int index;
	bool OF;
	bool stop = false;
	bool next;

	int offset;
	int nextRect;

	char buffer[256];

	int size = DECODE_MAX;
	BYTE* code;

	for (ent = 0; ent < NPC_TYPE_COUNT; ent++)
	{
		//use the jump table and shift the offset to get the data section offset
		code = (BYTE*)exe.section[0]+exe.eJT[ent]-0x401000;
		ptr = 0;

		next = false;
		nextRect = 0;
		eRectCount[ent] = 0;

		while (ptr < size && !next)//check to see if we've decoded up to size requested
		{
			for (i = 0; i < 4; i++)
			{
				switch (code[ptr])//I don't think any of these are used in the weapon code but just in case
				{
					//LOCK and Repeat Prefixes
				case 0xF0://LOCK prefix
				case 0xF2://REPNE prefix
				case 0xF3://REP and REPE prefix

					//Segment Override Prefixes
				case 0x2E://CS override prefix
				case 0x36://SS override prefix
				case 0x3E://DS override prefix
				case 0x26://ES override prefix
				case 0x64://FS override prefix
				case 0x65://GS override prefix

				case 0x66://Opperand size override prefix
				case 0x67://Address size override prefix
					ptr++;//skip byte for later use in prefix code (not yet implemented)
					break;
				};
			}
			oldPtr = ptr;
			OF = false;

			if (code[ptr] == 0x0F)//check for 0x0F extension first
			{
				ptr++;
				OF = true;
			}


			index = -1;
			for (i = 0; i < OPCODE_COUNT; i++)//find opcode in list of known codes and keep track of the index
			{
				if (disAsm.opList[i].code == code[ptr] && OF == disAsm.opList[i]._0F)
				{
					if (disAsm.opList[i].modRM == 0
					|| (disAsm.opList[i].modRM && (disAsm.opList[i].regCode == RM_UNDEF
														|| disAsm.opList[i].regCode == ((ModRM*)(&code[ptr+1]))->reg)
						)
						)
					{
						index = i;//keep track of index
						i = OPCODE_COUNT;//no need to look any further so...
					}
				}
			}
			if (index == -1)//unknown opcode
			{
				next = true;
			}
			else //known opcode to display (this SHOULD be the only one reached)
			{
				if (disAsm.opList[index].code == 0x33
				 || disAsm.opList[index].code == 0xC3)
					next = true;

				ptr++;

				if (disAsm.opList[index].code == 0xC7 && 
					(code[ptr] == 0x45 || code[ptr] == 0x85))//this is the MOV code to place the rects on the stack
				{
					//debug stuff
					if (ent == 42)
						ent = ent;

					//offset it no longer used because it isn't reliable (entity 17)
					if (code[ptr++] == 0x45)
						offset = -((char)(code[ptr++])/4);
					else
					{
						offset = -((int)(code[ptr])/4);
						ptr += 4;
					}

					//This counts the number of MOVs encountered for this entity
					//There should be 4 per rectangle and SHOULD appear in the same order every time
					switch (eRectCount[ent] % 4)
					{
					case 0:
						offset -= 4;
						eRectDisplay[ent][eRectCount[ent]/4].left = *((int*)(&code[ptr]));
						eRectCount[ent]++;
						break;
					case 1:
						eRectDisplay[ent][eRectCount[ent]/4].top = *((int*)(&code[ptr]));
						eRectCount[ent]++;
						break;
					case 2:
						eRectDisplay[ent][eRectCount[ent]/4].right = *((int*)(&code[ptr]));
						eRectCount[ent]++;
						break;
					case 3:
						eRectDisplay[ent][eRectCount[ent]/4].bottom = *((int*)(&code[ptr]));
						if (eRectDisplay[ent][eRectCount[ent]/4].bottom - eRectDisplay[ent][eRectCount[ent]/4].top == 0
						 || eRectDisplay[ent][eRectCount[ent]/4].right - eRectDisplay[ent][eRectCount[ent]/4].left == 0)
							eRectCount[ent] -= 4;
						eRectCount[ent]++;
						break;
					}
					ptr += 4;

				}
				else
					ptr += DecodeArguments(&code[ptr], buffer, index);

			}
		}
		eRectCount[ent] /= 4;

	}
	return;
}

//unlike the above function this one simply breaks down the raw data into separate line for each opcode
//This is useful for debugging and possibly advanced editing
void CaveStoryEXE::RawCodeSegment(BYTE* code, int size, char* buffer, unsigned char stopCode)
{
	int chrPtr = 0;//pointer to buffer
	int oldPtr;
	int ptr = 0;//pointer to data
	int i, j, k;
	int index;
	bool OF;
	bool stop = false;

	while (ptr < size)//check to see if we've decoded up to size requested
	{
		for (i = 0; i < 4; i++)
		{
			switch (code[ptr])//I don't think any of these are used in the weapon code but just in case
			{
				//LOCK and Repeat Prefixes
			case 0xF0://LOCK prefix
			case 0xF2://REPNE prefix
			case 0xF3://REP and REPE prefix

				//Segment Override Prefixes
			case 0x2E://CS override prefix
			case 0x36://SS override prefix
			case 0x3E://DS override prefix
			case 0x26://ES override prefix
			case 0x64://FS override prefix
			case 0x65://GS override prefix

			case 0x66://Opperand size override prefix
			case 0x67://Address size override prefix
				ptr++;//skip byte for later use in prefix code (not yet implemented)
				break;
			};
		}
		oldPtr = ptr;
		OF = false;

		if (code[ptr] == 0x0F)//check for 0x0F extension first
		{
			ptr++;
			OF = true;
		}

		
		index = -1;
		for (i = 0; i < OPCODE_COUNT; i++)//find opcode in list of known codes and keep track of the index
		{
			if (disAsm.opList[i].code == code[ptr] && OF == disAsm.opList[i]._0F)
			{
				if (disAsm.opList[i].modRM == 0
				|| (disAsm.opList[i].modRM && (disAsm.opList[i].regCode == RM_UNDEF
				|| disAsm.opList[i].regCode == ((ModRM*)(&code[ptr+1]))->reg)
					)
					)
//				if ((opList[i].modRM >= 0 && opList[i].modRM == ((ModRM*)(&code[ptr+1]))->reg) || opList[i].modRM < 0)
				{
					index = i;//keep track of index
					i = OPCODE_COUNT;//no need to look any further so...
				}
			}
		}
		if (index == -1)//unknown opcode
		{
			ptr++;
		}
		else //known opcode to display (this SHOULD be the only one reached)
		{
			if (stopCode == disAsm.opList[index].code)//not currently used
				stop = true;

			ptr++;
			ptr += DecodeArguments(&code[ptr], &buffer[chrPtr], index);

		}

		sprintf(&buffer[chrPtr], "0x");
		chrPtr += 2;
		while (oldPtr < ptr)
		{
			sprintf(&buffer[chrPtr], "%02X", code[oldPtr++]);
			chrPtr += 2;
		}

		buffer[chrPtr++] = '\r';
		buffer[chrPtr++] = '\n';

		if (stop)
		{
			buffer[chrPtr-2] = '\0';
			return;
		}
	}
	buffer[chrPtr-2] = '\0';
}

int CaveStoryEXE::DecodeArguments(BYTE* data, char* buffer, int index)
{
	char arg[3][32];
	arg[0][0] = 0;
	arg[1][0] = 0;
	arg[2][0] = 0;

	int i;
	int temp;
	char sign;
	int ptr = 0;


	if (disAsm.opList[index].modRM)
	{
		ptr++;

		//check for SIB byte
		if (((ModRM*)(&data[0]))->mod == 0 && ((ModRM*)(&data[0]))->RM == 4)
		{
			ptr++;
			//this switch statement is a little confusing because its been reduced to save space
			//basically it determines the scaling factor on the index (x1, x2, x4, x8)
			//and which register is being used for argument 1 (arguments may be reversed based on specific opcode logic)
			//this code would have been quite a bit longer if nested switches were used
			switch (((SIB*)(&data[1]))->base)
			{
				case 0: sprintf(arg[0], "PTR [EAX%s]",((SIB*)(&data[1]))->scale==0?"":((SIB*)(&data[1]))->scale==1?"*2":((SIB*)(&data[1]))->scale==2?"*4":"*8"); break;
				case 1: sprintf(arg[0], "PTR [ECX%s]",((SIB*)(&data[1]))->scale==0?"":((SIB*)(&data[1]))->scale==1?"*2":((SIB*)(&data[1]))->scale==2?"*4":"*8"); break;
				case 2: sprintf(arg[0], "PTR [EDX%s]",((SIB*)(&data[1]))->scale==0?"":((SIB*)(&data[1]))->scale==1?"*2":((SIB*)(&data[1]))->scale==2?"*4":"*8"); break;
				case 3: sprintf(arg[0], "PTR [EBX%s]",((SIB*)(&data[1]))->scale==0?"":((SIB*)(&data[1]))->scale==1?"*2":((SIB*)(&data[1]))->scale==2?"*4":"*8"); break;
				case 4: sprintf(arg[0], "PTR [ESP%s]",((SIB*)(&data[1]))->scale==0?"":((SIB*)(&data[1]))->scale==1?"*2":((SIB*)(&data[1]))->scale==2?"*4":"*8"); break;
				case 5: sprintf(arg[0], "PTR [EBP%s]",((SIB*)(&data[1]))->scale==0?"":((SIB*)(&data[1]))->scale==1?"*2":((SIB*)(&data[1]))->scale==2?"*4":"*8"); break;
				case 6: sprintf(arg[0], "PTR [ESI%s]",((SIB*)(&data[1]))->scale==0?"":((SIB*)(&data[1]))->scale==1?"*2":((SIB*)(&data[1]))->scale==2?"*4":"*8"); break;
				case 7: sprintf(arg[0], "PTR [EDI%s]",((SIB*)(&data[1]))->scale==0?"":((SIB*)(&data[1]))->scale==1?"*2":((SIB*)(&data[1]))->scale==2?"*4":"*8"); break;
			};//end switch

			//gets argument 2's register
			switch (((SIB*)(&data[1]))->index)
			{
				case 0: sprintf(arg[1], "EAX"); break;
				case 1: sprintf(arg[1], "ECX"); break;
				case 2: sprintf(arg[1], "EDX"); break;
				case 3: sprintf(arg[1], "EBX"); break;
//				case 4: sprintf(arg[1], "ESP"); break;//this one is not used but included for completeness
				case 5: sprintf(arg[1], "EBP"); break;
				case 6: sprintf(arg[1], "ESI"); break;
				case 7: sprintf(arg[1], "EDI"); break;
			};//end switch
		}
		else//if no SIB byte
		if (((ModRM*)(&data[0]))->mod == 1 || ((ModRM*)(&data[0]))->mod == 2)
		{
			if (((ModRM*)(&data[0]))->mod == 1)
				temp = (char)data[ptr];
			else
				temp = *(int*)(&data[ptr]);

			if (temp < 0)
				sign = '-';
			else
				sign = '+';

			temp = abs(temp);
			if (((ModRM*)(&data[0]))->mod == 1)
				ptr++;
			else
				ptr += 4;

			sprintf(arg[1], "PTR [%s %c 0x%02X]", REG_NAME[disAsm.opList[index].rmSize][((ModRM*)(&data[0]))->RM], sign, temp);

			sprintf(arg[0], REG_NAME[disAsm.opList[index].regSize][((ModRM*)(&data[0]))->reg]);
		}
		else
		if (((ModRM*)(&data[0]))->mod != 3)
		{
			switch (((ModRM*)(&data[0]))->RM)
			{
				case 4:
					if (((ModRM*)(&data[0]))->mod > 0)
						sprintf(arg[1], "PTR [ESP]");
					else
						sprintf(arg[1], "PTR [--]");
					break;
				case 5:
					if (((ModRM*)(&data[0]))->mod > 0)
						sprintf(arg[1], "PTR [EBP]");
					else
					{
						sprintf(arg[1], "PTR [0x%X]", *((int*)(&data[ptr])));
						ptr += 4;
					}
					break;
				default:
					sprintf(arg[1], "PTR [%s]", REG_NAME[disAsm.opList[index].rmSize][((ModRM*)(&data[0]))->RM]);
					break;
			};

			sprintf(arg[0], REG_NAME[disAsm.opList[index].regSize][((ModRM*)(&data[0]))->reg]);
		}
		else
		{
			switch (((ModRM*)(&data[0]))->RM)
			{
				case 4:
					if (((ModRM*)(&data[0]))->mod > 0)
						sprintf(arg[1], "ESP");
					else
						sprintf(arg[1], "[--]");
					break;
				case 5:
					if (((ModRM*)(&data[0]))->mod > 0)
						sprintf(arg[1], "EBP");
					else
						sprintf(arg[1], "[??]");
					break;
				default:
					sprintf(arg[1], REG_NAME[disAsm.opList[index].rmSize][((ModRM*)(&data[0]))->RM]);
					break;
			};
			sprintf(arg[0], REG_NAME[disAsm.opList[index].regSize][((ModRM*)(&data[0]))->reg]);
		}

	}

	//now let's check for fixed constants and registers
	switch (disAsm.opList[index].reg)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			sprintf(arg[disAsm.opList[index].order], REG_NAME[disAsm.opList[index].regSize][disAsm.opList[index].reg]);
			break;
	};

	//this is to account for JMP codes affecting the IP and setting the jump address correctly
	if (disAsm.opList[index].name[0] == 'J')
		i = 1+ptr+disAsm.opList[index].imm;
	else
		i = 0;

	// 1-disAsm.opList[index].order
	//the above is used to adjust which register info get over written
	//since you the immediate value and one of the registers typically
	switch (disAsm.opList[index].imm)
	{
		case 1:
			sprintf(arg[1-disAsm.opList[index].order], "0x%02X", i+(int)data[ptr]);
			break;
		case 2:
			sprintf(arg[1-disAsm.opList[index].order], "0x%02X", i+*((short*)((&data[ptr]))));
			break;
		case 4:
			sprintf(arg[1-disAsm.opList[index].order], "0x%02X", i+*((int*)((&data[ptr]))));
			break;
	};
	ptr += disAsm.opList[index].imm;

	if (disAsm.opList[index].regSize == 0 
	 && disAsm.opList[index].rmSize == 0 
	 && disAsm.opList[index].imm == 0)//no arguments?
		return ptr;

	if (arg[0][0] == 0)
		sprintf(buffer, "\t%s", arg[1]);
	else
	if (arg[1][0] == 0)
		sprintf(buffer, "\t%s", arg[0]);
	else
	if (disAsm.opList[index].order == REG_FIRST)
		sprintf(buffer, "\t%s, %s", arg[0], arg[1]);
	else
		sprintf(buffer, "\t%s, %s", arg[1], arg[0]);

	return ptr;
}


