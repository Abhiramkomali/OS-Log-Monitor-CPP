#include <iostream>
#include <vector>
#include <string>
#include "parser.h"
#include "detector.h"

int main() {
    std::string logFilePath;
    int choice;
    
    std::cout << "==========================================\n";
    std::cout << "  OS Log Monitor & Anomaly Detector CLI   \n";
    std::cout << "==========================================\n";
    std::cout << "1. Run analysis on default sample (logs/sample.log)\n";
    std::cout << "2. Enter a custom log file path\n";
    std::cout << "Enter your choice (1 or 2): ";
    
    // Project now requires user input before giving output
    std::cin >> choice;
    
    if (choice == 1) {
        logFilePath = "logs/sample.log";
    } else if (choice == 2) {
        std::cout << "Enter the path to your custom log file: ";
        std::cin >> logFilePath;
    } else {
        std::cout << "Invalid choice. Exiting.\n";
        return 1;
    }
    
    std::cout << "\nLoading log file: " << logFilePath << "...\n";
    
    // Parse the log file into structured data
    std::vector<LogEntry> logs = Parser::parseFile(logFilePath);
    
    if (logs.empty()) {
        std::cerr << "No logs to process. Ensure the log file exists and is not empty.\n";
        return 1;
    }
    
    std::cout << "Successfully parsed " << logs.size() << " log entries.\n";
    
    // Run anomaly detection rules on the parsed logs (gives output)
    Detector::analyze(logs);
    
    // Pause before exiting so the user can see the output
    std::cout << "\nPress Enter to exit...";
    std::cin.ignore();
    std::cin.get();
    
    return 0;
}
