import socket
import sys

SERVER_HOST = "192.168.252.145"
SERVER_PORT = 4444

def send_command(conn):
    try:
        while True:
            print("Enter command to send to client (type 'exit' to quit):")
            command = input()
            if command.lower() == "exit":
                break
            conn.send(command.encode())
            response = conn.recv(4096).decode()
            print("Response from client:", response)
    except KeyboardInterrupt:
        print("\nClosing connection...")
        conn.close()
        sys.exit()



def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((SERVER_HOST, SERVER_PORT))
        s.listen(1)
        print("Waiting for incoming connections...")
        conn, addr = s.accept()
        print("Connected to client:", addr)
        try:
            send_command(conn)
        except ConnectionResetError:
            print("\nClient closed connection unexpectedly.")
        finally:
            print("\nClosing connection...")
            conn.close()

if __name__ == "__main__":
    main()
