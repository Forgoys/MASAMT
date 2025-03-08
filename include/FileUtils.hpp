#pragma once

#include <string>
#include <vector>

namespace FileUtils {
    // 默认的排除文件列表
    const std::vector<std::string> DEFAULT_EXCLUDE_FILES = {".", "..", ".DS_Store", ".git"};
    
    /**
     * @brief 检查文件是否存在
     * 
     * @param path 文件路径
     * @return true 文件存在
     * @return false 文件不存在
     */
    bool fileExists(const std::string& path);
    
    /**
     * @brief 创建目录
     * 
     * @param path 目录路径
     */
    void createDirectory(const std::string& path);
    
    /**
     * @brief 获取指定目录下的所有子目录
     * 
     * @param path 目录路径
     * @param excludeList 要排除的文件或目录列表
     * @return std::vector<std::string> 子目录名称列表
     */
    std::vector<std::string> getSubdirectories(const std::string& path, 
                                            const std::vector<std::string>& excludeList = DEFAULT_EXCLUDE_FILES);
} 