#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

#define RELAY_ON 1      // Define relay on pin state
#define RELAY_OFF 0     // Define relay off pin state
 
#define Relay  D8       // Arduino pin where the relay connects

const char* deviceId = "waterTankController";

// WiFi and IP connection info.
const char* ssid = "";
const char* password = "";
const int port = 9926;

// Define server
ESP8266WebServer server(port);

void setup() {

  Wire.begin();
  Serial.begin(9600); 

  Serial.println("Starting **...");

  // Set WiFi mode to client (without this it may try to act as an AP).
  WiFi.mode(WIFI_STA);

  // Configure Hostname
  if ((deviceId != NULL) && (deviceId[0] == '\0')) {
    Serial.printf("No Device ID is Defined, Defaulting to board defaults");
  }
  else {
    wifi_station_set_hostname(deviceId);
    WiFi.setHostname(deviceId);
  }

  // Setup and wait for WiFi.
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Hostname: ");
  Serial.println(WiFi.hostname());

  // Setup Routing
  server.on("/on", HandleOn);
  server.on("/off", HandleOff);
  server.onNotFound(HandleNotFound);

  server.begin();
  Serial.println("HTTP server started at ip " + WiFi.localIP().toString() + ":" + String(port));

  // Configure relay
  digitalWrite(Relay, RELAY_OFF);
  pinMode(Relay, OUTPUT);   
  delay(1000); 

  Serial.println("Started ...");
}

void loop() {
  server.handleClient();
}

void HandleOff() {
  digitalWrite(Relay, RELAY_OFF);
  server.send(200, "text/plain", "turned off");
}

void HandleOn() {
  digitalWrite(Relay, RELAY_ON); 
  server.send(200, "text/plain", "turned on");
}

void HandleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/html", message);
}
