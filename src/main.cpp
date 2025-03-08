#include "OperatorInfo.hpp"
#include "CSVHandler.hpp"
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
        {0,           0,                 0,  0 }
    };

    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "hc1no:d:", long_options, &option_index)) != -1) {
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
            case '?':
                printHelp(argv[0]);
                exit(1);
                break;
        }
    }
    
    return options;
}

// Check if file exists
bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// Create directory
void createDirectory(const std::string& path) {
    mkdir(path.c_str(), 0777);
}

// Get all subdirectories in a directory
std::vector<std::string> getSubdirectories(const std::string& path) {
    std::vector<std::string> dirs;
    DIR* dir = opendir(path.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_DIR && 
                std::string(entry->d_name) != "." && 
                std::string(entry->d_name) != ".." &&
                std::string(entry->d_name) != ".DS_Store") {
                dirs.push_back(entry->d_name);
            }
        }
        closedir(dir);
    }
    return dirs;
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

int main(int argc, char *argv[]) {
    // Parse command line arguments
    CLIOptions options = parseArgs(argc, argv);

    // Get CSV handler instance
    CSVHandler& csvHandler = CSVHandler::getInstance();
    
    // Set options for CSV handler
    csvHandler.setOutputUTF8BOM(true);
    
    // Dataset sizes
    std::vector<std::string> datasets = {"MINI", "SMALL", "STANDARD", "LARGE", "EXTRALARGE"};
    if (!options.datasetFilter.empty()) {
        datasets.clear();
        datasets.push_back(options.datasetFilter);
    }
    
    // Process all computation loads in data directory
    std::vector<std::string> operators = getSubdirectories("data");
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
            
            // Skip if CSV file doesn't exist
            if (!csvHandler.isFileExists(csvPath)) continue;
            
            if (!options.oneLineOutput && !options.toCSV) {
                std::cout << "Processing dataset: " << dataset << std::endl;
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
                        csvHandler.writeAccessStrategy(opName, dataset, func.name, featureVector);
                    }
                } else {
                    // Print results to terminal
                    if (options.oneLineOutput) {
                        // One-line output format
                        std::cout << dataset << " " << opName << " " << func.name << ": \n";
                        
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
    }
    
    if (options.toCSV) {
        std::cout << "\nAll computation loads processed, results saved to CSV files in the results directory." << std::endl;
    }
    
    return 0;
}