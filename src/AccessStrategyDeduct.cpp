#include "AccessStrategyDeduct.hpp"

AccessFeatureVector::AccessFeatureVector(const VariableInfo &var)
{
    this->varName = var.name;
    this->S = var.size;
    this->N = var.access;
    this->patterns = var.patterns;
    calculateL();
    calculateD();
    calculateF();
}

void AccessFeatureVector::calculateL()
{
    if (patterns.empty()) {
        this->L = 0.0;
        return;
    }
    double locality = 0.0;
    for (const auto &pattern : patterns) {
        locality += pattern.second * exp(-pattern.first);
    }
    this->L = locality;
}

void AccessFeatureVector::calculateD() { this->D = static_cast<double>(N) / S; }

void AccessFeatureVector::calculateF() { this->F = L * D; }

void AccessFeatureVector::printInfo() const
{
    // 根据新的列顺序输出信息
    std::cout << varName 
              << ": 预分配=" << C 
              << ", 大小=" << S 
              << ", 访存=" << N << "次"
              << ", 模式:";
    
    for (const auto &pattern : patterns) {
        std::cout << " 步长" << pattern.first << "(" << std::fixed << std::setprecision(1) << pattern.second * 100 << "%)";
    }
    
    std::cout << ", 密度=" << std::fixed << std::setprecision(2) << D 
              << ", 局部性=" << std::fixed << std::setprecision(4) << L 
              << ", 策略=" << accessStrategyConfig.getStrategyName()
              << ", line=" << accessStrategyConfig.line
              << ", set=" << accessStrategyConfig.set
              << std::endl;
}

void AccessFeatureVector::printOneLine() const
{
    // 根据新的列顺序输出简洁的一行信息
    std::cout << varName << " C=" << C 
              << " S=" << S << " N=" << N 
              << " D=" << std::fixed << std::setprecision(2) << D 
              << " L=" << std::fixed << std::setprecision(4) << L
              << " " << accessStrategyConfig.getStrategyName() 
              << " line=" << accessStrategyConfig.line
              << " set=" << accessStrategyConfig.set;
}

void AccessStrategyDeducter::calculateC()
{
    // 归一化各个变量的空间划分因子，然后计算划分后的SM空间大小
    double sumF = 0.0;
    for (const auto &featureVector : accessFeatureVectors) {
        sumF += featureVector.F;
    }
    for (auto &featureVector : accessFeatureVectors) {
        featureVector.C = static_cast<int>(featureVector.F / sumF * AccessStrategyDeducter::C_total);
    }
}

void AccessStrategyDeducter::calculateC(std::vector<AccessFeatureVector> &accessFeatureVectors, int C_total)
{
    // 归一化各个变量的空间划分因子，然后计算划分后的SM空间大小
    double sumF = 0.0;
    for (const auto &featureVector : accessFeatureVectors) {
        sumF += featureVector.F;
    }
    for (auto &featureVector : accessFeatureVectors) {
        featureVector.C = static_cast<int>(featureVector.F / sumF * C_total);
    }
}

void AccessStrategyDeducter::determineStrategy(std::vector<AccessFeatureVector> &accessFeatureVectors)
{
    for (auto &featureVector : accessFeatureVectors) {
        // 如果划分后的SM空间大小为0，则使用UNSUITABLE策略
        if (featureVector.C == 0) {
            featureVector.accessStrategyConfig.setStrategy(AccessStrategy::UNSUITABLE);
            continue;
        }
        // 如果划分后的SM空间大小大于访存空间大小，则使用BULK策略
        if (featureVector.C >= featureVector.S) {
            featureVector.accessStrategyConfig.setStrategy(AccessStrategy::BULK);
            featureVector.accessStrategyConfig.setParm(0, featureVector.S);
            continue;
        }
        // 如果划分后的SM空间大小小于访存空间大小，则使用SINGLE/DIRECT策略
        if (featureVector.C < featureVector.S) {
            // 如果空间局部性大于策略决断常量，则使用SINGLE策略
            if (featureVector.L > AccessStrategyDeducter::strategy_determine_factor) {
                featureVector.accessStrategyConfig.setStrategy(AccessStrategy::SINGLE);
            } else { // 如果空间局部性小于策略决断常量，则使用DIRECT策略
                featureVector.accessStrategyConfig.setStrategy(AccessStrategy::DIRECT);
            }
        }
    }
}

void AccessStrategyDeducter::determineParameters(std::vector<AccessFeatureVector> &accessFeatureVectors)
{
    for (auto &featureVector : accessFeatureVectors) {
        if (featureVector.accessStrategyConfig.accessStrategy == AccessStrategy::UNSUITABLE) {
            featureVector.accessStrategyConfig.setParm(0, 0);
        } else if (featureVector.accessStrategyConfig.accessStrategy == AccessStrategy::BULK) {
            featureVector.accessStrategyConfig.setParm(0, featureVector.S);
        } else if (featureVector.accessStrategyConfig.accessStrategy == AccessStrategy::SINGLE) {
            // line = floor(log2(C))
            featureVector.accessStrategyConfig.setParm(0, floor(log2(featureVector.C)));
        } else if (featureVector.accessStrategyConfig.accessStrategy == AccessStrategy::DIRECT) {
            // 找出patterns中的最大步长
            int maxStride = 1;
            for (const auto &pattern : featureVector.patterns) {
                maxStride = std::max(maxStride, static_cast<int>(pattern.first));
            }
            
            // 找到最接近maxStride的2的幂的指数
            int line = static_cast<int>(round(log2(maxStride)));
            
            // 限制line的范围，确保set*2^line <= C
            line = std::max(4, std::min(line, static_cast<int>(floor(log2(featureVector.C)))));
            
            int set = std::max(1, static_cast<int>(floor(featureVector.C / (1 << line))));
            set = static_cast<int>(floor(log2(set)));
            featureVector.accessStrategyConfig.setParm(set, line);
        }
    }
}

void AccessStrategyDeducter::deductAccessStrategy(const FunctionInfo &func)
{
    /*-----------------------初始化--------------------------------*/
    this->funcName = func.name;
    accessFeatureVectors.clear(); // 清空最终结果向量

    // 初始SM可用空间
    int C_total = AccessStrategyDeducter::C_total;

    // 将所有变量添加到临时向量
    std::vector<AccessFeatureVector> accessFeatureVectors_tmp;
    for (const auto &var : func.variables) {
        AccessFeatureVector featureVector(var);
        accessFeatureVectors_tmp.push_back(featureVector);
    }

    /*-----------------------循环决策过程--------------------------------*/
    bool hasChanges = true;
    // int loop_count = 0;
    while (hasChanges && !accessFeatureVectors_tmp.empty()) {
        // loop_count++;
        // std::cout << "第" << loop_count << "次循环" << std::endl;
        // 划分空间
        calculateC(accessFeatureVectors_tmp, C_total);

        // 决策策略
        determineStrategy(accessFeatureVectors_tmp);

        // 创建新向量存储非BULK和非UNSUITABLE的变量
        std::vector<AccessFeatureVector> remaining;
        hasChanges = false; // 重置变化标志

        for (auto &featureVector : accessFeatureVectors_tmp) {
            if (featureVector.accessStrategyConfig.accessStrategy == AccessStrategy::BULK) {
                // 更新可用空间 - BULK只需要实际访存大小的空间
                C_total -= featureVector.S;
                // 添加到最终结果
                accessFeatureVectors.push_back(featureVector);
                hasChanges = true; // 标记有变化
            } else if (featureVector.accessStrategyConfig.accessStrategy == AccessStrategy::UNSUITABLE) {
                // 添加到最终结果
                accessFeatureVectors.push_back(featureVector);
                hasChanges = true; // 标记有变化
            } else {
                // 保留非BULK和非UNSUITABLE的变量进入下一轮
                remaining.push_back(featureVector);
            }
        }

        // 更新临时向量
        accessFeatureVectors_tmp = remaining;
        // std::cout << "剩余变量数量: " << accessFeatureVectors_tmp.size() << std::endl;
        // output C_total
        // std::cout << "C_total: " << C_total << std::endl;
    }

    /*-----------------------处理剩余变量--------------------------------*/
    // 对剩余变量进行最终决策
    if (!accessFeatureVectors_tmp.empty()) {
        calculateC(accessFeatureVectors_tmp, C_total);
        determineStrategy(accessFeatureVectors_tmp);

        // 将剩余变量添加到最终结果
        for (auto &featureVector : accessFeatureVectors_tmp) {
            accessFeatureVectors.push_back(featureVector);
        }
    }

    /*-----------------------确定参数--------------------------------*/
    // 根据需要调用参数确定函数
    determineParameters(accessFeatureVectors);
}

void AccessStrategyDeducter::printAccessStrategy() const
{
    std::cout << "函数: " << funcName << std::endl;
    for (const auto &featureVector : accessFeatureVectors) {
        featureVector.printInfo();
    }
}

void AccessStrategyDeducter::printAccessStrategyOneLine() const
{
    std::cout << "函数: " << funcName << " | ";
    bool isFirst = true;
    for (const auto &featureVector : accessFeatureVectors) {
        if (!isFirst) {
            std::cout << " | ";
        }
        featureVector.printOneLine();
        isFirst = false;
    }
    std::cout << std::endl;
}