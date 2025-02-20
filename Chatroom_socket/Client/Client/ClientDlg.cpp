
// ClientDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:

	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDlg dialog



CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Button_1, m_btnConnect);
	DDX_Control(pDX, Button_2, m_btnSend);
	DDX_Control(pDX, EditBox_ServerAddr, m_editServerAddr);
	DDX_Control(pDX, EditBox_Port, m_editPort);

}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Button_1, &CClientDlg::OnBnClicked1)
	ON_BN_CLICKED(Button_2, &CClientDlg::OnBnClicked2)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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
	m_btnConnect.SetWindowTextW(_T("CONNECT"));
	m_clientSocket = INVALID_SOCKET;
	m_editChatroom.SubclassDlgItem(EditBox_Chatroom, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize butt on to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
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

//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::OnBnClickedSend()
{
	CString strMessage;
	GetDlgItemText(IDC_EDIT4, strMessage);

	if (m_clientSocket == INVALID_SOCKET)
	{
		MessageBox(_T("Not connected to server!"));
		return;
	}

	const char* szMessage = CStringA(strMessage);

	int nResult = send(m_clientSocket, szMessage, strlen(szMessage), 0); 
	if (nResult == SOCKET_ERROR)
	{
		MessageBox(_T("Failed to send message to server!"));
	}
	else
	{
		CString strChatroom;
		m_editChatroom.GetWindowText(strChatroom);
		strChatroom += strMessage + _T("\r\n");
		m_editChatroom.SetWindowText(strChatroom);
	}
}



void CClientDlg::OnBnClicked1()
{
	// TODO: Add your control notification handler code here
	CString strButtonText;
	m_btnConnect.GetWindowTextW(strButtonText);

	if (strButtonText == _T("CONNECT"))
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			MessageBox(_T("Error: Unable to initialize Winsock."));
			return;
		}

		// Code to handle connection to server
		CString strServerAddr, strPort;
		m_editServerAddr.GetWindowTextW(strServerAddr);
		m_editPort.GetWindowTextW(strPort);

		// Convert CString to PCWSTR
		PCWSTR szServerAddr = strServerAddr.GetString();
		PCWSTR szPort = strPort.GetString();

		// Create socket
		m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_clientSocket == INVALID_SOCKET)
		{
			MessageBox(_T("Failed to initialize socket!"));
			WSACleanup();
			return;
		}

		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		if (InetPton(AF_INET, szServerAddr, &serverAddr.sin_addr) != 1)
		{
			MessageBox(_T("Invalid IP address!"));
			closesocket(m_clientSocket);
			WSACleanup();
			return;
		}
		serverAddr.sin_port = htons(_wtoi(szPort));

		if (connect(m_clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			MessageBox(_T("Failed to connect to server!"));
			closesocket(m_clientSocket);
			WSACleanup();
			return;
		}

		m_btnConnect.SetWindowTextW(_T("DISCONNECT"));
	}
	else if (strButtonText == _T("DISCONNECT"))
	{
		closesocket(m_clientSocket);
		m_btnConnect.SetWindowTextW(_T("CONNECT"));
		WSACleanup();
	}
}


void CClientDlg::OnBnClicked2()
{
	// TODO: Add your control notification handler code here
	CString strMessage;
	GetDlgItemText(IDC_EDIT4, strMessage);

	if (m_clientSocket == INVALID_SOCKET)
	{
		MessageBox(_T("Not connected to server!"));
		return;
	}

	const char* szMessage = CStringA(strMessage);

	int nResult = send(m_clientSocket, szMessage, strlen(szMessage), 0);
	if (nResult == SOCKET_ERROR)
	{
		MessageBox(_T("Failed to send message to server!"));
	}
	else
	{
		// update EditBox_Chatroom
		CString strChatroom;
		m_editChatroom.GetWindowText(strChatroom);
		strChatroom += strMessage + _T("\r\n"); 
		m_editChatroom.SetWindowText(strChatroom); 
	}
}
