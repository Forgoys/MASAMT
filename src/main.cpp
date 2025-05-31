#include "OperatorInfo.hpp"
#include "CSVHandler.hpp"
#include "FileUtils.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <getopt.h>
#include "test.hpp"

// Command line options
struct CLIOptions {
    bool toCSV = false;           // Output to CSV instead of terminal
    bool oneLineOutput = false;   // Use one line per variable output format
    bool showHeader = true;       // Show header in output
    std::string opFilter = "";    // Filter by operator name
    std::string datasetFilter = ""; // Filter by dataset name
    std::string csvPath = "";     // Process a specific CSV file
};

// Print help message
void printHelp(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n"
              << "Options:\n"
              << "  -h, --help                 Show this help message and exit\n"
              << "  -c, --csv                  Output to CSV files (default: terminal output)\n"
              << "  -1, --oneline              Use one-line output format\n"
              << "  -n, --no-header            Don't show headers in output\n"
              << "  -o, --operator=NAME        Process only the specified operator\n"
              << "  -d, --dataset=NAME         Process only the specified dataset\n"
              << "  -f, --file=PATH            Process a specific CSV file\n"
              << std::endl;
}

// Parse command line arguments
CLIOptions parseArgs(int argc, char* argv[]) {
    CLIOptions options;
    
    static struct option long_options[] = {
        {"help",      no_argument,       0, 'h'},
        {"csv",       no_argument,       0, 'c'},
        {"oneline",   no_argument,       0, '1'},
        {"no-header", no_argument,       0, 'n'},
        {"operator",  required_argument, 0, 'o'},
        {"dataset",   required_argument, 0, 'd'},
        {"file",      required_argument, 0, 'f'},
        {0,           0,                 0,  0 }
    };

    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "hc1no:d:f:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                printHelp(argv[0]);
                exit(0);
                break;
            case 'c':
                options.toCSV = true;
                break;
            case '1':
                options.oneLineOutput = true;
                break;
            case 'n':
                options.showHeader = false;
                break;
            case 'o':
                options.opFilter = optarg;
                break;
            case 'd':
                options.datasetFilter = optarg;
                break;
            case 'f':
                options.csvPath = optarg;
                break;
            case '?':
                printHelp(argv[0]);
                exit(1);
                break;
        }
    }
    
    return options;
}

// Print feature vector in one line
void printFeatureVectorOneLine(const AccessFeatureVector& featureVector) {
    std::cout << featureVector.varName << " [" 
              << featureVector.accessStrategyConfig.getStrategyName() << "] "
              << "Size:" << featureVector.S << "B Acc:" << featureVector.N 
              << " Density:" << std::fixed << std::setprecision(2) << featureVector.D
              << " Locality:" << std::fixed << std::setprecision(4) << featureVector.L;
    
    if (featureVector.accessStrategyConfig.accessStrategy != UNSUITABLE) {
        std::cout << " (set=" << featureVector.accessStrategyConfig.set 
                  << ",line=" << featureVector.accessStrategyConfig.line << ")";
    }
    std::cout << std::endl;
}

// Process a single CSV file
void processCSVFile(const std::string& csvPath, const CLIOptions& options) {
    CSVHandler& csvHandler = CSVHandler::getInstance();
    
    // Extract filename from path
    std::string filename = csvPath.substr(csvPath.find_last_of("/\\") + 1);
    
    // Check if it's legacy format
    std::string extractedDataset, extractedOpName;
    bool isLegacy = FileUtils::isLegacyCSVFormat(filename, extractedDataset, extractedOpName);
    
    std::string opName;
    std::string dataset;
    
    if (isLegacy) {
        // Legacy format: use extracted information
        opName = extractedOpName;
        dataset = extractedDataset;
        
        // Check filters
        if (!options.opFilter.empty() && options.opFilter != opName) return;
        if (!options.datasetFilter.empty() && options.datasetFilter != dataset) return;
        
        if (!options.oneLineOutput && !options.toCSV) {
            std::cout << "Processing legacy format file: " << filename << std::endl;
            std::cout << "Extracted operator: " << opName << ", dataset: " << dataset << std::endl;
        }
    } else {
        // Generic format: use filename as operator name
        opName = FileUtils::getFileNameWithoutExtension(csvPath);
        dataset = "UNKNOWN";
        
        // Check filters (only operator filter applies for generic format)
        if (!options.opFilter.empty() && options.opFilter != opName) return;
        
        if (!options.oneLineOutput && !options.toCSV) {
            std::cout << "Processing generic format file: " << filename << std::endl;
            std::cout << "Using operator name: " << opName << std::endl;
        }
    }
    
    // Skip if CSV file doesn't exist
    if (!csvHandler.isFileExists(csvPath)) {
        std::cerr << "Warning: CSV file does not exist: " << csvPath << std::endl;
        return;
    }
    
    // Read operator information
    OperatorInfo op;
    csvHandler.readOperatorInfo(opName, csvPath, op);
    
    // Process each function for strategy inference
    for (const auto& func : op.functions) {
        if (!options.oneLineOutput && !options.toCSV) {
            std::cout << "Processing function: " << func.name << std::endl;
        }
        
        // Perform strategy inference
        AccessStrategyDeducter deducter;
        deducter.deductAccessStrategy(func);
        
        if (options.toCSV) {
            // Write results to CSV file
            for (const auto& featureVector : deducter.accessFeatureVectors) {
                if (isLegacy) {
                    csvHandler.writeAccessStrategy(opName, dataset, func.name, featureVector);
                } else {
                    csvHandler.writeAccessStrategyGeneric(opName, func.name, featureVector);
                }
            }
        } else {
            // Print results to terminal
            if (options.oneLineOutput) {
                // One-line output format
                if (isLegacy) {
                    std::cout << dataset << " " << opName << " " << func.name << ": \n";
                } else {
                    std::cout << opName << " " << func.name << ": \n";
                }
                
                for (const auto& featureVector : deducter.accessFeatureVectors) {
                    printFeatureVectorOneLine(featureVector);
                }
            } else {
                // Regular output format
                if (options.showHeader) {
                    std::cout << "Function: " << func.name << std::endl;
                }
                
                for (const auto& featureVector : deducter.accessFeatureVectors) {
                    featureVector.printInfo();
                }
                
                std::cout << std::endl;
            }
        }
    }
}

// Process legacy format (original logic)
void processLegacyFormat(const CLIOptions& options) {
    // CSVHandler& csvHandler = CSVHandler::getInstance();
    
    // Dataset sizes
    std::vector<std::string> datasets = {"MINI", "SMALL", "STANDARD", "LARGE", "EXTRALARGE"};
    if (!options.datasetFilter.empty()) {
        datasets.clear();
        datasets.push_back(options.datasetFilter);
    }
    
    // Process all computation loads in data directory
    std::vector<std::string> operators = FileUtils::getSubdirectories("data");
    if (!options.opFilter.empty()) {
        operators.clear();
        operators.push_back(options.opFilter);
    }
    
    for (const auto& opName : operators) {
        if (!options.oneLineOutput && !options.toCSV) {
            std::cout << "\nProcessing computation load: " << opName << std::endl;
        }
        
        // Process each dataset size
        for (const auto& dataset : datasets) {
            std::string csvPath = "data/" + opName + "/" + dataset + "_DATASET_" + opName + ".csv";
            processCSVFile(csvPath, options);
        }
    }
}

int main(int argc, char *argv[]) {
    // Parse command line arguments
    CLIOptions options = parseArgs(argc, argv);

    // Get CSV handler instance
    CSVHandler& csvHandler = CSVHandler::getInstance();
    
    // Set options for CSV handler
    csvHandler.setOutputUTF8BOM(true);
    
    if (!options.csvPath.empty()) {
        // Process specific CSV file
        processCSVFile(options.csvPath, options);
    } else if (!options.opFilter.empty() || !options.datasetFilter.empty()) {
        // Use legacy format processing when filters are specified
        processLegacyFormat(options);
    } else {
        // Auto-detect and process all CSV files
        
        // First, try legacy format in data directory
        if (FileUtils::fileExists("data")) {
            processLegacyFormat(options);
        }
        
        // Then, process any CSV files in current directory (generic format)
        std::vector<std::string> csvFiles = FileUtils::getCSVFiles(".");
        for (const auto& csvFile : csvFiles) {
            // Skip if it matches legacy format pattern (already processed)
            std::string extractedDataset, extractedOpName;
            if (!FileUtils::isLegacyCSVFormat(csvFile, extractedDataset, extractedOpName)) {
                processCSVFile(csvFile, options);
            }
        }
    }
    
    if (options.toCSV) {
        std::cout << "\nAll CSV files processed, results saved to CSV files in the results directory." << std::endl;
    }
    
    return 0;
}