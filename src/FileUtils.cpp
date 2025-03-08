#include "FileUtils.hpp"
#include <sys/stat.h>
#include <iostream>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <dirent.h>
    #define MKDIR(dir) mkdir(dir, 0777)
#endif

namespace FileUtils {

bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

void createDirectory(const std::string& path) {
    if (!fileExists(path)) {
        if (MKDIR(path.c_str()) != 0) {
            std::cerr << "错误: 无法创建目录 " << path << std::endl;
        }
    }
}

std::vector<std::string> getSubdirectories(const std::string& path, 
                                         const std::vector<std::string>& excludeList) {
    std::vector<std::string> dirs;
    
#ifdef _WIN32
    // Windows 实现需要使用Windows API
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((path + "\\*").c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                std::string dirName = findData.cFileName;
                if (std::find(excludeList.begin(), excludeList.end(), dirName) == excludeList.end()) {
                    dirs.push_back(dirName);
                }
            }
        } while (FindNextFile(hFind, &findData) != 0);
        FindClose(hFind);
    } else {
        std::cerr << "错误: 无法打开目录 " << path << std::endl;
    }
#else
    // POSIX 实现
    DIR* dir = opendir(path.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_DIR) {
                std::string dirName = entry->d_name;
                if (std::find(excludeList.begin(), excludeList.end(), dirName) == excludeList.end()) {
                    dirs.push_back(dirName);
                }
            }
        }
        closedir(dir);
    } else {
        std::cerr << "错误: 无法打开目录 " << path << std::endl;
    }
#endif

    return dirs;
}

} 