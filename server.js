require('dotenv').config();
const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

const Log = require('./models/Log');
const Anomaly = require('./models/Anomaly');

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// Connect to MongoDB
mongoose.connect(process.env.MONGO_URI)
  .then(() => console.log('✅ Connected to MongoDB'))
  .catch(err => console.error('❌ MongoDB connection error:', err));

// --- Log Generation Helpers ---
function getFormattedTimestamp() {
  const now = new Date();
  const pad = (n) => n.toString().padStart(2, '0');
  const dateStr = `${now.getFullYear()}-${pad(now.getMonth() + 1)}-${pad(now.getDate())}`;
  const timeStr = `${pad(now.getHours())}:${pad(now.getMinutes())}:${pad(now.getSeconds())}`;
  return `[${dateStr} ${timeStr}]`;
}

function appendLog(level, message) {
  const logLine = `${getFormattedTimestamp()} ${level} ${message}\n`;
  const logFilePath = path.join(__dirname, 'logs', 'sample.log');
  try {
    if (!fs.existsSync(path.dirname(logFilePath))) {
      fs.mkdirSync(path.dirname(logFilePath), { recursive: true });
    }
    fs.appendFileSync(logFilePath, logLine);
    console.log(`Appended log: ${logLine.trim()}`);
  } catch (err) {
    console.error('Failed to write to sample.log', err);
  }
}

// --- Simulator API Endpoints ---

app.post('/api/login', (req, res) => {
  const { username, password } = req.body;
  if (!username) return res.status(400).json({ success: false, message: 'Username required' });

  // Fake auth: admin/admin123 works, anything else fails.
  if (username === 'admin' && password === 'admin123') {
    appendLog('INFO', `Successful login for user ${username}`);
    return res.json({ success: true, message: 'Login successful! Redirecting...' });
  } else {
    appendLog('WARNING', `Failed login attempt for user ${username}`);
    return res.status(401).json({ success: false, message: 'Invalid username or password' });
  }
});

app.post('/api/simulate', (req, res) => {
  const { action } = req.body;
  if (action === 'db_crash') {
    appendLog('ERROR', 'Database connection lost');
    appendLog('ERROR', 'Failed to write to disk');
    return res.json({ success: true, message: 'Simulated Database Crash logged.' });
  } 
  if (action === 'memory_leak') {
    appendLog('ERROR', 'Memory usage high');
    appendLog('ERROR', 'Service crashed unexpectedly');
    return res.json({ success: true, message: 'Simulated Memory Leak logged.' });
  }
  res.status(400).json({ error: 'Unknown simulation action' });
});

// --- Dashboard API Endpoints ---

// Get all logs
app.get('/api/logs', async (req, res) => {
  try {
    const logs = await Log.find();
    res.json(logs);
  } catch (error) {
    res.status(500).json({ error: 'Failed to fetch logs' });
  }
});

// Get all anomalies
app.get('/api/anomalies', async (req, res) => {
  try {
    const anomalies = await Anomaly.find();
    res.json(anomalies);
  } catch (error) {
    res.status(500).json({ error: 'Failed to fetch anomalies' });
  }
});

// Get stats
app.get('/api/stats', async (req, res) => {
  try {
    const totalLogs = await Log.countDocuments();
    const errorLogs = await Log.countDocuments({ level: 'ERROR' });
    const warningLogs = await Log.countDocuments({ level: 'WARNING' });
    const infoLogs = await Log.countDocuments({ level: 'INFO' });
    const totalAnomalies = await Anomaly.countDocuments();

    res.json({
      totalLogs,
      errorLogs,
      warningLogs,
      infoLogs,
      totalAnomalies
    });
  } catch (error) {
    res.status(500).json({ error: 'Failed to fetch stats' });
  }
});

// Trigger a scan via the C++ Executable
app.post('/api/scan', (req, res) => {
  console.log('🚀 Starting log analysis via C++ Monitor...');
  
  // The executable path
  const exePath = path.join(__dirname, process.platform === 'win32' ? 'monitor.exe' : 'monitor');
  
  const monitor = spawn(exePath, [], { cwd: __dirname });

  let output = '';
  
  monitor.stdout.on('data', (data) => {
    output += data.toString();
    // When the CLI asks for choice, provide '1' for default sample
    if (data.toString().includes('Enter your choice')) {
      monitor.stdin.write("1\n");
    }
    // When the CLI asks to press Enter to exit
    if (data.toString().includes('Press Enter to exit')) {
      monitor.stdin.write("\n");
    }
  });

  monitor.stderr.on('data', (data) => {
    console.error(`C++ Error: ${data}`);
  });

  monitor.on('close', async (code) => {
    console.log(`C++ Monitor exited with code ${code}`);
    
    if (code !== 0) {
      return res.status(500).json({ error: 'C++ execution failed', details: output });
    }

    try {
      // Read the generated JSON report
      const reportPath = path.join(__dirname, 'report.json');
      if (!fs.existsSync(reportPath)) {
        return res.status(500).json({ error: 'report.json was not generated' });
      }

      const rawData = fs.readFileSync(reportPath, 'utf8');
      const data = JSON.parse(rawData);

      // Clear existing DB data for a fresh state (optional, based on requirement)
      // Here we replace the DB state with the latest scan
      await Log.deleteMany({});
      await Anomaly.deleteMany({});

      // Insert new data into MongoDB
      if (data.logs && data.logs.length > 0) {
        await Log.insertMany(data.logs);
      }
      
      if (data.anomalies && data.anomalies.length > 0) {
        await Anomaly.insertMany(data.anomalies);
      }

      console.log('✅ MongoDB updated successfully with scan results.');
      res.json({ message: 'Scan complete', stats: {
        processedLogs: data.logs.length,
        anomaliesFound: data.anomalies.length,
        errors: data.errorCount
      }});

    } catch (err) {
      console.error('Error parsing report.json or saving to DB:', err);
      res.status(500).json({ error: 'Failed to process results', details: err.message });
    }
  });
});

app.listen(PORT, () => {
  console.log(`🌍 Server running on http://localhost:${PORT}`);
});
