import subprocess
import os
import time
import sys
import logging

logging.basicConfig(filename="script.log", level=logging.DEBUG)

def is_process_running(process_name):
    tasklist = subprocess.Popen(["tasklist", "/fo", "csv", "/nh"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
    output, _ = tasklist.communicate()
    return process_name.lower() in output.decode().lower()

def launch_ragnarok(ragnarok_exe_path):
    subprocess.Popen([ragnarok_exe_path], creationflags=subprocess.CREATE_NO_WINDOW)

def main():
    logging.info("Script started.")
    sys.stdout = open(os.devnull, 'w')

    ragnarok_exe_path = r"2018-05-30bRagexeRE-original_patched.exe"
    notepad_process_name = "notepad.exe"
    
    while True:
        try:
            if is_process_running(notepad_process_name):
                subprocess.Popen(["taskkill", "/f", "/im", os.path.basename(ragnarok_exe_path)], creationflags=subprocess.CREATE_NO_WINDOW)
                logging.info("Notepad found, killed Ragnarok process.")
                break
            else:
                if not is_process_running(os.path.basename(ragnarok_exe_path)):
                    launch_ragnarok(ragnarok_exe_path)
                    logging.info("Ragnarok launched.")
                
            time.sleep(5)
        except Exception as e:
            logging.error("An error occurred: %s", str(e))
            break

if __name__ == "__main__":
    main()
