import os
import shutil
import subprocess

def get_c_files():
    return [file for file in os.listdir() if file.endswith('.c')]

def try_compile():
    command = [
        "cc",
        *get_c_files(),
        "-o", "temp_executable",
        "-arch", "x86_64",
        "-shared",
        "-undefined", "dynamic_lookup"
    ]
    result = subprocess.run(command)
    return result.returncode == 0

def get_includes(file_name):
    with open(file_name, 'r') as f:
        lines = f.readlines()
    return [line.strip() for line in lines if line.startswith('#include')]

def remove_line(file_name, line):
    with open(file_name, 'r') as f:
        lines = f.readlines()
    with open(file_name, 'w') as f:
        for l in lines:
            if l.strip() != line:
                f.write(l)

def backup(file_name):
    shutil.copyfile(file_name, f"{file_name}.bak")

def restore(file_name):
    shutil.copyfile(f"{file_name}.bak", file_name)

def try_to_remove_include(file_name, include):
    backup(file_name)
    remove_line(file_name, include)
    if not try_compile():
        restore(file_name)
    print(file_name, include);

def clean_file(file_name):
    includes = get_includes(file_name)
    
    for include in includes:
        try_to_remove_include(file_name, include)

def main():
    c_files = get_c_files()

    for c_file in c_files:
        clean_file(c_file)

if __name__ == "__main__":
    main()
