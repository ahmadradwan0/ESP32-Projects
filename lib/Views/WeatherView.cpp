#include "WeatherView.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Orlando, Florida
#define WEATHER_LAT  28.5383
#define WEATHER_LON -81.3792

WeatherView::WeatherView(ISound& sound)
  : _sound(sound),
    _dataReady(false),
    _tempF(0),
    _tempC(0)
{
}

void WeatherView::OnEnter() {
  _sound.Happy();               // will replace with a dedicated Weather sound later
  _dataReady = _fetchWeather(); // blocking, might take 1-2 seconds
}

void WeatherView::OnExit() {
  // nothing to clean up
}

void WeatherView::Draw(Adafruit_SSD1306& display) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  if (!_dataReady) {
    // This branch will actually only show if fetch failed (error msg set)
    // or if we later make fetch async. For now, "Loading" is a placeholder.
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.print("Loading...");
  } else {
    // Title
    display.setTextSize(1);
    display.setCursor(0, 5);
    display.print("Current Weather");

    // Temperature line
    display.setTextSize(2);
    display.setCursor(0, 21);
    display.print((int)_tempF);
    display.print("F / ");
    display.print((int)_tempC);
    display.print("C");

    // Condition
    display.setTextSize(1);
    display.setCursor(0, 53);
    display.print(_conditionText);
  }

  display.display();
}

bool WeatherView::_fetchWeather() {
  HTTPClient http;

  // Use HTTP (not HTTPS) to avoid SSL certificate issues on ESP32
  String url = "http://api.open-meteo.com/v1/forecast";
  url += "?latitude=" + String(WEATHER_LAT, 4);
  url += "&longitude=" + String(WEATHER_LON, 4);
  url += "&current=temperature_2m,weathercode";
  url += "&temperature_unit=fahrenheit";
  url += "&forecast_days=1";

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode != 200) {
    _errorMsg = "HTTP " + String(httpCode);
    http.end();
    return false;
  }

  // Parse JSON
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, http.getString());
  http.end();

  if (error) {
    _errorMsg = "JSON error";
    return false;
  }

  // Extract current values
  float temp = doc["current"]["temperature_2m"];   // in °F because we asked for fahrenheit
  int   code = doc["current"]["weathercode"];

  _tempF = temp;
  _tempC = (temp - 32.0) * 5.0 / 9.0;
  _conditionText = _weatherCodeToString(code);

  return true;
}

String WeatherView::_weatherCodeToString(int code) {
  // WMO weather codes (subset)
  switch (code) {
    case 0:  return "Clear";
    case 1:  return "Mostly Clear";
    case 2:  return "Partly Cloudy";
    case 3:  return "Overcast";
    case 45: return "Fog";
    case 48: return "Rime Fog";
    case 51: return "Light Drizzle";
    case 53: return "Drizzle";
    case 55: return "Heavy Drizzle";
    case 61: return "Light Rain";
    case 63: return "Rain";
    case 65: return "Heavy Rain";
    case 71: return "Light Snow";
    case 73: return "Snow";
    case 75: return "Heavy Snow";
    case 95: return "Thunderstorm";
    default: return "Unknown";
  }
}