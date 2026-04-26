#include "AppServer.h"

AppServer::AppServer(int port) 
  : _server(port),
    _ssid(nullptr),
    _password(nullptr),
    _onConnected(nullptr),
    _started(false)
{}

void AppServer::SetWifi(const char* ssid, const char* password) {
  _ssid = ssid;
  _password = password;
}

void AppServer::OnConnected(ConnectedCallback cb) {
  _onConnected = cb;
}

// ---------- Route registration ----------

void AppServer::OnGetHTML(const char* path, GetHandler handler) {
  _server.on(path, HTTP_GET, [this, handler]() {
    _server.send(200, "text/html", handler());
  });
}

void AppServer::OnGetJSON(const char* path, GetHandler handler) {
  _server.on(path, HTTP_GET, [this, handler]() {
    _server.send(200, "application/json", handler());
  });
}

void AppServer::OnGetText(const char* path, GetHandler handler) {
  _server.on(path, HTTP_GET, [this, handler]() {
    _server.send(200, "text/plain", handler());
  });
}

void AppServer::OnPostText(const char* path, PostHandler handler) {
  _server.on(path, HTTP_POST, [this, handler]() {
    String body = _server.arg("plain");
    _server.send(200, "text/plain", handler(body));
  });
}

void AppServer::OnPostJSON(const char* path, PostHandler handler) {
  _server.on(path, HTTP_POST, [this, handler]() {
    String body = _server.arg("plain");
    _server.send(200, "application/json", handler(body));
  });
}

// ---------- WiFi (isolated to this one method) ----------

bool AppServer::_connectWifi() {
  if (_ssid == nullptr || _password == nullptr) return false;

  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 60) {
    delay(500);
    tries++;
  }

  return WiFi.status() == WL_CONNECTED;
}

// ---------- Lifecycle ----------

bool AppServer::Start() {
  if (!_connectWifi()) return false;

  _server.begin();
  _started = true;

  if (_onConnected) {
    _onConnected(WiFi.localIP());
  }
  return true;
}

void AppServer::Update() {
  if (_started) _server.handleClient();
}

void AppServer::Stop() {
  _server.stop();
  _started = false;
}

bool AppServer::IsConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

IPAddress AppServer::GetIP() const {
  return WiFi.localIP();
}