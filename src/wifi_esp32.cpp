//
// Created by sait1 on 03/10/2026.
//

#include "wifi_esp32.h"

#include "time.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

Preferences preferences;

const char* ssid = "Matamoros";
const char* password = "Mat#1167";
const char* ntpServer = "pool.ntp.org";

void configurarReloj() {
    configTime(-21600, 0, ntpServer);
}

String obtenerFechaHora() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){

        Serial.println("Error al obtener tiempo");
    }
    char buffer[30];
    // Formato: Día/Mes/Año Hora:Min
    strftime(buffer, 30, "%d/%m/%Y %H:%M", &timeinfo);
    return String(buffer);
}

void conectar_wifi() {


    preferences.begin("wifi-config", true);

    String memorizadoSSID = preferences.getString("ssid", ssid);
    String memorizadoPass = preferences.getString("pass", password);
    preferences.end();

    Serial.print("Intentando conectar a: ");
    Serial.println(memorizadoSSID);

    WiFi.setTxPower(WIFI_POWER_8_5dBm); // Reduce potencia = menos pico de corriente
    WiFi.begin(memorizadoSSID.c_str(), memorizadoPass.c_str());

    int i=0;
    unsigned long tiempoInicioWiFi = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - tiempoInicioWiFi < 12000) {
        delay(500);
        Serial.print(".");
    }

    Serial.print("Estado de WiFi: ");
    switch (WiFi.status()) {
        case WL_IDLE_STATUS:     Serial.println("Inactivo"); break;
        case WL_NO_SSID_AVAIL:   Serial.println("Red no encontrada (Revisa 2.4GHz)"); break;
        case WL_CONNECT_FAILED:  Serial.println("Contraseña incorrecta"); break;
        case WL_DISCONNECTED:    Serial.println("Desconectado"); break;
        case WL_CONNECTED:       Serial.println("¡CONECTADO!"); break;
        default:                 Serial.println("Buscando..."); break;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConectado! IP: " + WiFi.localIP().toString());
        configurarReloj();
    } else {
        Serial.println("No se encontro WiFi. Trabajando en modo OFFLINE.");
    }
}

void guardar_wiFi(String s, String p) {
    preferences.begin("wifi-config", false); // Abre el "archivo" wifi-config
    preferences.putString("ssid", s);        // Guarda el nombre de red
    preferences.putString("pass", p);        // Guarda la contraseña
    preferences.end();                       // Cierra y asegura los datos
    Serial.println("Datos guardados en memoria permanente.");
}

void configurar_wifi_manual() {
    while(Serial.available() > 0) Serial.read();

    Serial.println("\n--- CONFIGURACIÓN DE NUEVA RED ---");
    Serial.println("Escribe el nombre de la red (SSID) y presiona Enter:");

    //Espera real hasta que el usuario escriba algo
    while (!Serial.available()) { delay(100); }
    String tempSSID = Serial.readStringUntil('\n');
    tempSSID.trim();

    Serial.println("SSID recibido: " + tempSSID); // Confirmación
    Serial.println("Escribe la contraseña y presiona Enter:");

    delay(500);
    while(Serial.available() > 0) Serial.read();

    //Volvemos a esperar para la contraseña
    while (!Serial.available()) { delay(9000); }
    String tempPass = Serial.readStringUntil('\n');
    tempPass.trim();

    Serial.println("Cotraseña recibida: " + tempPass);
    Serial.print("Intentando conectar a: "); Serial.println(tempSSID);
    WiFi.disconnect(); // Corta la conexión actual
    WiFi.begin(tempSSID.c_str(), tempPass.c_str());

    Serial.print("Validando");
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n¡Conexión Exitosa!");
        guardar_wiFi(tempSSID, tempPass);
        Serial.println("IP: " + WiFi.localIP().toString());
        configurarReloj();
    } else {
        Serial.println("Error: Datos incorrectos. No se guardó nada.");
        conectar_wifi();
    }
}
