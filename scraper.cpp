#include "stdafx.h"
#include "resource.h"


void SendString(const char *formatstr ... );

HMODULE yeuxHandle = 0;

typedef  bool (*_SavePNGFileNC)(HWND,char*);
_SavePNGFileNC pSavePNGFileNC = 0;

typedef  bool (*_SavePNGFile)(HWND,char*);
_SavePNGFile pSavePNGFile = 0;

typedef  void (*_PreScrape)(HWND,bool);
_PreScrape pPreScrape= 0;

typedef  void (*_PostScrape)(HWND);
_PostScrape pPostScrape = 0;

typedef BOOL (*_ReadRegion)(HWND hwnd, char* name, char* result);
_ReadRegion pReadRegion = 0;
_ReadRegion pReadSymbol = 0;

typedef int (*_ReadRegionHash)(HWND hwnd, char* name);
_ReadRegionHash pReadRegionHash;

typedef BOOL (*_OpenTablemap)(char* filename);
_OpenTablemap pOpenTablemap = 0;

typedef BOOL (*_OpenTablemapFromMemory)(char* data, int size);
_OpenTablemapFromMemory pOpenTablemapFromMemory = 0;

typedef BOOL (*_GetRegionPos)(HWND hwnd,char* name, int& posleft, int& postop, int& posright, int& posbottom);
_GetRegionPos pGetRegionPos = 0;

typedef BOOL (*_SetRegionPos)(HWND hwnd,char* name, int posleft, int postop, int posright, int posbottom);
_SetRegionPos pSetRegionPos = 0;

static char *last_strstr(char *haystack, char *needle)
{
	if (*needle == '\0')
		return (char *) haystack;

	char *result = NULL;
	for (;;) {
		char *p = strstr(haystack, needle);
		if (p == NULL)
			break;
		result = p;
		haystack = p + 1;
	}

	return result;
}

bool InitScraper()
{
	yeuxHandle = LoadLibraryA("yeux_v5.dll");
	if (!yeuxHandle) {
		Log("Scraper DLL load FAILED");
		return false;
	}
	Log("Scraper DLL loaded");
	
	pPreScrape = (_PreScrape)GetProcAddress(yeuxHandle,"PreScrape");
	pPostScrape = (_PostScrape)GetProcAddress(yeuxHandle,"PostScrape");
	pReadRegion = (_ReadRegion)GetProcAddress(yeuxHandle,"ReadRegion");
	pReadSymbol = (_ReadRegion)GetProcAddress(yeuxHandle,"ReadSymbol");
	pReadRegionHash = (_ReadRegionHash)GetProcAddress(yeuxHandle,"ReadRegionHash");
	pOpenTablemap = (_OpenTablemap)GetProcAddress(yeuxHandle,"OpenTablemap");
	pGetRegionPos = (_GetRegionPos)GetProcAddress(yeuxHandle,"GetRegionPos");
	pSavePNGFile = (_SavePNGFile)GetProcAddress(yeuxHandle,"SavePNGFile");
	pSetRegionPos = (_SetRegionPos)GetProcAddress(yeuxHandle,"SetRegionPos");
	pOpenTablemapFromMemory = (_OpenTablemapFromMemory)GetProcAddress(yeuxHandle,"OpenTablemapFromMemory");

	HMODULE dllModule = GetModuleHandle(0);

	HRSRC rcTm =  FindResourceA(dllModule,MAKEINTRESOURCEA(IDR_TABLEMAP_PW_6MAX),MAKEINTRESOURCEA(10));

	if (!rcTm)
		Log("Failed to open from resource %p",GetLastError());
	else {
		HGLOBAL resourceData = ::LoadResource(dllModule, rcTm);
		void* pData = ::LockResource(resourceData);
		int size = SizeofResource(dllModule, rcTm);
		if (!pOpenTablemapFromMemory((char*)pData, size)) {
			Log("Failed to open tablemap from memory");
			return false;
		}
	}


	return true;
}

int HandHash(HWND h)
{
	if (!yeuxHandle)
		return 0;	

	pPreScrape(h, false);

	RECT r;
	GetClientRect(h,&r);


	int x1 = 51, y1 = 4, x2 = 131, y2 =  17;

	x1 = (r.right * x1) / 1052;
	y1 = (r.bottom * y1) / 768;
	x2 = (r.right * x2) / 1052;
	y2 = (r.bottom * y2) / 768;

	int s = pSetRegionPos(h,"hand",x1,y1,x2,y2);
	int result = pReadRegionHash(h,"hand");
	//Log("H: %x",result);
//	int _x1,_y1,_x2,_y2;
	//pGetRegionPos(h,"hand",_x1,_y1,_x2,_y2);

	pPostScrape(h);

	return result;
}

/*
bool ScrapeState(HWND h, HoldemState& s)
{	
	memset((void*)&s,0,sizeof s);

	char region[128];
	char buf[256];

	s.acting = -1;

	pPreScrape(h, true);

	
	pReadRegion(h,"c0handnumber",buf);
	strcpy_s(s.handId, buf);

	for (int a=0;a<6;a++)
	{		
		sprintf_s(region,"p%ddealer",a);
		pReadRegion(h,region,buf);
		if (!strcmp(buf,"true"))
			s.button = a;

		sprintf_s(region,"p%dacting",a);
		pReadRegion(h,region,buf);
		if (!strcmp(buf,"true"))
			s.acting = a;

		s.seated[a] = true;

		bool empty = false;
		sprintf_s(region,"p%dseat",a);
		if (pReadRegion(h,region,buf)) {
			if (strcmp(buf,"EMPTY")==0)
				empty = true;
			else if (strstr(buf,"SERV")) // RESERVED
				empty = true;
			else if (strstr("BET CALL CHECK FOLD RAISE ALLIN BB SB ANTE MUCK",buf)) {
				pPostScrape(h);	
				return false;
			}
		}

		if (empty) {
			s.seated[a] = false;
			continue;
		}

		sprintf_s(region,"p%dname",a);
		if (pReadRegion(h,region,buf))
			strcpy_s(s.names[a], buf);

		sprintf_s(region,"p%dbet",a);
		if (pReadRegion(h,region,buf))
			strcpy_s(s.bets[a], buf);

		sprintf_s(region,"p%dbalance",a);
		if (pReadRegion(h,region,buf))
			strcpy_s(s.stacks[a], buf);


		strcpy_s(s.cards[a], "none");


		char cards[5] = "????";

		for (int b = 0; b < 2; b++)
		{

			sprintf_s(region,"p%dcardface%drank",a,b);
			
			if (pReadRegion(h, region, buf))
			{
				cards[b*2] = buf[0];
				sprintf_s(region,"p%dcardface%dsuit",a,b);
				if (pReadRegion(h, region, buf))
				{
					cards[b*2+1] = buf[0];
				}
			}
		}

		bool hasOpenHolecards = true;

		for(int b=0;b<4;b++)
			if (cards[b]=='?')
				hasOpenHolecards = false;



		if (hasOpenHolecards) {
			strcpy_s(s.cards[a], cards);
		}
		else
		{

			sprintf_s(region,"p%dcardback",a);
			
			if (pReadRegion(h, region, buf))
			{
				if (strstr(buf,"true"))
				{
					strcpy_s(s.cards[a], "????");
				}
			}
		}		

	}

	for(int a=0;a<5;a++) {
		sprintf_s(region,"c0cardface%drank",a);
		if (pReadRegion(h,region,buf))
			s.board[a*2] = buf[0];
		else
			s.board[a*2] = '_';

		sprintf_s(region,"c0cardface%dsuit",a);
		if (pReadRegion(h,region,buf))
			s.board[a*2+1] = buf[0];
		else
			s.board[a*2+1] = '_';

	}

	for (int a=0;a<6;a++)
	{
		if (!s.seated[a])
			continue;

		sprintf_s(region,"p%dname",a);
		if (pReadRegion(h,region,buf))
			if (strcmp(s.names[a], buf)) {
				pPostScrape(h);	
				return false;
			}

		sprintf_s(region,"p%dbalance",a);
		if (pReadRegion(h,region,buf))
			if (strcmp(s.stacks[a], buf)) {
				pPostScrape(h);	
				return false;
			}
	}

	if (s.acting<0) {
		for (int a=0;a<6;a++)
		{
			sprintf_s(region,"p%dacting",a);
			pReadRegion(h,region,buf);
			if (!strcmp(buf,"true"))
				s.acting = a;	
		}
	}


	pPostScrape(h);	

	SendMessageA(h,WM_GETTEXT,256,(LPARAM)buf);	

	char *s1 = strstr(buf,"/");
	if (s1) {
		char *s2 = s1+1;
		int i=0;
		while (*s2!=' ')  {
			s.bblind[i] = *s2;
			s2++;
			i++;
		}
	}



	return true;
}

*/