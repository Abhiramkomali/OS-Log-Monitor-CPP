// Initialize Lucide Icons
lucide.createIcons();

document.addEventListener('DOMContentLoaded', () => {
  const scanBtn = document.getElementById('scan-btn');
  const loadingOverlay = document.getElementById('loading-overlay');
  const levelFilter = document.getElementById('level-filter');
  let currentLogs = [];

  // Fetch initial data
  fetchData();

  // Event Listeners
  scanBtn.addEventListener('click', runAnalysis);
  
  levelFilter.addEventListener('change', (e) => {
    const level = e.target.value;
    renderLogs(currentLogs, level);
  });

  async function runAnalysis() {
    loadingOverlay.classList.add('active');
    
    try {
      const response = await fetch('/api/scan', { method: 'POST' });
      const result = await response.json();
      
      if (!response.ok) throw new Error(result.error || 'Scan failed');
      
      // Add a slight delay for smooth UX transition
      setTimeout(() => {
        loadingOverlay.classList.remove('active');
        fetchData(); // Refresh UI with new data
      }, 1000);
      
    } catch (error) {
      console.error('Error running scan:', error);
      alert('Error running analysis: ' + error.message);
      loadingOverlay.classList.remove('active');
    }
  }

  async function fetchData() {
    try {
      const [statsRes, anomaliesRes, logsRes] = await Promise.all([
        fetch('/api/stats'),
        fetch('/api/anomalies'),
        fetch('/api/logs')
      ]);

      const stats = await statsRes.json();
      const anomalies = await anomaliesRes.json();
      currentLogs = await logsRes.json();

      updateStats(stats);
      renderAnomalies(anomalies);
      renderLogs(currentLogs, levelFilter.value);
    } catch (error) {
      console.error('Error fetching data:', error);
    }
  }

  function updateStats(stats) {
    document.getElementById('stat-total').textContent = stats.totalLogs || 0;
    document.getElementById('stat-warnings').textContent = stats.warningLogs || 0;
    document.getElementById('stat-errors').textContent = stats.errorLogs || 0;
    document.getElementById('stat-anomalies').textContent = stats.totalAnomalies || 0;
    document.getElementById('anomaly-badge').textContent = `${stats.totalAnomalies || 0} Found`;
    
    // Dynamic system status
    const statusEl = document.getElementById('system-status');
    if (stats.totalAnomalies > 0 || stats.errorLogs > 5) {
      statusEl.innerHTML = '<span class="pulse-dot" style="background: var(--error); box-shadow: 0 0 0 0 rgba(239, 68, 68, 0.7);"></span> Critical Issues';
      statusEl.style.color = 'var(--error)';
      statusEl.style.background = 'rgba(239, 68, 68, 0.1)';
      statusEl.style.borderColor = 'rgba(239, 68, 68, 0.2)';
    } else {
      statusEl.innerHTML = '<span class="pulse-dot"></span> System Online';
      statusEl.style.color = 'var(--success)';
      statusEl.style.background = 'rgba(16, 185, 129, 0.1)';
      statusEl.style.borderColor = 'rgba(16, 185, 129, 0.2)';
    }
  }

  function renderAnomalies(anomalies) {
    const container = document.getElementById('anomalies-list');
    
    if (!anomalies || anomalies.length === 0) {
      container.innerHTML = `<div class="empty-state" style="padding: 20px; color: var(--success); font-weight: 500;"><i data-lucide="check-circle"></i> No anomalies detected. System is healthy.</div>`;
      lucide.createIcons();
      return;
    }

    container.innerHTML = anomalies.map(a => `
      <div class="anomaly-item">
        <div class="anomaly-icon"><i data-lucide="shield-alert"></i></div>
        <div class="anomaly-content">
          <h4>${escapeHTML(a.type)}</h4>
          <p>${escapeHTML(a.message)}</p>
          <div class="anomaly-time"><i data-lucide="clock" style="width:14px;height:14px;"></i> ${escapeHTML(a.timestamp)}</div>
        </div>
      </div>
    `).join('');
    
    lucide.createIcons();
  }

  function renderLogs(logs, filterLevel) {
    const tbody = document.getElementById('logs-tbody');
    
    let filteredLogs = logs;
    if (filterLevel && filterLevel !== 'ALL') {
      filteredLogs = logs.filter(l => l.level === filterLevel);
    }

    if (!filteredLogs || filteredLogs.length === 0) {
      tbody.innerHTML = `<tr><td colspan="3" style="text-align:center; padding: 30px; color: var(--text-muted);">No logs found.</td></tr>`;
      return;
    }

    tbody.innerHTML = filteredLogs.map(l => `
      <tr>
        <td style="color: var(--text-muted);">${escapeHTML(l.timestamp)}</td>
        <td><span class="level-tag tag-${escapeHTML(l.level)}">${escapeHTML(l.level)}</span></td>
        <td style="font-family: monospace; font-size: 13px;">${escapeHTML(l.message)}</td>
      </tr>
    `).join('');
  }

  function escapeHTML(str) {
    if (!str) return '';
    return str.replace(/[&<>'"]/g, 
      tag => ({
          '&': '&amp;',
          '<': '&lt;',
          '>': '&gt;',
          "'": '&#39;',
          '"': '&quot;'
        }[tag] || tag)
    );
  }
});
