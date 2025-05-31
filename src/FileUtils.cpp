#include "FileUtils.hpp"
#include <sys/stat.h>
#include <iostream>
#include <regex>

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

bool isLegacyCSVFormat(const std::string& filename, std::string& extractedDataset, std::string& extractedOpName) {
    // 传统格式：{DATASET}_DATASET_{opName}.csv
    // 使用正则表达式匹配
    std::regex legacyPattern(R"(^([A-Z]+)_DATASET_(.+)\.csv$)");
    std::smatch matches;
    
    if (std::regex_match(filename, matches, legacyPattern)) {
        extractedDataset = matches[1].str();
        extractedOpName = matches[2].str();
        return true;
    }
    
    return false;
}

std::vector<std::string> getCSVFiles(const std::string& path) {
    std::vector<std::string> csvFiles;
    
#ifdef _WIN32
    // Windows 实现
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((path + "\\*.csv").c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                csvFiles.push_back(findData.cFileName);
            }
        } while (FindNextFile(hFind, &findData) != 0);
        FindClose(hFind);
    }
#else
    // POSIX 实现
    DIR* dir = opendir(path.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) { // 普通文件
                std::string filename = entry->d_name;
                // 检查是否以.csv结尾
                if (filename.length() > 4 && filename.substr(filename.length() - 4) == ".csv") {
                    csvFiles.push_back(filename);
                }
            }
        }
        closedir(dir);
    } else {
        std::cerr << "错误: 无法打开目录 " << path << std::endl;
    }
#endif

    return csvFiles;
}

std::string getFileNameWithoutExtension(const std::string& filepath) {
    // 找到最后一个路径分隔符的位置
    size_t lastSlash = filepath.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? filepath : filepath.substr(lastSlash + 1);
    
    // 移除扩展名
    size_t lastDot = filename.find_last_of('.');
    if (lastDot != std::string::npos) {
        return filename.substr(0, lastDot);
    }
    
    return filename;
}

}