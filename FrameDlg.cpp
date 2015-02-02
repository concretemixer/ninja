// FrameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ninja.h"
#include "FrameDlg.h"
#include "afxdialogex.h"


// CFrameDlg dialog

IMPLEMENT_DYNAMIC(CFrameDlg, CDialogEx)

CFrameDlg::CFrameDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFrameDlg::IDD, pParent)
{
	m_width = 2;
	m_color = 0x00FF00;	
}

CFrameDlg::~CFrameDlg()
{
}

void CFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFrameDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CFrameDlg message handlers

void CFrameDlg::SetTrackedWindow(HWND h) 
{ 
	m_tracked = h; 

	RECT r;
	POINT p;
	p.x = 0;
	p.y = 0;

	::ClientToScreen(h,&p);


	::GetClientRect(h, &r);
	::SetWindowPos(this->m_hWnd,h,p.x,p.y,r.right,r.bottom,0);
}

void CFrameDlg::OnPaint()
{
	if (!IsWindow(m_tracked)) {
		CloseWindow();
		return;
	}

	if (!m_init) {
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(m_hWnd, 1, 255, LWA_COLORKEY);	
		m_init = true;
	}

	CPaintDC dc(this); // device context for painting

	CRect r;

	

	GetClientRect(&r);

	dc.FillSolidRect(r, m_color);
	r.DeflateRect(m_width,m_width,m_width,m_width);
	dc.FillSolidRect(r, 1);
	




}


BOOL CFrameDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}
