import os

def merge_files(root_dir, output_file, exclude_dirs=None, exclude_files=None):
    if exclude_dirs is None:
        exclude_dirs = []
    if exclude_files is None:
        exclude_files = []
    
    with open(output_file, 'w', encoding='utf-8') as out_file:
        for subdir, dirs, files in os.walk(root_dir):
            # Skip excluded directories
            if any(excluded in subdir for excluded in exclude_dirs):
                continue
            
            for file in files:
                if file in exclude_files:
                    continue
                
                file_path = os.path.join(subdir, file)
                
                try:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        out_file.write(f"\n--- FILE: {file_path} ---\n\n")
                        out_file.write(f.read())
                        out_file.write("\n\n")
                except Exception as e:
                    print(f"Skipping {file_path} due to error: {e}")

if __name__ == "__main__":
    root_directory = "."  # Change this to your C++ project root directory
    output_filename = "merged_project.txt"
    exclude_dirs_list = ["build", "bin", "out", ".idea", ".vscode", "cmake-build-debug", "resources"]  # Directories to exclude
    exclude_files_list = [
    	"CMakeLists.txt", 
    	"Makefile", 
    	"merge_files.py", 
    	"merged_project.txt", 
    	"ReadMe.md", 
    	"settings.cpp",
    	"settings.h", 
    	"timer.cpp",
    	"timer.h",
    	"mainwindow.cpp",
    	"mainwindow.h"
    	]  # Specific files to exclude
    
    merge_files(root_directory, output_filename, exclude_dirs_list, exclude_files_list)
    print(f"All files merged into {output_filename}")

