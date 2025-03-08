#include "CSVHandler.hpp"
#include <sys/stat.h>
#include <iostream>
#include <cmath>
#include <locale>
#include <codecvt>

bool CSVHandler::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool CSVHandler::isFileExists(const std::string& path) {
    return fileExists(path);
}

void CSVHandler::createDirectory(const std::string& path) {
    mkdir(path.c_str(), 0777);
}

std::string CSVHandler::buildPatternsString(const std::vector<std::pair<int, double>>& patterns) {
    std::stringstream patternsStr;
    bool first = true;
    for (const auto& pattern : patterns) {
        if (!first) {
            patternsStr << ";";
        }
        patternsStr << pattern.first << "(" << std::fixed << std::setprecision(2) << pattern.second * 100 << "%)";
        first = false;
    }
    return patternsStr.str();
}

void CSVHandler::validateAndFixMetrics(double& density, double& locality) {
    // 确保密度非负
    if (density < 0) {
        std::cerr << "警告：发现负的访存密度 " << density << "，已修正为0" << std::endl;
        density = 0;
    }
    
    // 确保局部性在[0,1]范围内
    if (locality < 0) {
        std::cerr << "警告：发现负的空间局部性 " << locality << "，已修正为0" << std::endl;
        locality = 0;
    }
    if (locality > 1) {
        std::cerr << "警告：发现超过1的空间局部性 " << locality << "，已修正为1" << std::endl;
        locality = 1;
    }
}

void CSVHandler::readOperatorInfo(const std::string& opName, const std::string& csvPath, OperatorInfo& op) {
    op.getOperatorInfoFromCSV(opName, csvPath);
}

void CSVHandler::writeAccessStrategy(const std::string& opName, const std::string& dataset,
                                   const std::string& funcName, const AccessFeatureVector& featureVector) {
    // 如果不存在，创建results目录
    createDirectory("results");
    
    // 构建CSV文件名：results/opName.csv
    std::string csvFileName = "results/" + opName + ".csv";
    bool isNewFile = !fileExists(csvFileName);
    
    // 使用wofstream实现UTF-8输出
    std::locale utf8Locale(std::locale(), new std::codecvt_utf8<wchar_t>());
    std::wofstream csvFile;
    csvFile.imbue(utf8Locale);
    
    if (isNewFile) {
        csvFile.open(csvFileName);
        
        // 如果启用，写入UTF-8 BOM
        if (outputUTF8BOM) {
            csvFile.put(L'\xFEFF');  // UTF-8 BOM作为宽字符
        }
        
        // 写入CSV头部 - 转换为宽字符串
        for (size_t i = 0; i < strategyColumns.size(); ++i) {
            // 从UTF-8转换为宽字符串
            std::wstring wideColumn = utf8ToWide(strategyColumns[i]);
            csvFile << wideColumn;
            if (i < strategyColumns.size() - 1) {
                csvFile << L",";
            }
        }
        csvFile << L"\n";
    } else {
        csvFile.open(csvFileName, std::ios::app);
    }
    
    // 获取并验证访存密度和空间局部性
    double density = featureVector.D;
    double locality = featureVector.L;
    validateAndFixMetrics(density, locality);
    
    // 按新顺序写入数据行（将字符串转换为宽字符串）
    csvFile << utf8ToWide(dataset + "_DATASET") << L","              // 计算负载
            << utf8ToWide(funcName) << L","                          // 核函数名
            << utf8ToWide(featureVector.varName) << L","             // 变量名
            << featureVector.C << L","                               // 预分配空间大小
            << featureVector.S << L","                               // 数据块大小
            << featureVector.N << L","                               // 访存次数
            << utf8ToWide(buildPatternsString(featureVector.patterns)) << L","  // 访存步长和占比
            << std::fixed << std::setprecision(6) << density << L","            // 访存密度
            << std::fixed << std::setprecision(6) << locality << L","           // 访存空间局部性
            << utf8ToWide(featureVector.accessStrategyConfig.getStrategyName()) << L","  // 访存策略名
            << featureVector.accessStrategyConfig.line << L","                  // line参数
            << featureVector.accessStrategyConfig.set                           // set参数
            << L"\n";
            
    csvFile.close();
}

// 将UTF-8字符串转换为宽字符串
std::wstring CSVHandler::utf8ToWide(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

// 将宽字符串转换为UTF-8字符串
std::string CSVHandler::wideToUtf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}