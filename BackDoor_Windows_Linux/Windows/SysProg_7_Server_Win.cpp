#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") 

#define SERVER_PORT 4444
#define BUFFER_SIZE 4096

using namespace std;

void send_command(SOCKET conn) {
    try {
        while (true) {
            cout << "Enter command to send to client (type 'exit' to quit):" << endl;
            string command;
            getline(cin, command);
            if (command == "exit") {
                break;
            }
            send(conn, command.c_str(), command.length(), 0);
            char buffer[BUFFER_SIZE] = {0};
            recv(conn, buffer, BUFFER_SIZE, 0);
            cout << "Response from client: " << buffer << endl;
        }
    } catch (...) {
        cerr << "\nError occurred while sending/receiving data." << endl;
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error: Unable to initialize Winsock." << endl;
        return 1;
    }

    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        cerr << "Error: Unable to create socket." << endl;
        WSACleanup();
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Error: Binding failed." << endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 1) == SOCKET_ERROR) {
        cerr << "Error: Listening failed." << endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    cout << "Waiting for incoming connections" << endl;

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
    if (client_socket == INVALID_SOCKET) {
        cerr << "Error: Accepting connection failed." << endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to client." << endl;

    try {
        send_command(client_socket);
    } catch (...) {
        cerr << "\nError occurred during communication with client." << endl;
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
