#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <windows.h> // For Sleep() on Windows
#include "parser.h"
#include "detector.h"

int main() {
    std::string logFilePath;
    int choice;
    
    std::cout << "==========================================\n";
    std::cout << "  OS Log Monitor & Anomaly Detector CLI   \n";
    std::cout << "==========================================\n";
    std::cout << "1. Run batch analysis on default sample (logs/sample.log)\n";
    std::cout << "2. Run batch analysis on a custom log file\n";
    std::cout << "3. Live Monitor a custom log file (Real-Time)\n";
    std::cout << "Enter your choice (1, 2, or 3): ";
    
    // Project now requires user input before giving output
    std::cin >> choice;
    
    if (choice == 1) {
        logFilePath = "logs/sample.log";
    } else if (choice == 2 || choice == 3) {
        std::cout << "Enter the path to your log file: ";
        std::cin >> logFilePath;
    } else {
        std::cout << "Invalid choice. Exiting.\n";
        return 1;
    }
    
    // Instantiate our new stateful Detector
    Detector detector(logFilePath);
    
    if (choice == 1 || choice == 2) {
        std::cout << "\nLoading log file: " << logFilePath << "...\n";
        
        // Parse the log file into structured data
        std::vector<LogEntry> logs = Parser::parseFile(logFilePath);
        
        if (logs.empty()) {
            std::cerr << "No logs to process. Ensure the log file exists and is not empty.\n";
            return 1;
        }
        
        std::cout << "Successfully parsed " << logs.size() << " log entries.\n";
        
        // Run anomaly detection rules on the parsed logs
        detector.analyze(logs);
        
        // Pause before exiting so the user can see the output
        std::cout << "\nPress Enter to exit...";
        std::cin.ignore(); // handle trailing newline
        std::cin.get();
    }
    else if (choice == 3) {
        std::cout << "\n[LIVE MONITORING] Starting real-time tail on: " << logFilePath << "\n";
        std::cout << "Waiting for new logs... (Press Ctrl+C to stop)\n\n";
        
        std::ifstream file(logFilePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open log file: " << logFilePath << std::endl;
            return 1;
        }
        
        // Skip to the end of the file so we only analyze NEW incoming logs
        file.seekg(0, std::ios::end);
        
        std::string line;
        while (true) {
            // Check if there is a new line added to the file
            if (std::getline(file, line)) {
                if (!line.empty()) {
                    LogEntry entry = Parser::parseLine(line);
                    detector.analyzeLine(entry, true); // analyze and update report.html immediately
                }
            } else {
                // If EOF reached, clear the error state and wait before checking again
                file.clear();
                Sleep(500);
            }
        }
    }
    
    return 0;
}
