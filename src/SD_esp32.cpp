//
// Created by sait1 on 03/10/2026.
//
#include "GLOBALES.h"
#include "SD_esp32.h"
#include "wifi_esp32.h"
#include "avisos.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "EmonLib.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"



void datos() {

    Serial.print("\n--------------------------\n");

    Serial.println(obtenerFechaHora());
    Serial.print("Corriente Bomba 1: "); Serial.print(Irms_1); Serial.print(" A, Voltaje: "); Serial.println(voltaje1);
    Serial.print("Corriente Bomba 2: "); Serial.print(Irms_2); Serial.print(" A, Voltaje: "); Serial.println(voltaje2);
    Serial.print("Corriente Bomba 3: "); Serial.print(Irms_3); Serial.print(" A, Voltaje: "); Serial.println(voltaje3);
    Serial.print("Corriente Bomba 4: "); Serial.print(Irms_4); Serial.print(" A, Voltaje: "); Serial.println(voltaje4);
}

void guardar_En_SD() {
    // Abrimos el archivo en modo "APPEND" (añadir al final)


    File archivo = SD.open("/datos.csv", FILE_APPEND);

    if (archivo) {

        archivo.print(obtenerFechaHora()); archivo.print(";");
        archivo.print("Corriente Bomba 1: "); archivo.print(Irms_1); archivo.print(" A, Voltaje: "); archivo.print(voltaje1); archivo.println(";");
        archivo.print("Corriente Bomba 2: "); archivo.print(Irms_2); archivo.print(" A, Voltaje: "); archivo.print(voltaje2); archivo.println(";");
        archivo.print("Corriente Bomba 3: "); archivo.print(Irms_3); archivo.print(" A, Voltaje: "); archivo.print(voltaje3); archivo.println(";");
        archivo.print("Corriente Bomba 4: "); archivo.print(Irms_4); archivo.print(" A, Voltaje: "); archivo.print(voltaje4); archivo.println(";");

        archivo.close();
        Serial.println("Datos guardados en SD.");
    } else {
        Serial.println("Error al abrir el archivo para escribir.");
    }
}

void iniciar_SD() {
    SPI.begin(18, 19, 23, PIN_CS_SD);

    // Forzamos al driver a inicializar en Modo SPI estándar compatible con tarjetas viejas
    if (SD.begin(PIN_CS_SD, SPI, 2000000, "/sd", 5)) {
        tieneSD = true;
        pinMode(PIN_CS_SD, OUTPUT);
        digitalWrite(PIN_CS_SD, HIGH);
        Serial.println("SD_OK: ¡Tarjeta formateada detectada exitosamente!");

        // Tu bloque de creación de archivo datos.csv...
    } else {
        tieneSD = false;
        Serial.println("SD_FALLO: La SD sigue rechazando el montaje.");
    }
}




void procesarDatos() {
    String filaDatos = "";

    filaDatos += obtenerFechaHora() + ";";

    // Armamos la fila CSV
    filaDatos += String(Irms_1) + ";" + String(voltaje1) + ";" +
                 String(Irms_2) + ";" + String(voltaje2) + ";" +
                 String(Irms_3) + ";" + String(voltaje3) + ";" +
                 String(Irms_4) + ";" + String(voltaje4);

    if (tieneSD) {
        // Escenario A: Guardar directamente en la memoria SD
        File archivo = SD.open("/datos.csv", FILE_APPEND);
        if (archivo) {
            archivo.println(filaDatos);
            archivo.close();
            Serial.println("Datos guardados en SD.");
        }
    } else {
        // Escenario B: No hay SD, imprimimos con un prefijo para que el script de la Laptop lo guarde
        Serial.println(filaDatos);
    }
}

const char* url_google_sheets = "https://script.google.com/macros/s/AKfycbxmWH8HOrO1IYxBHTnK_2xRYplw2Dxhkt2r4gjS-U3fnVSfSm2HcaGfbqJQon7YIKhGrQ/exec";

void enviarDatosAGoogleSheets() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure cliente;

        cliente.setInsecure();

        HTTPClient http;

        Serial.println("\nEnviando datos a la nube...");
        http.begin(cliente, url_google_sheets);
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        http.addHeader("Content-Type", "application/json");

        // Creamos el paquete de datos en formato JSON para que Google lo entienda de forma nativa
        String cuerpoJSON = "{";
        cuerpoJSON += "\"fecha_hora\":\"" + obtenerFechaHora() + "\",";
        cuerpoJSON += "\"irms1\":" + String(Irms_1) + ",\"v1\":" + String(voltaje1) + ",";
        cuerpoJSON += "\"irms2\":" + String(Irms_2) + ",\"v2\":" + String(voltaje2) + ",";
        cuerpoJSON += "\"irms3\":" + String(Irms_3) + ",\"v3\":" + String(voltaje3) + ",";
        cuerpoJSON += "\"irms4\":" + String(Irms_4) + ",\"v4\":" + String(voltaje4);
        cuerpoJSON += "}";

        // Realizamos la petición HTTP POST enviando el JSON
        int codigoRespuesta = http.POST(cuerpoJSON);

        if (codigoRespuesta == 200) {
            // 200 significa que Google recibió y procesó el archivo con éxito
            Serial.println("Nube: Datos enviados exitosamente a Google Sheets.");
        } else if (codigoRespuesta > 0) {
            // Si da un código como 403, 404 o 500, imprimimos solo el número del error
            Serial.print("Nube: Error en el servidor de Google. Código HTTP: ");
            Serial.println(codigoRespuesta);
        } else {
            // Errores físicos de conexión (ej. -1 si se cayó el Wi-Fi en pleno envío)
            Serial.print("Nube: Fallo la conexión de red. Código: ");
            Serial.println(codigoRespuesta);
        }


        http.end();
    } else {
        Serial.println("No se puede enviar a la nube: Sin conexión Wi-Fi.");
    }
}
