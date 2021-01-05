//!Arduino 1.8.12
//-*- coding: utf-8 -*-
#define __author__  'Kirati Sreesamran'
#define __copyright__  'Copyright 2020, Pallet High Alarm'
#define __credits__  ['Kirati Sreesamran']
#define __license__  'NPOSL-3.0'
#define __version__  '2.0.1'
#define __maintainer__  'Kirati Sreesamran'
#define __email__  'kisr.kkc@gmail.com'
#define __status__  'In use'
// Built-in/Generic Imports
#ifdef ESP32
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
#define LED_ON      HIGH
#define LED_OFF     LOW
#else
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#define ESP_getChipId()   (ESP.getChipId())
#define LED_ON      LOW
#define LED_OFF     HIGH
#endif

#include <ArduinoOTA.h>
#include <Wire.h>

// 3rd Party Library
#define USE_AVAILABLE_PAGES     false
#include <ESP_WiFiManager.h>
#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, 4, 15);
//SSD1306Wire display(0x3c, 21, 22);
//########################
//#####   DATA SET   #####
//#####     BOOL     #####
//########################
bool shouldSaveConfig = false;
bool ledState = LOW;
//########################
//#####    NUMBER    #####
//########################
unsigned long startedAt;
unsigned long previousMillis = 0;
const long interval = 1000;
int station_List;
//########################
//#####    STRING    #####
//########################
const String project_name = "TTGO_OLED_";
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
const char* password = "00000000";
String Router_SSID;
String Router_Pass;
char mac[16] = "";
String MAC_T = "";
String MAC_D = "";
//########################
//#####     GPIO     #####
//#####   GPIO PIN   #####
//########################
const int PIN_LED = 2;
const int AP_Config = 0;

void setup() {
  pinMode(16, OUTPUT); digitalWrite(16, 1);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  pinMode(PIN_LED, OUTPUT);
  pinMode(AP_Config, INPUT);
  unsigned long startedAt = millis();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  drawProgressBarDemo(05);
  display.drawString(0, 0, "Booting...");
  display.display();
  delay(500);
  setup_wifi();
  drawProgressBarDemo(55);
  display.drawString(0, 0, "OTA Configulation");
  display.display();
  setup_ota();
  drawProgressBarDemo(75);
  display.drawString(0, 0, "OTA Configulation");
  display.display();
  delay(100);
  display.drawString(0, 10, "OTA Ready");
  display.display();
  delay(1000);
  Serial.println("[INFO] Ready");
  Serial.print("[INFO] IP address: ");
  Serial.println(WiFi.localIP());
  int boot_time = (millis() - startedAt) / 1000;
  while ((boot_time < 60) && (digitalRead(AP_Config) == HIGH))
    //while ((boot_time < 80))
  {
    boot_time = (millis() - startedAt) / 1000;
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 0, "Wifi Local IP");
    display.drawString(64, 10, String(Router_SSID));
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 24, WiFi.localIP().toString());
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 42, "Boot time : " + String(boot_time) + "s");
    display.drawString(64, 52, "Pass PRG to Continue");
    display.display();
    delay(100);
  }
  display.clear();
}

void loop() {
  ArduinoOTA.handle();
  String butt_S = String(digitalRead(AP_Config) ? "OFF" : "ON");
  display.drawString(64, 30, butt_S);
  display.display();
  delay(100);
  display.clear();
}

void setup_wifi() {
  ESP_WiFiManager ESP_wifiManager;
  ESP_wifiManager.setMinimumSignalQuality(-1);
  Serial.println("[INFO] Stored: SSID = " + Router_SSID + "\n[INFO] Pass = " + Router_Pass);
  Serial.print("[INFO] Opening configuration portal. ");
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();
  int j = 0;
  while ((j < 2) && (digitalRead(AP_Config) != LOW)) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      drawProgressBarDemo(j + 5);
      j++ ;
      display.drawString(0, 0, "Booting..");
      display.drawString(0, 10, "Hold Program Button");
      display.drawString(0, 20, "In 2 seconds for Setting");
      display.drawString(0, 30, "New Wifi in Config portal");
      display.drawString(62, 40, String(j) + "s");
      display.display();
    }
  }
  if ((Router_SSID == "")) {
    ESP_wifiManager.setConfigPortalTimeout(50);
    Serial.println("Timeout 50s");
    drawProgressBarDemo(5);
    display.drawString(0, 0, "Booting..");
    display.drawString(0, 10, "Configuration portal -");
    display.drawString(0, 20, "SSID  :" + String(ssid));
    display.drawString(0, 30, "PASS :" + String(password));
    display.drawString(0, 40, "HTTP://192.168.4.1:80");
    display.display();
  } else if ((Router_SSID != "") && (digitalRead(AP_Config) != HIGH)) {
    ESP_wifiManager.setConfigPortalTimeout(50);
    Serial.println("Timeout 50s");
    drawProgressBarDemo(5);
    display.drawString(0, 0, "Booting..");
    display.drawString(0, 10, "Configuration portal -");
    display.drawString(0, 20, "SSID  :" + String(ssid));
    display.drawString(0, 30, "PASS :" + String(password));
    display.drawString(0, 40, "HTTP://192.168.4.1:80");
    display.display();
  } else {
    ESP_wifiManager.setConfigPortalTimeout(1);
    drawProgressBarDemo(5);
    display.drawString(0, 0, "Booting..");
    display.drawString(0, 30, "");
    display.drawString(0, 10, "Conneting to ");
    display.drawString(0, 20, " " + String(Router_SSID));
    display.display();
    Serial.println("Skip");
  }
  wifi_sta_list_t stationList;
  ssid.toUpperCase();
  if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password)) {
    Serial.println("[INFO] Not connected to WiFi but continuing anyway.");
  } else {
    Serial.println("[INFO] WiFi connected...");
    int i = 5;
    while (i <= 10) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= 100) {
        previousMillis = currentMillis;
        drawProgressBarDemo(i);
        i++ ;
        display.display();
      }
    }
  }
  digitalWrite(PIN_LED, LED_OFF);
#define WIFI_CONNECT_TIMEOUT        30000L
#define WHILE_LOOP_DELAY            200L
#define WHILE_LOOP_STEPS            (WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ))
  startedAt = millis();
  while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT ) ) {
    drawProgressBarDemo(5);
    display.drawString(0, 0, "Booting..");
    display.drawString(0, 30, "");
    display.drawString(0, 10, "");
    display.drawString(0, 20, "Conneting to " + String(Router_SSID));
    display.display();
#ifdef ESP32
    WiFi.mode(WIFI_STA);
    WiFi.persistent (true);
    Serial.print("[INFO] Connecting to ");
    Serial.println(Router_SSID);
    WiFi.begin(Router_SSID.c_str(), Router_Pass.c_str());
#endif
    int i = 0;
    String a = ".";
    while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= WHILE_LOOP_DELAY) {
        previousMillis = currentMillis;
        drawProgressBarDemo(i + 5);
        a = a + "." ;
        display.drawString(0, 0, "Booting..");
        display.drawString(0, 30, String(Router_SSID));
        display.drawString(0, 10, "");
        display.drawString(0, 20, "Conneting to" );
        display.drawString(0, 40, a);
        display.display();
      }
    }
    Serial.print("[INFO] After waiting ");
    Serial.print((millis() - startedAt) / 1000);
    Serial.print(" secs more in setup(), connection result is ");
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print("[INFO] connected. Local IP: ");
      Serial.println(WiFi.localIP());
      display.clear();
      display.drawString(0, 0, "Connecting to " + String(ssid) + "... done");
      display.display();
      delay(500);
    }
  }
}

void setup_ota() {
  ArduinoOTA.setHostname("OLED_TTGO_");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("[INFO] Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\n[INFO] End");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[INFO] Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("[INFO] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("[INFO] Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("[INFO] Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("[INFO] Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("[INFO] Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("[INFO] End Failed");
  });
  ArduinoOTA.begin();
}

void drawProgressBarDemo(int counter) {
  display.clear();
  int progress = (counter) % 100;
  display.drawProgressBar(4, 54, 100, 6, progress);
  display.drawString(108, 50, String(progress) + "%");
}
