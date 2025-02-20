import subprocess
import os
import sys
import time
from urllib.request import urlretrieve


current_script = os.path.abspath(sys.argv[0])
url = ("https://github.com/Nghia-nee/Sys_Prog/raw/main/Sys_7/Linux/SysProg_7_Client.py")
filename = "Client.py"

def DelMe():
    os.remove(current_script)

def run():
    urlretrieve(url, filename)
    print("Da tai")
    
    process = subprocess.Popen(["python", "/home/nghia/Desktop/VCS/9/Client.py"])
    
    start_time = time.time()
    
    while True:
        poll = process.poll()
        if poll is not None:
            print("Subprocess hoan thanh")
            break
        
        elapsed_time = time.time() - start_time
        if elapsed_time > 5:
            print("Het 5s chuong trinh se tu ket thuc")
            process.terminate()
            process.wait()
            break

    print("Chuong trinh chuan bi xoa ban than")
    DelMe()
    
if __name__ == "__main__":
    run()
