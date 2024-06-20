import subprocess
import os
import time
import sys
from win10toast import ToastNotifier

toaster = ToastNotifier()

def is_process_running(process_name_keywords):
    tasklist = subprocess.Popen(["tasklist", "/fo", "csv", "/nh"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
    output, _ = tasklist.communicate()
    output = output.decode().lower()
    for keyword in process_name_keywords:
        if keyword in output:
            return True
    return False

def launch_ragnarok(ragnarok_exe_path):
    subprocess.Popen([ragnarok_exe_path], creationflags=subprocess.CREATE_NO_WINDOW)

def main():
    ragnarok_exe_path = r"2018-05-30bRagexeRE-original_patched.exe"
    forbidden_keywords = ["cheat", "hack", "cmd", "bot", "inject"]
    ragnarok_launched = False
    
    while True:
        try:
            if is_process_running(forbidden_keywords):
                toaster.show_toast("Forbidden Process Detected", "A forbidden process has been detected. Ragnarok will be terminated.")
                subprocess.Popen(["taskkill", "/f", "/im", os.path.basename(ragnarok_exe_path)], creationflags=subprocess.CREATE_NO_WINDOW)
                break
            else:
                if not ragnarok_launched:
                    launch_ragnarok(ragnarok_exe_path)
                    ragnarok_launched = True
                
            time.sleep(5)
        except Exception as e:
            print("An error occurred:", str(e))
            break

if __name__ == "__main__":
    main()
