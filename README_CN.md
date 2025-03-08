# MASAMT - 面向MT-3000的内存访问策略分析器

*[English](README.md) | [中文](README_CN.md)*

## 项目简介
MASAMT (面向MT-3000的内存访问策略分析器) 是一个C++项目，用于分析和优化MT-3000平台上的内存访问模式。该工具可以识别不同的访存特征，自动推断最佳的缓存访问策略，包括BULK、SINGLE和DIRECT等不同策略。

## 功能特性
- 自动分析访存模式和特征
- 基于访存行为数据推导最佳缓存策略
- 支持多种访问策略：BULK、SINGLE、DIRECT
- 可以处理复杂的访存模式和步长分析
- 提供CSV输出和终端输出多种格式

## 系统要求
- C++11兼容的编译器
- Make构建系统

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
```bash
./bin/masamt [选项]
```

### 命令行选项
- `-h, --help`：显示帮助信息
- `-c, --csv`：将结果输出到CSV文件（默认为终端输出）
- `-1, --oneline`：使用单行输出格式
- `-n, --no-header`：不显示输出的表头
- `-o, --operator=NAME`：仅处理指定的操作符
- `-d, --dataset=NAME`：仅处理指定的数据集

## 目录结构
- `src/`：源代码文件
- `include/`：头文件
- `bin/`：编译后的可执行文件
- `data/`：数据文件
- `results/`：输出结果

## 核心组件
- `AccessStrategyDeduct`：访存策略推导引擎
- `OperatorInfo`：操作符信息处理
- `CSVHandler`：CSV文件处理
- `FileUtils`：文件操作工具

## 贡献指南
欢迎提交问题报告和改进建议。如需贡献代码，请遵循以下步骤：
1. Fork项目
2. 创建您的特性分支
3. 提交您的更改
4. 确保通过所有测试
5. 提交Pull Request

## 许可证
本项目采用[LICENSE文件](LICENSE)中的许可证。 