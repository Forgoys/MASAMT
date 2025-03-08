#pragma once

#include <string>
#include <utility>
#include <vector>
#define SM_SPACE_SIZE 60 * 1024

class VariableInfo
{
public:
    VariableInfo() {};
    VariableInfo(const std::string &name, const unsigned long long size, const unsigned long long access,
                 const std::vector<std::pair<int, double>> &patterns);
    std::string name;
    unsigned long long size;
    unsigned long long access;
    std::vector<std::pair<int, double>> patterns;
};

class FunctionInfo
{
public:
    FunctionInfo() {};
    FunctionInfo(const std::string &name, const std::vector<VariableInfo> &variables);
    std::string name;
    std::vector<VariableInfo> variables;
};

class OperatorInfo
{
public:
    OperatorInfo() {};
    OperatorInfo(const std::string &name, const std::vector<FunctionInfo> &functions);
    std::string name;
    std::vector<FunctionInfo> functions;

    void getOperatorInfoFromCSV(const std::string &opName, const std::string &csvPath);
    void printInfo() const;
};
