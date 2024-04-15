#include <iostream>
#include <Windows.h>
#include <string>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

using namespace std;

#define DEST_PATH "D:\\Documents"

int main()
{
    char current_script[MAX_PATH];
    GetModuleFileName(NULL, current_script, MAX_PATH);
    
    string script_name = PathFindFileNameA(current_script);

    string dest_script_path = DEST_PATH;
    dest_script_path += "\\";
    dest_script_path += script_name;

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        if (RegSetValueEx(hKey, "Script", 0, REG_SZ, (BYTE*)dest_script_path.c_str(), (dest_script_path.length() + 1) * sizeof(char)) == ERROR_SUCCESS)
        {
            cout << "Registry key set successfully." << endl;
        }
        RegCloseKey(hKey);
    }
    return 0;
}
