#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ---------- Wi-Fi Config ----------
const char *ssid     = "TP-Link_DF6C_Cave";
const char *password = "Caveiot@123";

// ---------- Static IP Settings ----------
// Make sure this IP is free OR reserved for this device in your router.
IPAddress staticIP(192, 168, 1, 50);   // Desired ESP IP
IPAddress gateway (192, 168, 1, 1);    // Your router IP
IPAddress subnet  (255, 255, 255, 0);
IPAddress dns1    (192, 168, 1, 1);    // Can also use 8,8,8,8

// ---------- Web Server ----------
ESP8266WebServer server(80);

// ---------- Arduino Serial ----------
const int ARDUINO_BAUD = 9600;  // Must match Arduino's espSerial (or Serial) baud

// ---------- HTTP Handlers ----------
void handleRoot() {
  server.send(200, "text/plain", "ESP8266 Solenoid Bridge - Commands: /start, /stop, /bpm?val=120");
}

void handleStart() {
  Serial.println("S");  // Single character command
  Serial.flush();       // Ensure command is sent immediately
  delay(10);            // Small delay for reliability
  server.send(200, "text/plain", "Solenoid Started (S)");
}

void handleStop() {
  Serial.println("X");  // Single character command
  Serial.flush();       // Ensure command is sent immediately
  delay(10);            // Small delay for reliability
  server.send(200, "text/plain", "Solenoid Stopped (X)");
}

// Backward compatibility handlers
void handleEnable() {
  Serial.println("S");  // Use single character internally
  Serial.flush();
  delay(10);
  server.send(200, "text/plain", "Solenoid Enabled (S)");
}

void handleDisable() {
  Serial.println("X");  // Use single character internally
  Serial.flush();
  delay(10);
  server.send(200, "text/plain", "Solenoid Disabled (X)");
}

void handleBpm() {
  if (server.hasArg("val")) {
    String bpmCmd = "B" + server.arg("val");  // Simplified BPM command
    Serial.println(bpmCmd);
    Serial.flush();
    delay(10);
    server.send(200, "text/plain", "BPM set to " + server.arg("val"));
  } else {
    server.send(400, "text/plain", "Missing val parameter");
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

// ---------- Setup ----------
void setup() {
  Serial.begin(ARDUINO_BAUD);
  delay(100);

  Serial.println();
  Serial.println("[ESP] Booting.");

  // Apply static IP BEFORE begin()
  bool ok = WiFi.config(staticIP, gateway, subnet, dns1);
  if (!ok) {
    Serial.println("[ESP] WiFi.config FAILED (fallback to DHCP).");
  } else {
    Serial.println("[ESP] Static IP config applied.");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("[ESP] Connecting to ");
  Serial.print(ssid);

  // Wait for connection (timeout optional)
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 60) { // ~30s
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[ESP] Wi-Fi connected.");
  } else {
    Serial.println("[ESP] Wi-Fi FAILED to connect.");
  }

  Serial.print("[ESP] IP Address: ");
  Serial.println(WiFi.localIP());  // Should print 192.168.1.50 if static worked

  // HTTP routes - both new and backward compatible
  server.on("/",        handleRoot);
  server.on("/start",   handleStart);   // New single char command
  server.on("/stop",    handleStop);    // New single char command
  server.on("/enable",  handleEnable);  // Backward compatibility
  server.on("/disable", handleDisable); // Backward compatibility
  server.on("/bpm",     handleBpm);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("[ESP] HTTP server started.");
  Serial.println("[ESP] Routes: /start, /stop, /bpm?val=120");
}

// ---------- Loop ----------
void loop() {
  server.handleClient();
}