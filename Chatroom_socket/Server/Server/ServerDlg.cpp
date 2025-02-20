
// ServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include <afxsock.h>
#include <WinSock2.h>

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


// CServerDlg dialog



CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Button_Start , m_btnStart);
	DDX_Control(pDX, EditBox_IP, m_editIP);
	DDX_Control(pDX, EditBox_Port, m_editPort);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CServerDlg::OnLvnItemchangedList2)
	ON_BN_CLICKED(Button_Start, &CServerDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
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
	CRect rect_Log;
	CRect rect_Connect;

	CListCtrl* list_Connect = (CListCtrl*)GetDlgItem(ListCrtl_Connect);
	CListCtrl* list_Log = (CListCtrl*)GetDlgItem(ListCrtl_Log);

	list_Connect->GetClientRect(rect_Connect);
	list_Log->GetClientRect(rect_Log);

	list_Log->InsertColumn(0, L"Time", 0, rect_Log.Width() / 4);
	list_Log->InsertColumn(1, L"Event", 0, rect_Log.Width() * 3 / 4);

	list_Connect->InsertColumn(0, L"IP Address", 0, rect_Connect.Width() * 3 / 4);
	list_Connect->InsertColumn(1, L"Port", 0, rect_Connect.Width() / 4);
	return TRUE;
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlg::OnPaint()
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

HCURSOR CServerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::OnLvnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}


void CServerDlg::AddLogEntry(const CString& strTime, const CString& strEvent, CListCtrl* list_Log)
{
    if (list_Log)
    {
        int nIndex = list_Log->GetItemCount();
        list_Log->InsertItem(nIndex, strTime);
        list_Log->SetItemText(nIndex, 1, strEvent);
    }
}


void CServerDlg::AddConnection(const CString& strIP, const CString& strPort) 
{
    CListCtrl* list_Connect = (CListCtrl*)GetDlgItem(ListCrtl_Connect);
    if (list_Connect) 
    {
        int nIndex = list_Connect->GetItemCount();
        list_Connect->InsertItem(nIndex, strIP);
        list_Connect->SetItemText(nIndex, 1, strPort);

        CString strTime = CTime::GetCurrentTime().Format(_T("%H:%M:%S"));
        CString strEvent = _T("Client connected from IP: ") + strIP + _T(", Port: ") + strPort;
        AddLogEntry(strTime, strEvent, reinterpret_cast<CListCtrl*>(GetDlgItem(ListCrtl_Log)));
    }
}

void CServerDlg::RemoveConnection(const CString& strIP, const CString& strPort)
{
    CListCtrl* list_Connect = (CListCtrl*)GetDlgItem(ListCrtl_Connect);
    if (list_Connect) {
        int nItemCount = list_Connect->GetItemCount();
        for (int nIndex = 0; nIndex < nItemCount; ++nIndex) {
            CString strItemIP = list_Connect->GetItemText(nIndex, 0);
            CString strItemPort = list_Connect->GetItemText(nIndex, 1);
            if (strItemIP == strIP && strItemPort == strPort) {
                list_Connect->DeleteItem(nIndex);
                nItemCount--;

                break;
            }
        }
    }
}




DWORD WINAPI CServerDlg::ListenThreadProc(LPVOID lpParam)
{
    CServerDlg* pServerDlg = reinterpret_cast<CServerDlg*>(lpParam);

    while (true)
    {
        SOCKET clientSocket;
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        char strAddr[INET_ADDRSTRLEN];

        clientSocket = accept(pServerDlg->m_serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);

 
        if (inet_ntop(AF_INET, &clientAddr.sin_addr, strAddr, INET_ADDRSTRLEN) == NULL)
        {
            closesocket(clientSocket);
            continue;
        }


        CString strPort;
        pServerDlg->m_editPort.GetWindowTextW(strPort);
        pServerDlg->AddConnection(CString(strAddr), strPort);

        HANDLE hClientThread = CreateThread(NULL, 0, &CServerDlg::ClientThreadProc, reinterpret_cast<LPVOID>(clientSocket), 0, NULL);
        if (hClientThread == NULL)
        {
            closesocket(clientSocket);
            continue;
        }
        else
        {
            CloseHandle(hClientThread);
        }
    }

    return 0;
}




DWORD WINAPI CServerDlg::ClientThreadProc(LPVOID lpParam)
{
    SOCKET clientSocket = reinterpret_cast<SOCKET>(lpParam);
    CServerDlg* pServerDlg = reinterpret_cast<CServerDlg*>(AfxGetApp()->GetMainWnd());
    if (pServerDlg == nullptr) {
        return 1;
    }

    char recvBuffer[1024];
    int recvResult;

    while (true)
    {
        recvResult = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (recvResult == 0)
        {
            closesocket(clientSocket);

            CString strTime = CTime::GetCurrentTime().Format(_T("%H:%M:%S"));
            CString strEvent = _T("Client disconnected");
            pServerDlg->AddLogEntry(strTime, strEvent, reinterpret_cast<CListCtrl*>(pServerDlg->GetDlgItem(ListCrtl_Log)));
            CListCtrl* list_Connect = (CListCtrl*)pServerDlg->GetDlgItem(ListCrtl_Connect);
            if (list_Connect)
            {
                list_Connect->DeleteAllItems();
            }

            break;
        }
        else if (recvResult == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            break;
        }
    }

    return 0;
}




void CServerDlg::OnBnClickedStart()
{
    CString strButtonText;
    m_btnStart.GetWindowTextW(strButtonText);

    if (strButtonText == _T("START"))
    {
        CString strIP, strPort;
        m_editIP.GetWindowTextW(strIP);
        m_editPort.GetWindowTextW(strPort);
        if (strIP.IsEmpty() || strPort.IsEmpty())
        {
            MessageBox(_T("Please enter IP and Port before starting the server."));
            return;
        }

        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            MessageBox(_T("Error: Unable to initialize Winsock."));
            return;
        }

        unsigned short usPort = (unsigned short)_ttoi(strPort);

        m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_serverSocket == INVALID_SOCKET)
        {
            MessageBox(_T("Failed to create server socket!"));
            WSACleanup();
            return;
        }

        // Bind server socket
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(usPort);
        if (bind(m_serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            MessageBox(_T("Failed to bind server socket!"));
            closesocket(m_serverSocket);
            WSACleanup();
            return;
        }

        // Listen connections
        if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            MessageBox(_T("Failed to listen for incoming connections!"));
            closesocket(m_serverSocket);
            WSACleanup();
            return;
        }

        CString strTime = CTime::GetCurrentTime().Format(_T("%H:%M:%S"));
        CString strEvent = _T("Server started listening on IP: ") + strIP + _T(", Port: ") + strPort;
        AddLogEntry(strTime, strEvent, reinterpret_cast<CListCtrl*>(GetDlgItem(ListCrtl_Log)));

        m_btnStart.SetWindowTextW(_T("STOP"));

        m_hListenThread = CreateThread(NULL, 0, ListenThreadProc, this, 0, NULL);
        if (m_hListenThread == NULL)
        {
            MessageBox(_T("Failed to create listen thread!"));
            closesocket(m_serverSocket);
            WSACleanup();
            return;
        }
    }
    else if (strButtonText == _T("STOP"))
    {
        closesocket(m_serverSocket);
        WSACleanup();

        CString strTime = CTime::GetCurrentTime().Format(_T("%H:%M:%S"));
        CString strEvent = _T("Server stopped listening");
        AddLogEntry(strTime, strEvent, reinterpret_cast<CListCtrl*>(GetDlgItem(ListCrtl_Log)));

        m_btnStart.SetWindowTextW(_T("START"));
    }
}
