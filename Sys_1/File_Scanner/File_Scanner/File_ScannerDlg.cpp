
// File_ScannerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "File_Scanner.h"
#include "File_ScannerDlg.h"
#include "afxdialogex.h"

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


// CFileScannerDlg dialog



CFileScannerDlg::CFileScannerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILE_SCANNER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileScannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFileScannerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_button_Find, &CFileScannerDlg::OnBnClickedbuttonFind)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_listControl_Output, &CFileScannerDlg::OnLvnItemchangedlistcontrolOutput)
END_MESSAGE_MAP()




// CFileScannerDlg message handlers

BOOL CFileScannerDlg::OnInitDialog()
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
	CListCtrl* mylist = (CListCtrl*)GetDlgItem(IDC_listControl_Output);
	mylist->GetClientRect(rect);
	mylist->InsertColumn(0, L"Name", 0, rect.Width() / 7);
	mylist->InsertColumn(1, L"Size", 0, rect.Width() / 7);
	mylist->InsertColumn(2, L"Full Path", 0, rect.Width()*3 / 7);
	mylist->InsertColumn(3, L"Create Time", 0, rect.Width() / 7);
	mylist->InsertColumn(4, L"Modify Time", 0, rect.Width() / 7);

	GetClientRect(&m_oldClientRect);

	ModifyStyle(WS_THICKFRAME | WS_MAXIMIZEBOX, 0, SWP_DRAWFRAME);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileScannerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileScannerDlg::OnPaint()
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
HCURSOR CFileScannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CFileScannerDlg::OnBnClickedbuttonFind()
{
	CString strFolderPath;
	GetDlgItem(IDC_edit_Path)->GetWindowText(strFolderPath);

	CString strFileName;
	GetDlgItem(IDC_edit_FileName)->GetWindowText(strFileName);

	if (strFolderPath.IsEmpty()) {
		AfxMessageBox(_T("Vui lòng nhập đường dẫn thư mục."));
		return;
	}

	if (strFileName.IsEmpty()) {
		AfxMessageBox(_T("Vui lòng nhập tên file."));
		return;
	}

	CString searchPath = strFolderPath + _T("\\") + _T("*.*");


	CListCtrl* myList = (CListCtrl*)GetDlgItem(IDC_listControl_Output);
	myList->DeleteAllItems();

	SearchFiles(strFolderPath, strFileName, myList);
}

void CFileScannerDlg::SearchFiles(const CString& folder, const CString& fileName, CListCtrl* myList)
{
	CString searchPath = folder + _T("\\") + _T("*.*");

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(searchPath, &findFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (_tcscmp(findFileData.cFileName, _T(".")) != 0 && _tcscmp(findFileData.cFileName, _T("..")) != 0)
			{
				CString fullPath = folder + _T("\\") + findFileData.cFileName;

				// Kiểm tra nếu là thư mục
				if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					// Đệ quy để tìm kiếm trong thư mục con
					SearchFiles(fullPath, fileName, myList);
				}
				else
				{
					// So sánh tên file và thêm vào danh sách nếu khớp
					if (_tcsstr(findFileData.cFileName, fileName) != NULL)
					{
						CString fileSize;
						fileSize.Format(_T("%u"), findFileData.nFileSizeLow);

						// Định dạng ngày tạo và ngày sửa
						FILETIME ftCreate = findFileData.ftCreationTime;
						FILETIME ftLastAccess = findFileData.ftLastAccessTime;
						FILETIME ftLastWrite = findFileData.ftLastWriteTime;
						SYSTEMTIME stCreate, stLastAccess, stLastWrite;

						FileTimeToSystemTime(&ftCreate, &stCreate);
						FileTimeToSystemTime(&ftLastAccess, &stLastAccess);
						FileTimeToSystemTime(&ftLastWrite, &stLastWrite);

						CString creationDate, lastAccessDate, lastWriteDate;
						creationDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
							stCreate.wMonth, stCreate.wDay, stCreate.wYear,
							stCreate.wHour, stCreate.wMinute, stCreate.wSecond);
						lastAccessDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
							stLastAccess.wMonth, stLastAccess.wDay, stLastAccess.wYear,
							stLastAccess.wHour, stLastAccess.wMinute, stLastAccess.wSecond);
						lastWriteDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
							stLastWrite.wMonth, stLastWrite.wDay, stLastWrite.wYear,
							stLastWrite.wHour, stLastWrite.wMinute, stLastWrite.wSecond);

						ULONGLONG fileSizeBytes = findFileData.nFileSizeLow + ((ULONGLONG)findFileData.nFileSizeHigh << 32);


						if (fileSizeBytes < 1024)
						{
							fileSize.Format(_T("%u B"), findFileData.nFileSizeLow);
						}
						else if (fileSizeBytes < 1024 * 1024)
						{
							fileSize.Format(_T("%.2f KB"), static_cast<double>(fileSizeBytes) / 1024);
						}
						else if (fileSizeBytes < 1024 * 1024 * 1024)
						{
							fileSize.Format(_T("%.2f MB"), static_cast<double>(fileSizeBytes) / (1024 * 1024));
						}
						else
						{
							fileSize.Format(_T("%.2f GB"), static_cast<double>(fileSizeBytes) / (1024 * 1024 * 1024));
						}

						int nIndex = myList->InsertItem(myList->GetItemCount(), findFileData.cFileName);
						myList->SetItemText(nIndex, 1, fileSize);
						myList->SetItemText(nIndex, 2, fullPath);
						myList->SetItemText(nIndex, 3, creationDate);
						myList->SetItemText(nIndex, 4, lastWriteDate);
					}
				}
			}
		} while (FindNextFile(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
}





void CFileScannerDlg::OnLvnItemchangedlistcontrolOutput(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
