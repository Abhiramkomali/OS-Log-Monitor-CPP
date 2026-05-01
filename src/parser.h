#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

// Structure to hold parsed log data
struct LogEntry {
    std::string timestamp;
    std::string level;    // e.g., INFO, ERROR, WARNING
    std::string message;  // e.g., "User logged in", "Failed login attempt"
};

class Parser {
public:
    // Reads the log file and parses each line into a LogEntry
    static std::vector<LogEntry> parseFile(const std::string& filepath);

private:
    // Helper function to parse a single line
    static LogEntry parseLine(const std::string& line);
};

#endif // PARSER_H
