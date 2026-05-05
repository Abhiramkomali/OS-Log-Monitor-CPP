#ifndef DETECTOR_H
#define DETECTOR_H

#include <vector>
#include <string>
#include "parser.h"

#include <map>

// Structure to hold data about a detected anomaly
struct Anomaly {
    std::string type;
    std::string message;
    std::string timestamp;
};

class Detector {
public:
    // Constructor takes the path of the log file being monitored
    Detector(const std::string& logFilePath);

    // Batch analysis (for existing Mode 1 & 2)
    void analyze(const std::vector<LogEntry>& logs);

    // Real-time analysis of a single line (for Mode 3)
    void analyzeLine(const LogEntry& entry, bool generateReport = true);

private:
    static const int MAX_FAILED_LOGINS = 3; // Threshold for failed logins
    static const int MAX_ERROR_COUNT = 5;   // Threshold for ERROR logs

    // Stateful variables to remember history
    std::string currentLogFile;
    std::map<std::string, int> failedAttempts;
    int errorCount;
    std::vector<Anomaly> anomalies;
    std::vector<LogEntry> parsedHistory;

    // Helper functions for specific anomaly checks
    void checkFailedLogin(const LogEntry& entry);
    void checkErrorRate(const LogEntry& entry);
    
    // Automatically generates an HTML Webpage Dashboard
    void generateHtmlReport();
    
    // Automatically generates a JSON file for the Node.js backend
    void generateJsonReport();
};

#endif // DETECTOR_H
