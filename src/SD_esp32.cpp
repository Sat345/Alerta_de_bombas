//
// Created by sait1 on 03/10/2026.
//
#include "GLOBALES.h"
#include "SD_esp32.h"
#include "wifi_esp32.h"
#include "avisos.h"
#include "reloj_esp32.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "EmonLib.h"
#include "SPI.h"
#include "SdFat.h" // Usamos únicamente SdFat

SdFat sdFatCard;
bool tieneSD = false;

void iniciarSD() {
    Guardado_microSD = false;
    tieneSD = false;

    pinMode(PIN_CS_SD, OUTPUT);
    digitalWrite(PIN_CS_SD, HIGH);

    SdSpiConfig spiConfig(PIN_CS_SD, SHARED_SPI, SD_SCK_MHZ(4));

    if (sdFatCard.begin(spiConfig)) {
        tieneSD = true;
        pinMode(PIN_CS_SD, OUTPUT);
        digitalWrite(PIN_CS_SD, HIGH);
        Serial.println("SD_OK: ¡Tarjeta Nokia de 2GB montada con éxito!");

        File32 archivoLocal;

        // Usamos la bandera O_CREAT y O_WRITE de forma directa para obligar la apertura
        if (archivoLocal.open("datos.csv", O_RDWR | O_CREAT | O_AT_END)) {
            // Si el archivo está completamente vacío (tamaño 0), escribimos las cabeceras
            Guardado_microSD = true;
            if (archivoLocal.fileSize() == 0) {
                archivoLocal.println("Fecha_Hora,Irms_1,Irms_2,Irms_3,Irms_4");
                Serial.println("¡Archivo datos.csv creado con cabeceras con éxito!");
            } else {
                Serial.println("Archivo datos.csv ya existía con datos.");
            }
            archivoLocal.close();
        } else {
            Serial.println("Error crítico al intentar crear el archivo base.");
        }
    } else {
        sdFatCard.initErrorPrint(&Serial);
    }
}

void procesarDatos() {
    Guardado_microSD = false;
    String fecha = obtenerFechaHoraRTC();
    if (fecha.length() < 2 || fecha.indexOf("Error") >= 0) {
        fecha = "00/00/0000 00:00:00"; // Estampa limpia para evitar romper el JSON de Google
        color_led(4, tira.Color(255,0,0));
    }else {
        color_led(4, tira.Color(0,0,255));
    }

    String filaDatos = fecha + ",";
    filaDatos += String(Irms_1) + "," +
                 String(Irms_2) + "," +
                 String(Irms_3) + "," +
                 String(Irms_4);

    if (tieneSD) {
        File32 archivoLocal;
        // Apertura en modo Lectura/Escritura apuntando al final del archivo
        if (archivoLocal.open("datos.csv", O_RDWR | O_CREAT | O_AT_END)) {
            Guardado_microSD = true;
            archivoLocal.println(filaDatos);
            archivoLocal.close();
            Serial.println("Datos guardados en SD (SdFat).");
        } else {
            Serial.println("Error al abrir archivo con SdFat en el loop.");
            tieneSD = false;
        }
    } else {
        iniciarSD();
        Serial.println(filaDatos);
    }
}



const char* url_google_sheets = "https://script.google.com/macros/s/AKfycbyRAa_KPgM25ii4y9FH_3-m70nC8NbZWWQw7hYDXBO8roC-FOzlJt1uSkv6aw5fzvihFQ/exec";

void enviarDatosAGoogleSheets() {
    envio_GoogleSheets = false;

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("No se puede enviar a la nube: Sin conexión Wi-Fi.");
        return;
    }

    // 1. Armar el JSON
    String fechaLimpia = obtenerFechaHoraRTC();
    fechaLimpia.trim();

    char jsonBuffer[200];
    snprintf(jsonBuffer, sizeof(jsonBuffer),
             "{\"fecha_hora\":\"%s\",\"irms1\":%.2f,\"irms2\":%.2f,\"irms3\":%.2f,\"irms4\":%.2f}",
             fechaLimpia.c_str(), Irms_1, Irms_2, Irms_3, Irms_4);

    // 2. Conexión
    WiFiClientSecure cliente;
    cliente.setInsecure();

    HTTPClient http;
    http.setConnectTimeout(5000);
    http.setTimeout(8000);
    http.begin(cliente, url_google_sheets);
    http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
    http.addHeader("Content-Type", "application/json");

    // 3. Un solo POST (ya no se sigue la redirección)
    unsigned long t0 = millis();
    int codigo = http.POST(jsonBuffer);
    unsigned long dt = millis() - t0;
    http.end();

    Serial.printf("Nube: HTTP %d en %lu ms\n", codigo, dt);

    // 4. 200, 301, 302 o 303 significan que el script se ejecutó
    if (codigo == 200 || codigo == 301 || codigo == 302 || codigo == 303) {
        envio_GoogleSheets = true;
        Serial.println("Nube: Datos enviados a Google Sheets.");
    } else if (codigo > 0) {
        Serial.printf("Nube: Error del servidor. Código HTTP: %d\n", codigo);
    } else {
        Serial.printf("Nube: Falló la conexión. Código: %d\n", codigo);
    }
}
