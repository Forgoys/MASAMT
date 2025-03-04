#include "AccessStrategyDeduct.hpp"
#define SM_SPACE_SIZE 60 * 1024
AccessFeatureVector::AccessFeatureVector(const VariableInfo &var)
{
    this->varName = var.name;
    this->size = var.size;
    this->access = var.access;
    this->patterns = var.patterns;
    calculateAccessEntropy();
    calculateSpatialLocality();
    calculateAccessDensity();
}

void AccessFeatureVector::calculateAccessEntropy()
{
    // 访存熵用于衡量访存步长分布的不确定性
    // 这里使用步长的占比作为概率来计算熵
    // 例如:如果所有访存都使用同一个步长(占比为1),熵为0,表示步长分布非常集中
    // 如果访存步长的占比分布均匀(比如有4个步长,每个占比0.25),熵最大,表示步长分布分散
    // 熵的计算公式: H = -Σ(p_i * log2(p_i)), 其中p_i是每个步长的占比
    if (patterns.empty()) {
        this->accessEntropy = 0.0;
        return;
    }
    double entropy = 0.0;
    for (const auto &pattern : patterns) {
        entropy += -1 * pattern.second * log2(pattern.second);
    }
    this->accessEntropy = entropy;
}

void AccessFeatureVector::calculateSpatialLocality()
{
    if (patterns.empty()) {
        this->spatialLocality = 0.0;
        return;
    }
    double locality = 0.0;
    for (const auto &pattern : patterns) {
        locality += pattern.second * exp(-pattern.first);
    }
    this->spatialLocality = locality;
}

void AccessFeatureVector::calculateAccessDensity() { this->accessDensity = static_cast<double>(access) / size; }

void AccessFeatureVector::printInfo() const
{
    std::cout << varName << ": 大小" << size << "字节, 访存" << access << "次, 密度" << accessDensity << ", 模式:";
    for (const auto &pattern : patterns) {
        std::cout << "步长" << pattern.first << "(" << pattern.second * 100 << "%) ";
    }
    std::cout << ", 局部性" << spatialLocality;
    std::cout << ", 策略: " << accessStrategyConfig.getStrategyName();
    if (accessStrategyConfig.accessStrategy != UNSUITABLE) {
        std::cout << ", set=" << accessStrategyConfig.set << ", line=" << accessStrategyConfig.line;
    }
    std::cout << std::endl;
}

void AccessStrategyDeducter::deductAccessStrategy(const FunctionInfo &func)
{
    this->funcName = func.name;
    for (const auto &var : func.variables) {
        AccessFeatureVector featureVector(var);
        accessFeatureVectors.push_back(featureVector);
    }
    // 先按访存次数降序排序,访存次数相同时按密度降序排序，密度相同的按空间局部性降序排序
    std::sort(accessFeatureVectors.begin(), accessFeatureVectors.end(),
              [](const AccessFeatureVector &a, const AccessFeatureVector &b) {
                  if (a.access != b.access) {
                      return a.access > b.access;
                  }
                  if (a.accessDensity != b.accessDensity) {
                      return a.accessDensity > b.accessDensity;
                  }
                  return a.spatialLocality > b.spatialLocality;
              });

    for (auto &featureVector : accessFeatureVectors) {
        if (featureVector.size < SM_SPACE_SIZE) {
            featureVector.accessStrategyConfig.setStrategy(AccessStrategy::BULK);
            featureVector.accessStrategyConfig.setParm(0, static_cast<int>(std::ceil(std::log2(featureVector.size))));
            continue;
        }
        if (featureVector.spatialLocality > 0.1) {
            // 检查是否存在步长为1的访问模式
            bool hasStride1 = false;
            for (const auto &pattern : featureVector.patterns) {
                if (pattern.first == 1) {
                    hasStride1 = true;
                    break;
                }
            }
            if (hasStride1) {
                featureVector.accessStrategyConfig.setStrategy(AccessStrategy::SINGLE);
                featureVector.accessStrategyConfig.setParm(1, 1);
            } else {
                featureVector.accessStrategyConfig.setStrategy(AccessStrategy::DIRECT);
                featureVector.accessStrategyConfig.setParm(1, 1);
            }
        } else {
            featureVector.accessStrategyConfig.setStrategy(AccessStrategy::UNSUITABLE);
            continue;
        }
    }
}

void AccessStrategyDeducter::printAccessStrategy() const
{
    std::cout << "函数: " << funcName << std::endl;
    for (const auto &featureVector : accessFeatureVectors) {
        featureVector.printInfo();
    }
}
