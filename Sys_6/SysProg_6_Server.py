import socket
import threading

def handle_client(client_socket, address):
    try:
        while True:
            data = client_socket.recv(1024)
            if not data:
                break
            num_bytes = len(data)
            
            client_socket.sendall(str(num_bytes).encode())
    finally:
        client_socket.close()
        print(f"Client disconnected: {address}")

def server(ip, port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((ip, port))
    server_socket.listen(5)
    print(f'Server started on {ip}:{port}')
    try:
        while True:
            client_sock, address = server_socket.accept()
            print(f'Client connected: {address}')
            client_thread = threading.Thread(target=handle_client, args=(client_sock, address))
            client_thread.start()
    finally:
        server_socket.close()

if __name__ == "__main__":
    ip = input("Improt the server IP address: ")
    port = input("Import the port: ")
    server(ip, int(port))
