# MASAMT - Memory Access Strategy Automatic Mapping Tool

*[English](README.md) | [中文](README_CN.md)*

## Project Overview
MASAMT (Memory Access Strategy Automatic Mapping Tool) is a C++ project designed to analyze and optimize memory access patterns. This tool identifies different memory access characteristics and automatically deduces the optimal cache access strategy, including BULK, SINGLE, and DIRECT strategies.

## Features
- Automatic analysis of memory access patterns and features
- Deduction of optimal cache strategies based on memory access behavior
- Support for multiple access strategies: BULK, SINGLE, DIRECT
- Handling of complex access patterns and stride analysis
- CSV and terminal output options

## Requirements
- C++11 compatible compiler
- Make build system

## Installation
1. Clone the repository
```bash
git clone https://github.com/yourusername/MASAMT.git
cd MASAMT
```

2. Build the project
```bash
make
```

The compiled executable will be located in the `bin` directory.

## Usage
```bash
./bin/masamt [options]
```

### Command Line Options
- `-h, --help`: Display help information
- `-c, --csv`: Output results to CSV files (default is terminal output)
- `-1, --oneline`: Use one-line output format
- `-n, --no-header`: Don't show headers in output
- `-o, --operator=NAME`: Process only the specified operator
- `-d, --dataset=NAME`: Process only the specified dataset

## Directory Structure
- `src/`: Source code files
- `include/`: Header files
- `bin/`: Compiled executables
- `data/`: Data files
- `results/`: Output results

## Core Components
- `AccessStrategyDeduct`: Memory access strategy deduction engine
- `OperatorInfo`: Operator information handling
- `CSVHandler`: CSV file handling
- `FileUtils`: File operation utilities

## Contributing
Contributions are welcome! To contribute, please follow these steps:
1. Fork the project
2. Create your feature branch
3. Commit your changes
4. Ensure all tests pass
5. Submit a Pull Request

## License
This project is licensed under the terms specified in the [LICENSE file](LICENSE). 