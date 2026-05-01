#ifndef DETECTOR_H
#define DETECTOR_H

#include <vector>
#include <string>
#include "parser.h"

// Structure to hold data about a detected anomaly
struct Anomaly {
    std::string type;
    std::string message;
    std::string timestamp;
};

class Detector {
public:
    // Analyze logs and detect anomalies based on rules
    static void analyze(const std::vector<LogEntry>& logs);

private:
    static const int MAX_FAILED_LOGINS = 3; // Threshold for failed logins
    static const int MAX_ERROR_COUNT = 5;   // Threshold for ERROR logs

    // Helper functions for specific anomaly checks
    static void checkFailedLogins(const std::vector<LogEntry>& logs, std::vector<Anomaly>& anomalies);
    static void checkErrorRates(const std::vector<LogEntry>& logs, std::vector<Anomaly>& anomalies);
    
    // Automatically generates an HTML Webpage Dashboard
    static void generateHtmlReport(const std::vector<LogEntry>& logs, const std::vector<Anomaly>& anomalies);
};

#endif // DETECTOR_H
