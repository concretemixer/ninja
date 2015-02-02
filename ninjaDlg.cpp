
// ninjaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ninja.h"
#include "ninjaDlg.h"
#include "FrameDlg.h"
#include "afxdialogex.h"

#include <map>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool InitScraper();

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CninjaDlg dialog




CninjaDlg::CninjaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CninjaDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CninjaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CninjaDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CninjaDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CninjaDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CninjaDlg message handlers

BOOL CninjaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitScraper();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CninjaDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CninjaDlg::OnPaint()
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
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CninjaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


#define _WIN32_WINNT 0x0400
#pragma comment( lib, "user32.lib" )

#include <windows.h>
#include <stdio.h>

int HandHash(HWND h);

CRITICAL_SECTION tablesCriticalSection;

struct TableData {
	HWND handle;
	int slot;
	bool playing;
	bool folded;
	unsigned int btnTime;
	int handNumHash;
	CFrameDlg* frame;
};

std::map<HWND,TableData> tables;

void PressMouse(HWND h, POINT p)
{
	HWND hc = ChildWindowFromPoint(h,p);	
	if (hc!=h) {		
		RECT r;
		GetClientRect(hc,&r);

		int x = (r.left+r.right) / 2;
		int y = (r.top+r.bottom) / 2;
		PostMessage(hc,WM_LBUTTONDOWN,MK_LBUTTON, x	| (y << 16));	
		PostMessage(hc,WM_LBUTTONUP,MK_LBUTTON, x	| (y << 16));
	}
}


void SimulatePrefold(HWND h, int x, int y)
{
	wchar_t buf[256];
	SendMessageW(h,WM_GETTEXT,256,(LPARAM)buf);
	if (wcsstr(buf,L"Blinds")) {
		POINT p;
		RECT r;
		GetClientRect(h,&r);		 


		p.x = (r.right * 401) / 1052;
		p.y = (r.bottom * 643) / 768;		
		PressMouse(h,p);
		

		p.x = (r.right * 552) / 1052;
		p.y = (r.bottom * 725) / 768;		
		PressMouse(h,p);

		EnterCriticalSection(&tablesCriticalSection);
		tables[h].folded = true;
		LeaveCriticalSection(&tablesCriticalSection);

		Log("preFolded %x",h);
	}	
}

void CheckForFold(HWND h, int x, int y)
{
	wchar_t buf[256];
	SendMessageW(h,WM_GETTEXT,256,(LPARAM)buf);
	if (wcsstr(buf,L"Blinds")) {

		POINT p;
		RECT r;
		GetClientRect(h,&r);

		p.x = x;
		p.y = y;
		::ScreenToClient(h,&p);

		HWND hc = ChildWindowFromPointEx(h,p,CWP_SKIPINVISIBLE);

		if (hc == h)
			return;

		p.x = (r.right * 401) / 1052;
		p.y = (r.bottom * 643) / 768;		
		HWND hpre = ChildWindowFromPointEx(h,p,CWP_SKIPINVISIBLE);


		p.x = (r.right * 552) / 1052;
		p.y = (r.bottom * 725) / 768;		
		HWND hfold = ChildWindowFromPointEx(h,p,CWP_SKIPINVISIBLE);

		

		if ((hc==hpre) || (hc==hfold)) {
			Log("%x | %x %x",hc,hpre,hfold);
			EnterCriticalSection(&tablesCriticalSection);
			tables[h].folded = true;
			LeaveCriticalSection(&tablesCriticalSection);
			Log("Folded %x",h);
		}
	}	
}


HHOOK hMouseHook;

__declspec(dllexport) LRESULT CALLBACK KeyboardEvent (int nCode, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;

	if (pMouseStruct != NULL) {
		if (wParam==WM_RBUTTONUP) {
			HWND h = GetForegroundWindow();
			SimulatePrefold(h, pMouseStruct->pt.x, pMouseStruct->pt.y);
		}		
		if (wParam==WM_LBUTTONUP) {
			HWND h = GetForegroundWindow();
			CheckForFold(h, pMouseStruct->pt.x, pMouseStruct->pt.y);
		}
	}
	return CallNextHookEx(hMouseHook,nCode,wParam,lParam);
}


CFrameDlg frames[24];
int nextSlot = 0;

void MessageLoop()
{
	MSG message;
	while (GetMessage(&message,NULL,0,0)) {
		TranslateMessage( &message );
		DispatchMessage( &message );
	}
}

HANDLE hThread;

DWORD WINAPI MyMouseLogger(LPVOID lpParm)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	
	if (!hInstance) return 1;

	hMouseHook = SetWindowsHookEx (  
		WH_MOUSE_LL,
		(HOOKPROC) KeyboardEvent,  
		hInstance,                 
		NULL                       
		);
	
	MessageLoop();

	UnhookWindowsHookEx(hMouseHook);
	return 0;
}


void UpdateTableData(HWND h)
{	
	if (tables.find(h)==tables.end()) {
		TableData t;
		t.handle = h;
		t.playing = false;
		t.folded = false;
		t.handNumHash = 0;
		t.btnTime = 0;
		t.slot = nextSlot;
		nextSlot++;

		t.frame = &(frames[t.slot]);
		tables[h] = t;

		Log("%x - found",h);
	}
	/*
	if (!IsWindow(h)) {
		Log("%x - closed",h);
		tables.erase(h);
		return;
	}
	*/



	RECT mainRect;
	::GetWindowRect(h, &mainRect);

	bool play = tables[h].playing;	
	bool abtn = false;
	bool tbtn = false;

	if (play && tables[h].frame && tables[h].frame->m_hWnd!=0) {
		tables[h].frame->SetTrackedWindow(h);
		//tables[h].frame->ShowWindow(SW_SHOW);
	}

	int hash = HandHash(h);
	if (hash!=tables[h].handNumHash) {
		Log("New hand %x",h);
		tables[h].handNumHash = hash;
		tables[h].folded = false;
	}

	HWND handle = 0;

	while (handle = ::FindWindowEx(h,handle,0,0)) 
	{
		RECT r;
		::GetClientRect(h, &r);

		int y = (r.bottom * 611) / 768;	
		int x1 = (r.right * 379) / 1052;
		int x2 = (r.right * 908) / 1052;

		if (::IsWindowVisible(handle)) {
			
			::GetWindowRect(handle,&r);
			if (r.top- mainRect.top> y) {				
				if (!play) {
					if (r.left - mainRect.left > x2) {
						//Log("%x - playing",h);
						tables[h].playing = true;


					}
				}
				else if ((r.left - mainRect.left > x1) && (r.left - mainRect.left < x2)) {
					if (tables[h].playing) {						
						tables[h].btnTime = timeGetTime();

						//tables[h].folded = false;
						//Log("%x - buttons, %d",h,tables[h].btnTime);
					}
				}
			}
		}
	}

	{
		RECT r;
		::GetClientRect(h, &r);

		POINT p;
		p.x = (r.right * 552) / 1052;
		p.y = (r.bottom * 725) / 768;		
		abtn =  IsWindowVisible(ChildWindowFromPointEx(h,p,0));

		p;
		p.x = (r.right * 858) / 1052;
		p.y = (r.bottom * 666) / 768;		
		tbtn =  IsWindowVisible(ChildWindowFromPointEx(h,p,0));

	}
	
	if (tables[h].frame && tables[h].frame->m_hWnd!=0) {		
		tables[h].frame->SetFrame(tbtn ? 4 : 2, tbtn ? 0xff : 0xFF00);
		tables[h].frame->ShowWindow((abtn || tbtn) ? SW_SHOW : SW_HIDE);
	}

}

void UpdateTableWindow(const TableData& data)
{
	
	int alpha = 255;
	if (data.folded && data.playing && data.btnTime>0) {
		DWORD t = timeGetTime() - data.btnTime;
		if (t > 7000)  {
			if (t > 9000)		
				alpha = 160;
			else {
				alpha = 255 - (95*(t - 7000))/2000;
				if (alpha < 160)
					alpha = 160;
			}
		}
	}
	//Log("a = %d",alpha);
	::SetWindowLong(data.handle, GWL_EXSTYLE, ::GetWindowLong(data.handle, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(data.handle, 1, alpha, LWA_ALPHA);	
	
}

HANDLE hThread2;

DWORD WINAPI MyWindowWatcher(LPVOID lpParm)
{
	while (true) {

		HWND h = 0;
		while (h = ::FindWindowEx(0,h,L"wxWindowNR",0)) 
		{
			wchar_t buf[256];
			SendMessageW(h,WM_GETTEXT,256,(LPARAM)buf);
			if (wcsstr(buf,L"Blinds")) {
				EnterCriticalSection(&tablesCriticalSection);
				UpdateTableData(h);
				LeaveCriticalSection(&tablesCriticalSection);
			}
		}

		EnterCriticalSection(&tablesCriticalSection);
		std::map<HWND,TableData>::iterator i = tables.begin();
		while(i!=tables.end()) {
			UpdateTableWindow(i->second);
			i++;
		}
		LeaveCriticalSection(&tablesCriticalSection);

		Sleep(300);
	}

	return 0;
}


void CninjaDlg::OnBnClickedButton1()
{
	DWORD dwThread;
	
	for(int a=0;a<20;a++)
	{
		frames[a].Create(IDD_FRAMEDIALOG, CWnd::GetDesktopWindow());
	}

	InitializeCriticalSection(&tablesCriticalSection);

	hThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)
		MyMouseLogger, 0, NULL, &dwThread);

	hThread2 = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)
		MyWindowWatcher, 0, NULL, &dwThread);
	
	GetDlgItem(IDC_BUTTON1)->EnableWindow(0);
	// TODO: Add your control notification handler code here
}



void CninjaDlg::OnBnClickedButton2()
{
	

}