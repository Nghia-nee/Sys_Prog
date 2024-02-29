
// Proccess_ViewerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Proccess_Viewer.h"
#include "Proccess_ViewerDlg.h"
#include "afxdialogex.h"
#include <psapi.h>
#include <string> 
#include <tchar.h>



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


// CProccessViewerDlg dialog



CProccessViewerDlg::CProccessViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCCESS_VIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProccessViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProccessViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_button_Refresh, &CProccessViewerDlg::OnBnClickedbuttonRefresh)
END_MESSAGE_MAP()


// CProccessViewerDlg message handlers

BOOL CProccessViewerDlg::OnInitDialog()
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
	CRect rect;
	CListCtrl* mylist = (CListCtrl*)GetDlgItem(IDC_LIST1);
	mylist->GetClientRect(rect);
	mylist->InsertColumn(0, L"Process ID", 0, rect.Width() / 15);
	mylist->InsertColumn(1, L"Process Name", 0, rect.Width() * 2 / 15);
	mylist->InsertColumn(2, L"Command line", 0, rect.Width() * 6 / 15);
	mylist->InsertColumn(3, L"Full path", 0, rect.Width() * 6 / 15);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProccessViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CProccessViewerDlg::OnPaint()
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
HCURSOR CProccessViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProccessViewerDlg::OnBnClickedbuttonRefresh()
{
	// TODO: Add your control notification handler code here
	CListCtrl* mylist = (CListCtrl*)GetDlgItem(IDC_LIST1);
	mylist->DeleteAllItems();

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		// Handle the error, if needed.
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.
	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			PrintProcessNameAndID(aProcesses[i], mylist);
		}
	}
}

void CProccessViewerDlg::PrintProcessNameAndID(DWORD processID, CListCtrl* mylist)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	TCHAR szProcessPath[MAX_PATH] = TEXT("");
	TCHAR szCommandLine[MAX_PATH] = TEXT("");

	// Get a handle to the process.
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

	// Get the process name and full path.
	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
		{
			// Get the process name.
			GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));

			// Get the full path of the process.
			GetModuleFileNameEx(hProcess, hMod, szProcessPath, sizeof(szProcessPath) / sizeof(TCHAR));

			// Get the command line of the process.
			GetProcessCommandLine(processID, szCommandLine, sizeof(szCommandLine) / sizeof(TCHAR));
		}
	}

	// Insert the information into the list control.
	int nIndex = mylist->InsertItem(mylist->GetItemCount(), std::to_wstring(processID).c_str());
	mylist->SetItemText(nIndex, 1, szProcessName);
	mylist->SetItemText(nIndex, 2, szCommandLine);
	mylist->SetItemText(nIndex, 3, szProcessPath);

	// Release the handle to the process.
	CloseHandle(hProcess);
}

void CProccessViewerDlg::GetProcessCommandLine(DWORD processID, LPTSTR szCommandLine, DWORD bufferSize)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

	if (hProcess != NULL)
	{
		DWORD dwSize = bufferSize;
		if (QueryFullProcessImageName(hProcess, 0, szCommandLine, &dwSize) == 0)
		{
			// Handle the error, if needed.
		}

		CloseHandle(hProcess);
	}
}
