#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <functional>

class AppServer {
  public:
    // Type aliases for handler signatures (cleaner than writing them out everywhere)
    using GetHandler = std::function<String()>;
    using PostHandler = std::function<String(String body)>;
    using ConnectedCallback = std::function<void(IPAddress ip)>;

    AppServer(int port = 80);

    // ==== Configuration ====
    void SetWifi(const char* ssid, const char* password);
    void OnConnected(ConnectedCallback cb);

    // ==== Route registration ====
    void OnGetHTML(const char* path, GetHandler handler);
    void OnGetJSON(const char* path, GetHandler handler);
    void OnGetText(const char* path, GetHandler handler);
    void OnPostText(const char* path, PostHandler handler);
    void OnPostJSON(const char* path, PostHandler handler);

    // ==== Lifecycle ====
    bool Start();    // connects to WiFi + starts the HTTP server
    void Update();   // call every loop iteration
    void Stop();

    // ==== Utility ====
    bool IsConnected() const;
    IPAddress GetIP() const;

  private:
    WebServer _server;
    const char* _ssid;
    const char* _password;
    ConnectedCallback _onConnected;
    bool _started;

    bool _connectWifi();   // all WiFi logic in one place
};