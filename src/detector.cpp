#include "detector.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>

// Console color codes for better readability
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string YELLOW = "\033[33m";
const std::string CYAN = "\033[36m";

void Detector::analyze(const std::vector<LogEntry>& logs) {
    std::cout << CYAN << "\n--- Starting Anomaly Detection ---" << RESET << "\n";
    
    std::vector<Anomaly> anomalies;
    
    checkFailedLogins(logs, anomalies);
    checkErrorRates(logs, anomalies);
    
    // Automatically generate the HTML Dashboard
    generateHtmlReport(logs, anomalies);
    
    std::cout << CYAN << "--- Analysis Complete ---\n" << RESET << "\n";
    std::cout << "Dashboard generated successfully at: " << YELLOW << "report.html" << RESET << "\n";
}

void Detector::checkFailedLogins(const std::vector<LogEntry>& logs, std::vector<Anomaly>& anomalies) {
    std::map<std::string, int> failedAttempts;
    
    for (const auto& entry : logs) {
        if (entry.level == "WARNING" && entry.message.find("Failed login") != std::string::npos) {
            size_t pos = entry.message.find("user ");
            if (pos != std::string::npos) {
                std::string username = entry.message.substr(pos + 5);
                failedAttempts[username]++;
                
                if (failedAttempts[username] == MAX_FAILED_LOGINS) {
                    std::string msg = "Multiple failed login attempts (" + std::to_string(MAX_FAILED_LOGINS) + ") detected for user: " + username;
                    std::cout << RED << "[ALERT] " << msg << " at " << entry.timestamp << RESET << "\n";
                    anomalies.push_back({"Security Alert", msg, entry.timestamp});
                }
            }
        } else if (entry.level == "INFO" && entry.message.find("Successful login") != std::string::npos) {
            size_t pos = entry.message.find("user ");
            if (pos != std::string::npos) {
                std::string username = entry.message.substr(pos + 5);
                failedAttempts[username] = 0; 
            }
        }
    }
}

void Detector::checkErrorRates(const std::vector<LogEntry>& logs, std::vector<Anomaly>& anomalies) {
    int errorCount = 0;
    
    for (const auto& entry : logs) {
        if (entry.level == "ERROR") {
            errorCount++;
            
            if (errorCount == MAX_ERROR_COUNT) {
                std::string msg = "High error rate detected! " + std::to_string(MAX_ERROR_COUNT) + " errors logged.";
                std::cout << YELLOW << "[ALERT] " << msg << " up to " << entry.timestamp << RESET << "\n";
                anomalies.push_back({"System Health", msg, entry.timestamp});
            }
        }
    }
    
    std::cout << "Total ERROR logs found: " << errorCount << "\n";
}

void Detector::generateHtmlReport(const std::vector<LogEntry>& logs, const std::vector<Anomaly>& anomalies) {
    std::ofstream htmlFile("report.html");
    if (!htmlFile.is_open()) {
        std::cerr << "Error: Could not create report.html\n";
        return;
    }
    
    htmlFile << "<!DOCTYPE html>\n<html lang='en'>\n<head>\n";
    htmlFile << "<meta charset='UTF-8'>\n";
    htmlFile << "<title>Log Monitor Dashboard</title>\n";
    htmlFile << "<style>\n";
    htmlFile << "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f7f6; margin: 0; padding: 20px; }\n";
    htmlFile << ".container { max-width: 1000px; margin: 0 auto; }\n";
    htmlFile << "h1 { color: #2c3e50; text-align: center; border-bottom: 2px solid #3498db; padding-bottom: 10px; }\n";
    htmlFile << ".card { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); margin-bottom: 20px; }\n";
    htmlFile << ".alert { background-color: #ffebee; border-left: 5px solid #f44336; padding: 15px; margin-bottom: 10px; border-radius: 4px; }\n";
    htmlFile << ".alert-title { font-weight: bold; color: #d32f2f; margin-bottom: 5px; }\n";
    htmlFile << "table { width: 100%; border-collapse: collapse; margin-top: 15px; }\n";
    htmlFile << "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }\n";
    htmlFile << "th { background-color: #2c3e50; color: white; }\n";
    htmlFile << ".level-INFO { color: #2196F3; font-weight: bold; }\n";
    htmlFile << ".level-WARNING { color: #FF9800; font-weight: bold; }\n";
    htmlFile << ".level-ERROR { color: #F44336; font-weight: bold; }\n";
    htmlFile << "</style>\n</head>\n<body>\n";
    
    htmlFile << "<div class='container'>\n";
    htmlFile << "<h1>🛡️ System Log & Anomaly Dashboard</h1>\n";
    
    // Read raw input
    std::ifstream rawFile("logs/sample.log");
    std::string rawInput;
    if (rawFile.is_open()) {
        rawInput.assign((std::istreambuf_iterator<char>(rawFile)), std::istreambuf_iterator<char>());
    } else {
        rawInput = "Could not load raw log file.";
    }

    // Input Section
    htmlFile << "<div class='card'>\n";
    htmlFile << "<h2>📥 1. Raw Input Logs</h2>\n";
    htmlFile << "<p>This is the raw data being fed into the C++ program:</p>\n";
    htmlFile << "<pre style='background: #1e1e1e; color: #00ff00; padding: 15px; border-radius: 5px; overflow-x: auto; font-family: monospace;'>" << rawInput << "</pre>\n";
    htmlFile << "</div>\n";
    
    // Anomalies Section
    htmlFile << "<div class='card'>\n";
    htmlFile << "<h2>🚨 2. Detected Anomalies (Output)</h2>\n";
    if (anomalies.empty()) {
        htmlFile << "<p style='color: green; font-weight: bold;'>✅ System is healthy. No anomalies detected.</p>\n";
    } else {
        for (const auto& a : anomalies) {
            htmlFile << "<div class='alert'>\n";
            htmlFile << "<div class='alert-title'>[" << a.type << "] at " << a.timestamp << "</div>\n";
            htmlFile << "<div>" << a.message << "</div>\n";
            htmlFile << "</div>\n";
        }
    }
    htmlFile << "</div>\n";
    
    // Logs Table Section
    htmlFile << "<div class='card'>\n";
    htmlFile << "<h2>📜 3. Parsed Output (Data Structures)</h2>\n";
    htmlFile << "<p>The raw text is parsed into C++ structs and analyzed line-by-line:</p>\n";
    htmlFile << "<table>\n<tr><th>Timestamp</th><th>Level</th><th>Message</th></tr>\n";
    
    for (const auto& entry : logs) {
        htmlFile << "<tr>\n";
        htmlFile << "<td>" << entry.timestamp << "</td>\n";
        htmlFile << "<td class='level-" << entry.level << "'>" << entry.level << "</td>\n";
        htmlFile << "<td>" << entry.message << "</td>\n";
        htmlFile << "</tr>\n";
    }
    
    htmlFile << "</table>\n</div>\n";
    
    htmlFile << "</div>\n</body>\n</html>\n";
    htmlFile.close();
}
