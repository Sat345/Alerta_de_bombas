//
// Created by sait1 on 03/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_WIFI_ESP32_H
#define ALERTA_DE_BOMBAS_WIFI_ESP32_H

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

extern Preferences preferences;

extern const char* ssid;
extern const char* password;
extern const char* ntpServer;

void configurarReloj();
String obtenerFechaHoraWifi();
void conectarWifi();
void guardarWiFi(String s, String p);
void configurarWifiManual();

#endif //ALERTA_DE_BOMBAS_WIFI_ESP32_H