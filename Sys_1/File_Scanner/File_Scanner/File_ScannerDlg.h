// File_ScannerDlg.h : header file
//

#pragma once

// CFileScannerDlg dialog
class CFileScannerDlg : public CDialogEx
{
// Construction
public:
    CFileScannerDlg(CWnd* pParent = nullptr);    // standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FILE_SCANNER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void SearchFiles(const CString& folder, const CString& fileName, CListCtrl* myList);
    CRect m_oldClientRect;

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
    afx_msg void OnBnClickedbuttonFind();

    afx_msg void OnLvnItemchangedlistcontrolOutput(NMHDR* pNMHDR, LRESULT* pResult);
};
