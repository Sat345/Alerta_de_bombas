//
// Created by sait1 on 03/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_WIFI_ESP32_H
#define ALERTA_DE_BOMBAS_WIFI_ESP32_H

#include <Arduino.h>
#include <Preferences.h>

extern Preferences preferences;

extern const char* ssid;
extern const char* password;
extern const char* ntpServer;

void configurarReloj();
String obtenerFechaHora();
void conectar_wifi();
void guardar_wiFi(String s, String p);
void configurar_wifi_manual();

#endif //ALERTA_DE_BOMBAS_WIFI_ESP32_H