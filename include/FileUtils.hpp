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
    
    /**
     * @brief 检查CSV文件名是否为传统格式并提取信息
     * 
     * @param filename 文件名（不包含路径）
     * @param extractedDataset 提取的数据集名称（输出参数）
     * @param extractedOpName 提取的算子名称（输出参数）
     * @return true 是传统格式
     * @return false 不是传统格式
     */
    bool isLegacyCSVFormat(const std::string& filename, std::string& extractedDataset, std::string& extractedOpName);
    
    /**
     * @brief 获取指定目录下的所有CSV文件
     * 
     * @param path 目录路径
     * @return std::vector<std::string> CSV文件名列表
     */
    std::vector<std::string> getCSVFiles(const std::string& path);
    
    /**
     * @brief 从文件路径中提取文件名（不包含路径和扩展名）
     * 
     * @param filepath 文件路径
     * @return std::string 文件名
     */
    std::string getFileNameWithoutExtension(const std::string& filepath);
}