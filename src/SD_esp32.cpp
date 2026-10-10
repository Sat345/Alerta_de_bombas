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

void iniciar_SD() {
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
                archivoLocal.println("Fecha_Hora;Irms_1;Voltaje_1;Irms_2;Voltaje_2;Irms_3;Voltaje_3;Irms_4;Voltaje_4");
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
    }

    String filaDatos = fecha + ";";
    filaDatos += String(Irms_1) + ";" + String(voltaje1) + ";" +
                 String(Irms_2) + ";" + String(voltaje2) + ";" +
                 String(Irms_3) + ";" + String(voltaje3) + ";" +
                 String(Irms_4) + ";" + String(voltaje4);

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
        iniciar_SD();
        Serial.println(filaDatos);
    }
}



const char* url_google_sheets = "https://script.google.com/macros/s/AKfycbyfhKNxa6Bn3fiB2GkTO5Jl9NjbE8svH3BOmQu4xuVTU8g1-6WpV25NAqUYnFmpp-CEtA/exec";

void enviarDatosAGoogleSheets() {
    envio_GoogleSheets = false;
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure cliente;
        cliente.setInsecure();

        HTTPClient http;
        Serial.println("\nEnviando datos a la nube...");

        http.begin(cliente, url_google_sheets);
        http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
        http.addHeader("Content-Type", "application/json");

        // 1. Obtener la fecha del RTC y limpiarla de cualquier carácter extraño de control
        String fechaLimpia = obtenerFechaHoraRTC();
        fechaLimpia.trim(); // .trim() elimina automáticamente cualquier espacio, \r o \n oculto al inicio o final

        // 2. Construcción segura del JSON usando el formateador nativo para evitar fallos de comillas manuales
        char jsonBuffer[300];
        snprintf(jsonBuffer, sizeof(jsonBuffer),
                 "{\"fecha_hora\":\"%s\",\"irms1\":%.2f,\"v1\":%d,\"irms2\":%.2f,\"v2\":%d,\"irms3\":%.2f,\"v3\":%d,\"irms4\":%.2f,\"v4\":%d}",
                 fechaLimpia.c_str(),
                 Irms_1, voltaje1,
                 Irms_2, voltaje2,
                 Irms_3, voltaje3,
                 Irms_4, voltaje4);

        String cuerpoJSON = String(jsonBuffer);

        // 3. Enviamos el POST inicial
        int codigoRespuesta = http.POST(cuerpoJSON);

        // 4. Manejo de redirección estándar de Google (301 / 302)
        if (codigoRespuesta == 301 || codigoRespuesta == 302) {
            String nuevaURL = http.getLocation();
            http.end();

            http.begin(cliente, nuevaURL);
            codigoRespuesta = http.GET();
        }

        // 5. Evaluación de la respuesta del servidor
        if (codigoRespuesta == 200) {
            Serial.println("Nube: Datos enviados exitosamente a Google Sheets.");
            String respuestaEnTexto = http.getString();
            Serial.print("Respuesta de Google: "); Serial.println(respuestaEnTexto);
            envio_GoogleSheets = true;
        } else if (codigoRespuesta > 0) {
            Serial.print("Nube: Error en el servidor de Google. Código HTTP: ");
            Serial.println(codigoRespuesta);
        } else {
            Serial.print("Nube: Falló la conexión de red. Código: ");
            Serial.println(codigoRespuesta);
        }

        http.end();
    } else {
        Serial.println("No se puede enviar a la nube: Sin conexión Wi-Fi.");
    }
}

