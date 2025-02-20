import socket
import threading

def receive_messages(sock):
    while True:
        data = sock.recv(1024)
        if not data:
            break
        print(f'Received: {data.decode()}')
    print('Connection closed.')

def send_messages(sock):
    try:
        while True:
            message = input('')
            sock.sendall(message.encode())
    except EOFError:
        pass 

def client():
    ip = input("Import the server IP address: ")
    port = input("Import the port: ")

    try:
        port = int(port)
    except ValueError:
        print("Invalid port number. Try again.")
        return
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((ip, port))
        print(f'Connected to server on {ip}:{port}')
        
        while True:
            message = input('Enter your message: ')
            if message.lower() == 'exit':
                break
            sock.sendall(message.encode())
            
            response = sock.recv(1024)
            print(f'Response from server: {response.decode()}')
    except ConnectionRefusedError:
        print("Connection refused.")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        sock.close()

if __name__ == "__main__":
    client()
