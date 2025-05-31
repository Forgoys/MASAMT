# MASAMT - 面向MT-3000的内存访问策略分析器

*[English](README.md) | [中文](README_CN.md)*

## 项目简介
MASAMT (面向MT-3000的内存访问策略分析器) 是一个C++工具，专门用于分析内存访问模式并自动推导MT-3000平台的最佳缓存访问策略。该工具支持BULK、SINGLE和DIRECT等多种访问策略，特别适用于优化各种基准测试程序和工作负载。

## 核心特性
- **通用CSV支持**：既支持结构化的PolyBench数据集，也支持任意基准测试套件的CSV文件
- **自动策略推导**：分析内存访问模式并推荐最优缓存策略
- **多格式兼容**：处理传统PolyBench命名约定和通用CSV格式
- **基准测试优化**：完美适用于分析各种测试程序的不同工作负载
- **灵活输出选项**：支持终端和CSV输出格式

## 基准测试程序兼容性

### PolyBench集成（传统格式）
最初专为PolyBench程序设计，MASAMT无缝处理结构化数据集命名：
- **文件格式**：`{DATASET}_DATASET_{program}.csv`（如：`STANDARD_DATASET_adi.csv`）
- **数据集规模**：MINI、SMALL、STANDARD、LARGE、EXTRALARGE
- **输出特点**：结果中包含数据集规模分类信息

### 通用基准测试支持（通用格式）
现已扩展支持任何基准测试程序或自定义工作负载：
- **文件格式**：任意CSV文件名（如：`matrix_multiply.csv`、`fft_analysis.csv`）
- **应用场景**：SPEC基准测试、自定义算法、应用程序性能分析数据
- **输出特点**：精简结果，不包含数据集特定列

## 应用领域

### 性能分析场景
- **跨平台基准测试**：比较不同基准测试套件的内存策略
- **工作负载特征分析**：分析各种计算负载的内存访问模式
- **缓存优化**：为特定算法确定最优缓存配置
- **系统调优**：基于访问模式分析指导硬件配置决策

### 支持的使用场景
- PolyBench内核优化
- SPEC CPU基准测试分析
- 自定义算法性能分析
- 应用程序特定的内存模式研究
- 不同工作负载下缓存行为研究

## 系统要求
- C++11兼容的编译器
- Make构建系统
- 包含内存访问模式数据的CSV输入文件

## 安装
1. 克隆仓库
```bash
git clone https://github.com/yourusername/MASAMT.git
cd MASAMT
```

2. 编译项目
```bash
make
```

编译后的可执行文件将位于`bin`目录中。

## 使用方法

### 处理单个文件
```bash
# 处理任意CSV文件（自动检测格式）
./bin/masamt -f path/to/benchmark_data.csv -c

# 处理PolyBench传统格式
./bin/masamt -f data/adi/STANDARD_DATASET_adi.csv -c

# 处理通用基准测试数据
./bin/masamt -f spec_results.csv -c
```

### 批量处理
```bash
# 处理所有可用数据（传统格式 + 通用CSV文件）
./bin/masamt -c

# 按特定程序过滤（PolyBench）
./bin/masamt -o adi -c

# 按数据集规模过滤（仅PolyBench）
./bin/masamt -d STANDARD -c
```

### 输出选项
```bash
# 终端输出（默认）
./bin/masamt -f benchmark.csv

# CSV输出便于进一步分析
./bin/masamt -f benchmark.csv -c

# 紧凑的单行格式
./bin/masamt -f benchmark.csv -1
```

## 命令行选项
- `-h, --help`：显示帮助信息
- `-c, --csv`：将结果输出到CSV文件（默认为终端输出）
- `-1, --oneline`：使用单行输出格式
- `-n, --no-header`：不显示输出的表头
- `-o, --operator=NAME`：仅处理指定的程序/算子
- `-d, --dataset=NAME`：仅处理指定的数据集（仅PolyBench）
- `-f, --file=PATH`：处理指定的CSV文件

## 输入CSV格式
工具期望CSV文件包含以下列：
```
变量名, 函数名, 内存大小, 访问次数, 步长1, 占比1, 步长2, 占比2, ...
```

## 输出分析
工具生成包含以下内容的综合报告：
- **内存访问密度**：每字节访问频率
- **空间局部性**：基于模式的局部性测量
- **策略推荐**：BULK、SINGLE或DIRECT缓存策略
- **配置参数**：优化的set和line参数

## 目录结构
- `src/`：源代码文件
- `include/`：头文件
- `bin/`：编译后的可执行文件
- `data/`：PolyBench数据文件（传统格式）
- `results/`：生成的分析结果

## 核心组件
- `AccessStrategyDeduct`：内存访问策略推导引擎
- `OperatorInfo`：程序/算子信息处理
- `CSVHandler`：支持双格式的通用CSV文件处理
- `FileUtils`：文件操作和格式检测工具

## 示例工作流

### 分析PolyBench程序
```bash
# 构建工具
make

# 分析所有PolyBench内核
./bin/masamt -c

# 专注于特定内核和数据集
./bin/masamt -o adi -d STANDARD -c
```

### 分析自定义基准测试
```bash
# 处理您的基准测试数据
./bin/masamt -f my_benchmark_results.csv -c

# 比较多个基准测试运行
./bin/masamt -f run1.csv -c
./bin/masamt -f run2.csv -c
```

## 实际应用案例

### 科研场景
- **算法优化研究**：分析不同算法的内存访问特征
- **系统性能评估**：评估硬件配置对不同工作负载的影响
- **缓存策略比较**：比较不同缓存策略在实际应用中的效果

### 工业应用
- **产品性能调优**：为实际产品选择最优内存配置
- **基准测试分析**：系统化分析标准基准测试结果
- **硬件设计指导**：为新硬件设计提供内存子系统优化建议

## 贡献指南
欢迎贡献！该工具设计为可扩展的，支持新的基准测试格式和分析方法。贡献步骤：

1. Fork项目
2. 创建您的特性分支（`git checkout -b feature/new-benchmark-support`）
3. 提交您的更改（`git commit -am 'Add support for new benchmark format'`）
4. 推送到分支（`git push origin feature/new-benchmark-support`）
5. 创建Pull Request

## 许可证
本项目采用[LICENSE文件](LICENSE)中指定的许可证条款。

## 引用
如果您在研究中使用MASAMT，请引用：
```
MASAMT: Memory Access Strategy Analyzer for MT-3000
[您的发表详情]
```