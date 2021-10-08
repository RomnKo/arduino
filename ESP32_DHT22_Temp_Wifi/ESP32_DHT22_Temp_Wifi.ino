/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

// Import required libraries
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "time.h"
#include <ArduinoOTA.h>
#include "SPIFFS.h"

// Replace with your network credentials
const char* ssid = "ILoveCatsTheMost2G";
const char* password = "ilovecatsverymuch";

// Set time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -28800;
const int   daylightOffset_sec = 3600;

#define DHTPIN 27       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

IPAddress local_IP(192, 168, 1, 199);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 4);   //optional
IPAddress secondaryDNS(192, 168, 1, 4); //optional

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }

  Serial.println(f);
  return String(f);
}

String getDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  Serial.println(h);
  return String(h);
}

String getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "--";
  }

  return asctime (&timeinfo);
}

//const char index_html[] PROGMEM = R"rawliteral()rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var) {
  //Serial.println(var);
  if (var == "TEMPERATURE") {
    return getDHTTemperature();
  }
  else if (var == "HUMIDITY") {
    return getDHTHumidity();
  }
  else if (var == "TIME") {
    return getTime();
  }
  return String();
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  dht.begin();

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Wifi Status: ");
  Serial.print(WiFi.status());

  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  unsigned long interval = 10000;
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    Serial.println("Wifi Not Connected Status: ");
    Serial.println(WiFi.status());
    Serial.println(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    Serial.println("Wifi ReConnect Status: ");
    Serial.println(WiFi.status());
    previousMillis = currentMillis;
  }
  Serial.println(WiFi.status());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getDHTHumidity().c_str());
  });
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getTime().c_str());
  });

  // Start server
  server.begin();
  Serial.println("server started");

  // Init and get the time
  Serial.println("Get the time");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
  
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void loop() {
  delay(1000);
  printLocalTime();
}


void printLocalTime() {
  delay(5000);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println(&timeinfo, "%A, %I %d %Y %H:%M:%S");

  ArduinoOTA.handle();
}
