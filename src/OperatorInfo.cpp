#include "OperatorInfo.hpp"
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <iomanip>


VariableInfo::VariableInfo(const std::string& name, const unsigned long long size, 
                         const unsigned long long access, const std::vector<std::pair<int, double>>& patterns)
{
    this->name = name;
    this->size = size;
    this->access = access;
    this->patterns = patterns;
}

FunctionInfo::FunctionInfo(const std::string& name, const std::vector<VariableInfo>& variables)
{
    this->name = name;
    this->variables = variables;
}

OperatorInfo::OperatorInfo(const std::string& name, const std::vector<FunctionInfo>& functions)
{
    this->name = name;
    this->functions = functions;
}

void OperatorInfo::getOperatorInfoFromCSV(const std::string& opName, const std::string& csvPath) {
    // 设置算子名称
    this->name = opName;
    
    // 创建一个map来存储每个函数的变量信息
    std::map<std::string, std::vector<VariableInfo>> functionVariables;
    
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << csvPath << std::endl;
        return;
    }
    
    std::string line;
    // 跳过标题行
    std::getline(file, line);
    
    // 读取每一行数据
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        // 将一行分割成多个字段
        while (std::getline(ss, token, ',')) {
            if (!token.empty()) {  // 只添加非空字段
                tokens.push_back(token);
            }
        }
        
        // 检查是否至少有基本字段（变量名、函数名、元素数量、访问次数）
        if (tokens.size() < 4) {
            std::cerr << "警告: 行数据格式不正确，跳过该行" << std::endl;
            continue;
        }
        
        std::string varName = tokens[0];
        std::string funcName = tokens[1];
        unsigned long long size = std::stoull(tokens[2]);
        unsigned long long access = std::stoull(tokens[3]);
        
        // 创建pattern信息
        std::vector<std::pair<int, double>> patterns;
        
        // 从第5个字段开始，每两个字段组成一对步长与占比
        for (size_t i = 4; i + 1 < tokens.size(); i += 2) {
            try {
                int step = std::stoi(tokens[i]);
                double percentage = std::stod(tokens[i + 1]) / 100.0;
                patterns.push_back({step, percentage});
            } catch (const std::exception& e) {
                std::cerr << "警告: 解析步长或占比失败: " << e.what() << std::endl;
                break;
            }
        }
        
        // 创建变量信息
        VariableInfo varInfo(varName, size, access, patterns);
        
        // 将变量信息添加到对应的函数中
        functionVariables[funcName].push_back(varInfo);
    }
    
    // 将map中的数据转换为FunctionInfo对象列表
    std::vector<FunctionInfo> functions;
    for (const auto& pair : functionVariables) {
        functions.push_back(FunctionInfo(pair.first, pair.second));
    }
    
    // 设置functions成员变量
    this->functions = functions;
    
    file.close();
}

void OperatorInfo::printInfo() const {
    std::cout << "[算子] " << name << " (" << functions.size() << "个函数)" << std::endl;
    
    for (const auto& func : functions) {
        std::cout << "\n[函数] " << func.name << " (" << func.variables.size() << "个变量)" << std::endl;
        
        for (const auto& var : func.variables) {
            std::cout << "  |- " << var.name 
                     << " (大小: " << var.size << "B, 访问次数: " << var.access << ")";
            
            if (!var.patterns.empty()) {
                std::cout << ": ";
                bool first = true;
                for (const auto& pattern : var.patterns) {
                    if (!first) std::cout << ", ";
                    std::cout << "步长" << pattern.first << "(" 
                            << std::fixed << std::setprecision(1) 
                            << pattern.second * 100 << "%)";
                    first = false;
                }
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}