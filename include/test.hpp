#include "OperatorInfo.hpp"
#include "AccessStrategyDeduct.hpp"
#include <iostream>
#include <cmath>

int test_operator_info() {
    // 创建OperatorInfo对象
    OperatorInfo op;
    
    // 测试不同数据集大小的CSV文件
    std::string datasets[] = {"MINI", "SMALL", "STANDARD", "LARGE", "EXTRALARGE"};
    
    for (const auto& dataset : datasets) {
        std::cout << "\n========== 测试 " << dataset << " 数据集 ==========\n" << std::endl;
        
        std::string opName = "adi";
        std::string csvPath = "data/" + opName + "/" + dataset + "_DATASET_" + opName + ".csv";
        
        // 读取CSV文件
        op.getOperatorInfoFromCSV(opName, csvPath);
        
        // 打印信息
        op.printInfo();
    }
    
    return 0;
}

/*
 * 计算空间局部性
 * @param patterns: 步长与占比的列表
 * @return: 空间局部性
 * 
 * 空间局部性计算公式：SL = Σ(p_i * e^(-s_i))
 * 其中p_i为步长s_i的访问占比,e^(-s_i)为步长s_i对应的衰减因子
 */
double calcuSpatialLocality(const std::vector<std::pair<int, double>>& patterns) {
    if (patterns.empty()) {
        return 0.0;
    }
    double sum = 0.0;
    for (const auto& pattern : patterns) {
        sum += pattern.second * exp(-pattern.first);
    }
    return sum;
}

int test_spatial_locality() {
    // 1. 步长为0，占比从0%到100%
    std::cout << "1. 步长为0:" << std::endl;
    for (int i = 0; i <= 10; i++) {
        double p = i * 0.1;
        std::cout << p * 100 << "%: " << calcuSpatialLocality({{0, p}}) << std::endl;
    }

    // 2. 步长为1，占比从0%到100%
    std::cout << "\n2. 步长为1:" << std::endl;
    for (int i = 0; i <= 10; i++) {
        double p = i * 0.1;
        std::cout << p * 100 << "%: " << calcuSpatialLocality({{1, p}}) << std::endl;
    }

    // 3. 步长0和1，总占比100%
    std::cout << "\n3. 步长0和1:" << std::endl;
    for (int i = 0; i <= 10; i++) {
        double p0 = i * 0.1;
        std::cout << "0:" << p0 * 100 << "%, 1:" << (1-p0) * 100 << "% -> " 
                  << calcuSpatialLocality({{0, p0}, {1, 1-p0}}) << std::endl;
    }

    // 4. 步长0和x(2到1024)
    std::cout << "\n4. 步长0和x:" << std::endl;
    for (int x = 2; x <= 1024; x *= 2) {
        for (int i = 0; i <= 10; i++) {
            double p0 = i * 0.1;
            std::cout << "x=" << x << ", 0:" << p0 * 100 << "% -> " 
                      << calcuSpatialLocality({{0, p0}, {x, 1-p0}}) << std::endl;
        }
    }

    // 5. 步长1和x(2到1024)
    std::cout << "\n5. 步长1和x:" << std::endl;
    for (int x = 2; x <= 1024; x *= 2) {
        for (int i = 0; i <= 10; i++) {
            double p1 = i * 0.1;
            std::cout << "x=" << x << ", 1:" << p1 * 100 << "% -> " 
                      << calcuSpatialLocality({{1, p1}, {x, 1-p1}}) << std::endl;
        }
    }

    return 0;
    
}

/*
 * 计算每字节的访问次数
 */
double calcuAccessPerByte(const unsigned long long& access, const unsigned long long& size) {
    return static_cast<double>(access) / size;
}

void test_adi_operator() {
    std::cout << "\n=== ADI算子访存特征分析 ===" << std::endl;
    
    // 读取ADI算子信息
    OperatorInfo adi;
    adi.getOperatorInfoFromCSV("adi", "data/adi/STANDARD_DATASET_adi.csv");
    
    // 遍历每个函数
    for (const auto& func : adi.functions) {
        std::cout << "\n[函数] " << func.name << std::endl;
        
        // 将变量按访存次数排序
        std::vector<VariableInfo> sorted_vars = func.variables;
        std::sort(sorted_vars.begin(), sorted_vars.end(), 
            [](const VariableInfo& a, const VariableInfo& b) {
                return a.access > b.access;
            });
        
        // 分析每个变量
        for (const auto& var : sorted_vars) {
            std::cout << "变量: " << var.name << std::endl;
            std::cout << "  访存次数: " << var.access << std::endl;
            
            // 打印访存模式
            std::cout << "  访存模式: ";
            for (const auto& p : var.patterns) {
                std::cout << "步长" << p.first << "(" << p.second*100 << "%) ";
            }
            std::cout << std::endl;
            
            // 计算空间局部性和访存密度
            double locality = calcuSpatialLocality(var.patterns);
            double accessPerByte = calcuAccessPerByte(var.access, var.size);
            
            std::cout << "  空间局部性: " << locality << std::endl;
            std::cout << "  每字节访问数: " << accessPerByte << std::endl;
        }
    }
}

void test_access_strategy_deduct() {
    std::cout << "\n=== ADI算子访存特征分析 ===" << std::endl;
    
    // 读取ADI算子信息
    OperatorInfo adi;
    adi.getOperatorInfoFromCSV("adi", "data/adi/STANDARD_DATASET_adi.csv");
    for (const auto& func : adi.functions) {    
        AccessStrategyDeducter adi_access_strategy_deduct;
        adi_access_strategy_deduct.deductAccessStrategy(func);
        adi_access_strategy_deduct.printAccessStrategy();
    }
}