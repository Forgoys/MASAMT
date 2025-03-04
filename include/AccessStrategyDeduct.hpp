#pragma once

#include "OperatorInfo.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

enum AccessStrategy {
    BULK,
    SINGLE,
    DIRECT,
    UNSUITABLE
};

class AccessStrategyConfig {
public:
    AccessStrategy accessStrategy;
    int set;
    int line;
    AccessStrategyConfig(){};
    AccessStrategyConfig(AccessStrategy accessStrategy) : accessStrategy(accessStrategy){};
    AccessStrategyConfig(AccessStrategy accessStrategy, int set, int line) : accessStrategy(accessStrategy), set(set), line(line){};
    void setStrategy(AccessStrategy accessStrategy) {
        this->accessStrategy = accessStrategy;
    }
    void setParm(int set, int line) {
        this->set = set;
        this->line = line;
    }
    ~AccessStrategyConfig(){};
    std::string getStrategyName() const {
        switch (accessStrategy) {
            case BULK: return "CACHE_BULK";
            case SINGLE: return "CACHE_SINGLE";
            case DIRECT: return "CACHE_DIRECT";
            case UNSUITABLE: return "CACHE_UNSUITABLE";
        }
    }
    int getSpaceUsage() const {
        return (1 << set) * (1 << line);
    }
    void printInfo() const {
        std::cout << "访问模式: " << getStrategyName() << ", set: " << set << ", line: " << line << ", 空间占用: " << getSpaceUsage() << "字节" << std::endl;
    }
};

class AccessFeatureVector
{
public:
    std::string varName;
    unsigned long long size;
    unsigned long long access;
    std::vector<std::pair<int, double>> patterns;
    double accessEntropy;
    double spatialLocality;
    double accessDensity;
    AccessStrategyConfig accessStrategyConfig;
    AccessFeatureVector(){};
    AccessFeatureVector(const VariableInfo &var);
    ~AccessFeatureVector(){};
    void printInfo() const;
private:
    void calculateAccessEntropy();
    void calculateSpatialLocality();
    void calculateAccessDensity();
};

// 根据函数信息，提取访存特征，并推断缓存策略
class AccessStrategyDeducter
{
public:
    std::string funcName;
    std::vector<AccessFeatureVector> accessFeatureVectors;
    AccessStrategyDeducter(){};
    ~AccessStrategyDeducter(){};
    void deductAccessStrategy(const FunctionInfo &func);
    void printAccessStrategy() const;
};
