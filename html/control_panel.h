#pragma once

// PROGMEM stores this in flash memory instead of RAM — important for big strings
const char CONTROL_PANEL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en" data-bs-theme="dark">
<head>
  <meta charset="UTF-8">
  <title>ESP32 Face Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.css" rel="stylesheet">
  <style>
    body {
      background: radial-gradient(ellipse at top, #1a1d26 0%, #0d0f14 100%);
      min-height: 100vh;
      font-family: -apple-system, system-ui, sans-serif;
    }
    .header {
      border-bottom: 1px solid rgba(255,255,255,0.08);
      padding: 1.5rem 0;
      margin-bottom: 1.5rem;
    }
    .header h1 { font-weight: 600; font-size: 1.5rem; margin: 0; }
    .header .dot {
      width: 8px; height: 8px; border-radius: 50%;
      background: #2ecc71; box-shadow: 0 0 12px #2ecc71;
      display: inline-block; margin-right: 8px;
      animation: pulse 2s infinite;
    }
    @keyframes pulse { 0%,100%{opacity:1} 50%{opacity:.4} }
    .card {
      background: rgba(255,255,255,0.03);
      border: 1px solid rgba(255,255,255,0.08);
      border-radius: 14px;
      backdrop-filter: blur(10px);
      margin-bottom: 1rem;
    }
    .card-title {
      font-size: 0.7rem;
      text-transform: uppercase;
      letter-spacing: 0.15em;
      color: #888;
      font-weight: 600;
      margin-bottom: 0.8rem;
    }
    .btn {
      font-weight: 500;
      padding: 0.6rem 0.8rem;
      border-radius: 10px;
      transition: all 0.15s ease;
      font-size: 0.9rem;
    }
    .btn:hover { transform: translateY(-1px); }
    .btn:active { transform: translateY(0); filter: brightness(0.85); }
    .btn-mood   { background: linear-gradient(135deg, #4af, #2876d6); color: white; border: none; }
    .btn-action { background: linear-gradient(135deg, #f7b731, #e58e26); color: white; border: none; }
    .btn-api { background: linear-gradient(135deg, #2ecc71, #27ae60); color: white; border: none; box-shadow: 0 0 8px rgba(46, 204, 113, 0.6); }
    .btn-look   { background: rgba(255,255,255,0.08); color: #eee; border: 1px solid rgba(255,255,255,0.1); }
    .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
    .dpad {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr;
      grid-template-rows: 1fr 1fr 1fr;
      gap: 8px; max-width: 240px; margin: 0 auto;
    }
    .dpad .empty { visibility: hidden; }
    .status-bar {
      background: rgba(0,0,0,0.3);
      border: 1px solid rgba(255,255,255,0.05);
      border-radius: 10px;
      padding: 0.6rem 1rem;
      font-family: monospace;
      font-size: 0.8rem;
      color: #888;
      margin-top: 1rem;
    }
    .status-bar.ok { color: #2ecc71; border-color: rgba(46,204,113,0.3); }
  </style>
</head>
<body>
<div class="container" style="max-width: 480px;">
  <div class="header d-flex align-items-center justify-content-between">
    <h1><i class="bi bi-emoji-smile me-2"></i>ESP32 Face</h1>
    <small class="text-muted"><span class="dot"></span>Online</small>
  </div>

  <div class="card"><div class="card-body">
    <div class="card-title">Mood</div>
    <div class="grid-2">
      <button class="btn btn-mood" onclick="hit('/face/happy')"><i class="bi bi-emoji-smile"></i> Happy</button>
      <button class="btn btn-mood" onclick="hit('/face/angry')"><i class="bi bi-emoji-angry"></i> Angry</button>
      <button class="btn btn-mood" onclick="hit('/face/tired')"><i class="bi bi-moon-stars"></i> Tired</button>
      <button class="btn btn-mood" onclick="hit('/face/neutral')"><i class="bi bi-emoji-neutral"></i> Neutral</button>
    </div>
  </div></div>

    <div class="card"><div class="card-body">
        <div class="card-title">API's</div>
            <div class="grid-2">
                <button class="btn btn-api" onclick="hit('/weather')">🌤️ Weather</button>
                <button class="btn btn-api" onclick="hit('/clock')">🕒 Clock</button>
            </div>
        </div>
    </div>

  <div class="card"><div class="card-body">
    <div class="card-title">Look</div>
    <div class="dpad">
      <div class="empty"></div>
      <button class="btn btn-look" onclick="hit('/look/up')"><i class="bi bi-arrow-up"></i></button>
      <div class="empty"></div>
      <button class="btn btn-look" onclick="hit('/look/left')"><i class="bi bi-arrow-left"></i></button>
      <button class="btn btn-look" onclick="hit('/look/center')"><i class="bi bi-record-circle"></i></button>
      <button class="btn btn-look" onclick="hit('/look/right')"><i class="bi bi-arrow-right"></i></button>
      <div class="empty"></div>
      <button class="btn btn-look" onclick="hit('/look/down')"><i class="bi bi-arrow-down"></i></button>
      <div class="empty"></div>
    </div>
  </div></div>

  <div class="card"><div class="card-body">
    <div class="card-title">Actions</div>
    <div class="grid-2">
      <button class="btn btn-action" onclick="hit('/face/blink')"><i class="bi bi-eye"></i> Blink</button>
      <button class="btn btn-action" onclick="hit('/face/laugh')"><i class="bi bi-emoji-laughing"></i> Laugh</button>
      <button class="btn btn-action" onclick="hit('/face/winkleft')">Wink L</button>
      <button class="btn btn-action" onclick="hit('/face/winkright')">Wink R</button>
      <button class="btn btn-action" onclick="hit('/face/confused')" style="grid-column: span 2"><i class="bi bi-question-circle"></i> Confused</button>
    </div>
  </div></div>

  <div class="status-bar" id="status">Ready.</div>
</div>

<script>
const status = document.getElementById('status');
async function hit(path) {
  status.textContent = 'Sending ' + path + '...';
  status.className = 'status-bar';
  try {
    const r = await fetch(path);
    status.textContent = r.ok ? 'OK: ' + path : 'Failed: ' + path;
    if (r.ok) status.className = 'status-bar ok';
  } catch (e) {
    status.textContent = 'Error: ' + e.message;
  }
}
</script>
</body>
</html>
)rawliteral";