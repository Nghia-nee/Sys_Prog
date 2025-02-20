
// ServerDlg.h : header file
//

#pragma once



// CServerDlg dialog
class CServerDlg : public CDialogEx
{
// Construction
public:
	CServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	CButton m_btnStart;
	CEdit m_editIP;
	CEdit m_editPort;
	SOCKET m_serverSocket;
	HANDLE m_hListenThread;
	void AddLogEntry(const CString& strTime, const CString& strEvent, CListCtrl* list_Log);
	void AddConnection(const CString& strIP, const CString& strPort);
	void RemoveConnection(const CString& strIP, const CString& strPort);
	static DWORD WINAPI ListenThreadProc(LPVOID lpParam);
	static DWORD WINAPI ClientThreadProc(LPVOID lpParam);

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
	afx_msg void OnLvnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedStart();

};
