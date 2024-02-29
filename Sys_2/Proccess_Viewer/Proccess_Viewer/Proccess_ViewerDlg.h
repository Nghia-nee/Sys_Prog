
// Proccess_ViewerDlg.h : header file
//

#pragma once


// CProccessViewerDlg dialog
class CProccessViewerDlg : public CDialogEx
{
// Construction
public:
	CProccessViewerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCCESS_VIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void CProccessViewerDlg::PrintProcessNameAndID(DWORD processID, CListCtrl* mylist);
	void CProccessViewerDlg::GetProcessCommandLine(DWORD processID, LPTSTR szCommandLine, DWORD bufferSize);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedbuttonRefresh();
};
