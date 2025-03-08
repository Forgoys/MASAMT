#pragma once

#include "OperatorInfo.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

enum AccessStrategy
{
    BULK,
    SINGLE,
    DIRECT,
    UNSUITABLE
};

class AccessStrategyConfig
{
public:
    AccessStrategy accessStrategy;
    int set = 0;
    int line = 0;
    int spaceUsage = 0;
    AccessStrategyConfig() {};
    AccessStrategyConfig(AccessStrategy accessStrategy) : accessStrategy(accessStrategy) {};
    AccessStrategyConfig(AccessStrategy accessStrategy, int set, int line)
        : accessStrategy(accessStrategy), set(set), line(line) {};
    AccessStrategyConfig(const AccessStrategyConfig &other)
        : accessStrategy(other.accessStrategy), set(other.set), line(other.line), spaceUsage(other.spaceUsage) {};
    void setStrategy(AccessStrategy accessStrategy) { this->accessStrategy = accessStrategy; }
    void setParm(int set, int line)
    {
        this->set = set;
        this->line = line;
    }
    ~AccessStrategyConfig() {};
    std::string getStrategyName() const
    {
        switch (accessStrategy) {
        case BULK:
            return "CACHE_BULK";
        case SINGLE:
            return "CACHE_SINGLE";
        case DIRECT:
            return "CACHE_DIRECT";
        case UNSUITABLE:
            return "CACHE_UNSUITABLE";
        default:
            return "UNKNOWN";
        }
    }
    int getSpaceUsage() const { return ((set == 0) ? 0 : 1 << set) + ((line == 0) ? 0 : 1 << line); }
    void printInfo() const
    {
        std::cout << "访问模式: " << getStrategyName() << ", set: " << set << ", line: " << line
                  << ", 空间占用: " << getSpaceUsage() << "字节" << std::endl;
    }
    static int getLineBasedOnSize(const int &S) { return static_cast<int>(std::ceil(std::log2(S))); }
    static int calculateLineSpace(const int &line) { return (line == 0) ? 0 : 1 << line; }
};

class AccessFeatureVector
{
public:
    std::string varName;
    // 访存空间大小
    unsigned long long S;
    // 访存次数
    unsigned long long N;
    // 访存模式 <步长d_i, 占比p_i>
    std::vector<std::pair<int, double>> patterns;
    // 空间局部性
    double L;
    // 访存密度
    double D;
    // 空间划分因子
    double F;
    // 划分后的SM空间大小（字节）
    int C;
    // 缓存策略配置
    AccessStrategyConfig accessStrategyConfig;
    AccessFeatureVector() {};
    AccessFeatureVector(const VariableInfo &var);
    AccessFeatureVector(const AccessFeatureVector &other)
        : varName(other.varName), S(other.S), N(other.N), patterns(other.patterns), L(other.L), D(other.D), F(other.F), C(other.C), accessStrategyConfig(other.accessStrategyConfig) {};
    ~AccessFeatureVector() {};
    bool operator==(const AccessFeatureVector &other) const
    {
        return varName == other.varName;
    }
    void printInfo() const;
    // Concise one-line output
    void printOneLine() const;

private:
    void calculateL();
    void calculateD();
    void calculateF();
};

// 根据函数信息，提取访存特征，并推断缓存策略
class AccessStrategyDeducter
{
public:
    int C_total = 60 * 1024;
    std::string funcName;
    std::vector<AccessFeatureVector> accessFeatureVectors;
    int spaceUsage = 0;
    constexpr const static double strategy_determine_factor = 0.14;

    AccessStrategyDeducter() {};
    ~AccessStrategyDeducter() {};
    // 按照空间划分因子计算各个变量的SM空间大小
    void calculateC();
    // 按照空间划分因子计算各个变量的SM空间大小
    static void calculateC(std::vector<AccessFeatureVector> &accessFeatureVectors, int C_total);
    // 推断缓存策略
    static void determineStrategy(std::vector<AccessFeatureVector> &accessFeatureVectors);
    // 确定参数
    static void determineParameters(std::vector<AccessFeatureVector> &accessFeatureVectors);
    // 决策模型，推断函数中各个变量的缓存策略和缓存策略参数
    void deductAccessStrategy(const FunctionInfo &func);
    // 打印缓存策略
    void printAccessStrategy() const;
    // 打印单行策略
    void printAccessStrategyOneLine() const;
    // 添加空间占用
    void addSpaceUsage(const int &spaceUsage) { this->spaceUsage += spaceUsage; }
    // 移除空间占用
    void removeSpaceUsage(const int &spaceUsage) { this->spaceUsage -= spaceUsage; }
    // 检查空间占用是否超过SM空间大小
    bool checkSpaceUsage() const { return this->spaceUsage > SM_SPACE_SIZE; }
};