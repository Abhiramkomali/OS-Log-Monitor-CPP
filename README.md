# Log Monitoring and Anomaly Detection System

## 📌 Project Objective
The **Log Monitoring and Anomaly Detection System** is a lightweight, high-performance C++ application designed to parse system logs and detect abnormal patterns. By utilizing rule-based logic, the system effectively identifies suspicious activities such as multiple failed login attempts and high error rates, providing real-time alerts.

## 🚀 Features
- **Log Parsing**: Extracts timestamps, log levels (INFO, WARNING, ERROR), and messages from raw log text.
- **Anomaly Detection**:
  - Identifies potential brute-force attacks by tracking repeated failed login attempts per user.
  - Monitors and alerts upon reaching a high threshold of system ERROR logs.
- **Modular Design**: Separates concerns across different files (`parser`, `detector`, `main`) for clean architecture.
- **Color-Coded Alerts**: Uses ANSI escape codes to highlight critical alerts in the terminal.
- **Zero Dependencies**: Built strictly using standard C++ libraries.

## ⚙️ Working Explanation
1. **File I/O**: The system reads logs from `logs/sample.log` line by line.
2. **Parsing (`parser.cpp`)**: Each line is tokenized using `std::istringstream`. The timestamp is cleaned up, and the log is categorized into a `LogEntry` structure.
3. **Detection (`detector.cpp`)**: 
   - A `std::map` tracks the count of failed login attempts per username. If the count reaches the threshold (3), a RED alert is triggered. Successful logins reset this counter.
   - A counter tracks the total number of ERROR logs. If the threshold (5) is reached, a YELLOW alert is generated.
4. **Execution (`main.cpp`)**: Orchestrates the workflow by passing parsed logs into the detector for analysis.

## 🛠️ How to Compile and Run

### Compilation
Open your terminal/command prompt and compile the source code using `g++`:

```bash
g++ src/main.cpp src/parser.cpp src/detector.cpp -o monitor
```

### Execution
Run the compiled executable:

- On Windows:
  ```cmd
  monitor.exe
  ```
- On Linux/macOS:
  ```bash
  ./monitor
  ```

### Sample Output
```
Loading log file: logs/sample.log...
Successfully parsed 13 log entries.

--- Starting Anomaly Detection ---
[ALERT] Multiple failed login attempts (3) detected for user: john_doe at 2023-10-27 10:13:55
[ALERT] High error rate detected! 5 errors logged up to 2023-10-27 10:30:00
Total ERROR logs found: 5
--- Analysis Complete ---
```

## 🎓 Viva Questions & Answers

**Q1: What is the purpose of this project?**
**A1:** The project simulates a system monitor that parses raw log files and applies rule-based anomaly detection to find suspicious activities like potential brute-force attacks (repeated failed logins) and system instability (high error counts).

**Q2: How did you implement log parsing in C++?**
**A2:** I used `std::ifstream` to read the log file line by line. Then, I used `std::istringstream` to break each line into words (tokens), extracting the date, time, log level, and the rest as the log message. The data is stored in a `LogEntry` struct.

**Q3: How does the anomaly detection logic track failed logins for different users?**
**A3:** I used a `std::map<std::string, int>` where the key is the username and the value is the count of consecutive failed attempts. When a "Failed login" message is parsed, the username is extracted, and its map counter increments. If the counter hits the threshold, an alert is triggered. A successful login resets the user's counter to 0.

**Q4: Why did you choose C++ for this system?**
**A4:** C++ is highly efficient and provides low-level memory control, which is essential for parsing massive log files quickly. Features like STL (`std::vector`, `std::map`) make data manipulation robust and straightforward.

**Q5: How is memory managed in your application?**
**A5:** The project relies on RAII (Resource Acquisition Is Initialization) via C++ standard containers like `std::vector` and `std::string`. The `std::ifstream` automatically closes the file handle when it goes out of scope, preventing resource leaks without needing explicit manual memory management like `new` or `delete`.

**Q6: What is a struct, and why did you use `LogEntry` as a struct instead of a class?**
**A6:** A struct is a data structure in C++ where members are public by default. I used a struct for `LogEntry` because it acts as a simple Plain Old Data (POD) container holding the parsed log attributes (timestamp, level, message) without requiring complex encapsulation or member functions.

**Q7: How would you scale this project to handle millions of logs?**
**A7:** Instead of loading all logs into a `std::vector` at once (which consumes a lot of RAM), I would implement a streaming approach where each log line is parsed and immediately passed to the `Detector` for analysis before moving to the next line.

**Q8: What are ANSI escape codes, and how are they used here?**
**A8:** ANSI escape codes are special character sequences used to control text formatting and colors in terminal outputs. I used codes like `\033[31m` to print critical alerts in red, making them easily distinguishable from regular text.
