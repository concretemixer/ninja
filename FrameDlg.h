#pragma once


// CFrameDlg dialog

class CFrameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFrameDlg)

public:
	CFrameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFrameDlg();

// Dialog Data
	enum { IDD = IDD_FRAMEDIALOG };

	void SetTrackedWindow(HWND h);
	void SetFrame(int w, COLORREF color)
	{
		m_width = w;
		m_color = color;
		InvalidateRect(0,1);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	HWND m_tracked;
	int m_width;
	COLORREF m_color;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};
