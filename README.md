# MASAMT - Memory Access Strategy Analyzer for MT-3000

*[English](README.md) | [中文](README_CN.md)*

## Project Overview
MASAMT (Memory Access Strategy Analyzer for MT-3000) is a C++ tool designed to analyze memory access patterns and automatically deduce optimal cache access strategies for the MT-3000 platform. The tool supports multiple access strategies including BULK, SINGLE, and DIRECT, making it ideal for optimizing various benchmark programs and workloads.

## Key Features
- **Universal CSV Support**: Processes both structured PolyBench datasets and arbitrary CSV files from any benchmark suite
- **Automatic Strategy Deduction**: Analyzes memory access patterns and recommends optimal cache strategies
- **Multi-Format Compatibility**: Handles legacy PolyBench naming conventions and generic CSV formats
- **Benchmark Optimization**: Perfect for analyzing different workloads across various test programs
- **Flexible Output Options**: Supports both terminal and CSV output formats

## Benchmark Program Compatibility

### PolyBench Integration (Legacy Format)
Originally designed for PolyBench programs, MASAMT seamlessly handles the structured dataset naming:
- **File Format**: `{DATASET}_DATASET_{program}.csv` (e.g., `STANDARD_DATASET_adi.csv`)
- **Dataset Sizes**: MINI, SMALL, STANDARD, LARGE, EXTRALARGE
- **Output**: Includes dataset size classification in results

### Universal Benchmark Support (Generic Format)
Now extended to support any benchmark program or custom workload:
- **File Format**: Any CSV filename (e.g., `matrix_multiply.csv`, `fft_analysis.csv`)
- **Use Cases**: SPEC benchmarks, custom algorithms, application profiling data
- **Output**: Streamlined results without dataset-specific columns

## Applications

### Performance Analysis Scenarios
- **Cross-Platform Benchmarking**: Compare memory strategies across different benchmark suites
- **Workload Characterization**: Analyze memory access patterns for various computational loads
- **Cache Optimization**: Determine optimal cache configurations for specific algorithms
- **System Tuning**: Guide hardware configuration decisions based on access pattern analysis

### Supported Use Cases
- PolyBench kernel optimization
- SPEC CPU benchmark analysis
- Custom algorithm profiling
- Application-specific memory pattern studies
- Research on cache behavior across different workloads

## Requirements
- C++11 compatible compiler
- Make build system
- CSV input files with memory access pattern data

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

### Processing Individual Files
```bash
# Process any CSV file (auto-detects format)
./bin/masamt -f path/to/benchmark_data.csv -c

# Process PolyBench legacy format
./bin/masamt -f data/adi/STANDARD_DATASET_adi.csv -c

# Process generic benchmark data
./bin/masamt -f spec_results.csv -c
```

### Batch Processing
```bash
# Process all available data (legacy + generic CSV files)
./bin/masamt -c

# Filter by specific program (PolyBench)
./bin/masamt -o adi -c

# Filter by dataset size (PolyBench)
./bin/masamt -d STANDARD -c
```

### Output Options
```bash
# Terminal output (default)
./bin/masamt -f benchmark.csv

# CSV output for further analysis
./bin/masamt -f benchmark.csv -c

# Compact one-line format
./bin/masamt -f benchmark.csv -1
```

## Command Line Options
- `-h, --help`: Display help information
- `-c, --csv`: Output results to CSV files (default: terminal output)
- `-1, --oneline`: Use one-line output format
- `-n, --no-header`: Don't show headers in output
- `-o, --operator=NAME`: Process only the specified program/operator
- `-d, --dataset=NAME`: Process only the specified dataset (PolyBench only)
- `-f, --file=PATH`: Process a specific CSV file

## Input CSV Format
The tool expects CSV files with the following columns:
```
Variable_Name, Function_Name, Memory_Size, Access_Count, Stride_1, Percentage_1, Stride_2, Percentage_2, ...
```

## Output Analysis
The tool generates comprehensive reports including:
- **Memory Access Density**: Access frequency per byte
- **Spatial Locality**: Pattern-based locality measurement  
- **Strategy Recommendation**: BULK, SINGLE, or DIRECT cache strategy
- **Configuration Parameters**: Optimized set and line parameters

## Directory Structure
- `src/`: Source code files
- `include/`: Header files
- `bin/`: Compiled executables
- `data/`: PolyBench data files (legacy format)
- `results/`: Generated analysis results

## Core Components
- `AccessStrategyDeduct`: Memory access strategy deduction engine
- `OperatorInfo`: Program/operator information handling
- `CSVHandler`: Universal CSV file processing with dual-format support
- `FileUtils`: File operations and format detection utilities

## Example Workflow

### Analyzing PolyBench Programs
```bash
# Build the tool
make

# Analyze all PolyBench kernels
./bin/masamt -c

# Focus on specific kernel and dataset
./bin/masamt -o adi -d STANDARD -c
```

### Analyzing Custom Benchmarks
```bash
# Process your benchmark data
./bin/masamt -f my_benchmark_results.csv -c

# Compare multiple benchmark runs
./bin/masamt -f run1.csv -c
./bin/masamt -f run2.csv -c
```

## Contributing
Contributions are welcome! This tool is designed to be extensible for new benchmark formats and analysis methods. To contribute:

1. Fork the project
2. Create your feature branch (`git checkout -b feature/new-benchmark-support`)
3. Commit your changes (`git commit -am 'Add support for new benchmark format'`)
4. Push to the branch (`git push origin feature/new-benchmark-support`)
5. Create a Pull Request

## License
This project is licensed under the terms specified in the [LICENSE file](LICENSE).

## Citation
If you use MASAMT in your research, please cite:
```
MASAMT: Memory Access Strategy Analyzer for MT-3000
[Your publication details here]
```