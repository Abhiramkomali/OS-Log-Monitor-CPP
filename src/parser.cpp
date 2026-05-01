#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

LogEntry Parser::parseLine(const std::string& line) {
    LogEntry entry;
    std::istringstream iss(line);
    
    // Expected format: [YYYY-MM-DD HH:MM:SS] LEVEL Message...
    // Example: [2023-10-27 10:00:00] ERROR Failed login attempt for user admin
    
    std::string date, time;
    if (iss >> date >> time >> entry.level) {
        // Remove brackets from timestamp
        if (!date.empty() && date.front() == '[') date = date.substr(1);
        if (!time.empty() && time.back() == ']') time.pop_back();
        
        entry.timestamp = date + " " + time;
        
        // The rest of the line is the message
        std::string word;
        while (iss >> word) {
            if (!entry.message.empty()) {
                entry.message += " ";
            }
            entry.message += word;
        }
    }
    
    return entry;
}

std::vector<LogEntry> Parser::parseFile(const std::string& filepath) {
    std::vector<LogEntry> entries;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open log file: " << filepath << std::endl;
        return entries;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            entries.push_back(parseLine(line));
        }
    }
    
    file.close();
    return entries;
}
