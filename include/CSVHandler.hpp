#pragma once

#include "OperatorInfo.hpp"
#include "AccessStrategyDeduct.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// CSV处理工具类
class CSVHandler {
public:
    // 单例模式
    static CSVHandler& getInstance() {
        static CSVHandler instance;
        return instance;
    }

    // 从CSV文件读取算子信息
    void readOperatorInfo(const std::string& opName, const std::string& csvPath, OperatorInfo& op);

    // 将访存特征和策略信息写入CSV文件
    void writeAccessStrategy(const std::string& opName, const std::string& dataset,
                           const std::string& funcName, const AccessFeatureVector& featureVector);

    // 检查文件是否存在
    bool isFileExists(const std::string& path);
    
    // 设置输出UTF-8 BOM选项
    void setOutputUTF8BOM(bool value) { outputUTF8BOM = value; }

private:
    CSVHandler() = default;
    ~CSVHandler() = default;
    CSVHandler(const CSVHandler&) = delete;
    CSVHandler& operator=(const CSVHandler&) = delete;

    // 检查文件是否存在（内部使用）
    bool fileExists(const std::string& path);

    // 创建目录
    void createDirectory(const std::string& path);

    // CSV列名（新的顺序）
    const std::vector<std::string> strategyColumns = {
        "计算负载", "核函数名", "变量名", 
        "预分配空间大小", "数据块大小", "访存次数", 
        "访存步长和占比", "访存密度", "访存空间局部性", 
        "访存策略名", "line", "set"
    };

    // 构建模式字符串
    std::string buildPatternsString(const std::vector<std::pair<int, double>>& patterns);

    // 验证并修复访存密度和空间局部性的值
    void validateAndFixMetrics(double& density, double& locality);
    
    // 控制输出UTF-8 BOM的标志
    bool outputUTF8BOM = true;
    
    // 字符串编码转换函数
    std::wstring utf8ToWide(const std::string& str);
    std::string wideToUtf8(const std::wstring& wstr);
};