#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
extern "C" {
#include "user_interface.h"
}

// example json
// {"play": "line one:  ", "captain": "line two"}

const char* host     = "foo.bar.tld"; //domain, no slashes
String path          = "/display.json"; // path to light.json beginning with slash (ex: /display)

LiquidCrystal lcd(D7, D6, D5, D1 , D2, D3);

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  Serial.println("IP address: " + WiFi.localIP());
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected");
  wifi_station_set_hostname("opslcd");
  Serial.begin(115200);
  lcd.begin(16, 2);
  delay(10);
}

void loop() {
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive\r\n\r\n");

  delay(3000);

  int dir = 0;
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(line);
    String play    = root[String("play")];
    String captain = root[String("captain")];

    Serial.println(play);
    Serial.println(captain);

    lcd.setCursor(0, 0);
    lcd.print(play);
    lcd.setCursor(0, 2);
    lcd.print(captain);

  }
  Serial.print("closing connection. ");
}
