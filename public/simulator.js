lucide.createIcons();

document.getElementById('login-form').addEventListener('submit', async (e) => {
  e.preventDefault();
  const username = document.getElementById('username').value;
  const password = document.getElementById('password').value;
  const btn = document.getElementById('login-btn');
  const msgEl = document.getElementById('login-message');

  btn.disabled = true;
  btn.textContent = 'Authenticating...';
  msgEl.className = 'message hidden';

  try {
    const res = await fetch('/api/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password })
    });
    
    const data = await res.json();
    
    if (res.ok) {
      msgEl.textContent = data.message;
      msgEl.className = 'message success';
      setTimeout(() => {
        msgEl.className = 'message hidden';
        document.getElementById('username').value = '';
        document.getElementById('password').value = '';
      }, 2000);
    } else {
      msgEl.textContent = data.message;
      msgEl.className = 'message error';
      // Clear password field to simulate real login behavior
      document.getElementById('password').value = '';
    }
  } catch (err) {
    msgEl.textContent = 'Network error occurred.';
    msgEl.className = 'message error';
  } finally {
    btn.disabled = false;
    btn.textContent = 'Sign In';
  }
});

async function simulateEvent(action, btnId) {
  const btn = document.getElementById(btnId);
  const msgEl = document.getElementById('dev-message');
  const originalText = btn.innerHTML;
  
  btn.disabled = true;
  btn.innerHTML = 'Executing...';
  msgEl.className = 'message hidden';

  try {
    const res = await fetch('/api/simulate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ action })
    });
    const data = await res.json();
    
    msgEl.textContent = data.message;
    msgEl.className = 'message success';
    setTimeout(() => { msgEl.className = 'message hidden'; }, 3000);
  } catch (err) {
    msgEl.textContent = 'Diagnostic command failed.';
    msgEl.className = 'message error';
  } finally {
    btn.disabled = false;
    btn.innerHTML = originalText;
  }
}

document.getElementById('btn-crash').addEventListener('click', () => simulateEvent('db_crash', 'btn-crash'));
document.getElementById('btn-leak').addEventListener('click', () => simulateEvent('memory_leak', 'btn-leak'));
