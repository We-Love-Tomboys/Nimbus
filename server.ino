#include <WiFi.h>
#include <WebServer.h>
#include "DHTesp.h"
#include "time.h"

// Wi-Fi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// NTP settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; // IST (+5:30)
const int daylightOffset_sec = 0;

DHTesp dht;               // DHT object
WebServer server(80);     // HTTP server

struct tm timeinfo;       // Declare globally so setup() and handleData() can use it

void handleData() {
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    server.send(500, "text/plain", "Failed to get time");
    return;
  }

  char timeStr[25];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

  // Prepare JSON
  String json = "{";
  json += "\"temperature\": " + String(temperature, 1) + ",";
  json += "\"humidity\": " + String(humidity, 1) + ",";
  json += "\"timestamp\": \"" + String(timeStr) + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.setup(4, DHTesp::DHT11);  // DHT11 on GPIO4

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());

  // Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Waiting for NTP time sync...");
  while(!getLocalTime(&timeinfo)) {  // Now it works
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nTime synced!");

  // Set up HTTP route
  server.on("/data", handleData);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();  // Handle incoming HTTP requests
}
