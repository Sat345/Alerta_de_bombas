//
// Created by sait1 on 09/10/2026.
//

#include "reloj_esp32.h"
#include <WiFi.h>
#include <Wire.h>

#include "wifi_esp32.h"

RTC_DS3231 rtc;

void iniciar_RTC() {
    // Inicializamos el bus I2C en los pines nativos 21 y 22
    Wire.begin(21, 22);

    if (!rtc.begin()) {
        Serial.println("RTC_FALLO: No se encontró el módulo HW-111 (DS3231).");
        return;
    }

    // SOLUCIÓN 1: Si el oscilador interno del chip está apagado por alguna razón, lo despertamos
    // Esto obliga a los segundos a empezar a correr físicamente en el silicio
    if (rtc.lostPower()) {
        Serial.println("RTC_AVISO: ¡El módulo HW-111 detectó pérdida de energía total!");
    }

    // SOLUCIÓN 2: Sacamos la sincronización del 'if' para que se ejecute SIEMPRE al arrancar.
    // De esta forma, cada que prendas tu circuito, se actualizará con la hora exacta actual de red.
    Serial.println("Actualizando hora del HW-111...");
    if (WiFi.status() == WL_CONNECTED) {
        sincronizarRelojConInternet();
    } else {
        // Si no hay internet, solo le ponemos una hora base para que no empiece en ceros,
        // pero el chip seguirá contando el tiempo desde donde se quedó gracias a su batería.
        Serial.println("RTC_AVISO: Sin Wi-Fi. El chip mantendrá su hora interna respaldada por la batería.");
    }

    Serial.println("RTC_OK: Módulo HW-111 inicializado y corriendo.");
}

String obtenerFechaHoraRTC() {
    DateTime ahora = rtc.now();
    char buffer[30];

    snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d",
             ahora.day(), ahora.month(), ahora.year(),
             ahora.hour(), ahora.minute(), ahora.second());

    return String(buffer);
}



void sincronizarRelojConInternet() {
    if (WiFi.status() == WL_CONNECTED) {
        struct tm timeinfo;

        // Intentamos obtener la hora del servidor NTP de Espressif
        if (getLocalTime(&timeinfo)) {
            // El struct tm cuenta los años desde 1900, por lo que sumamos 1900
            // El struct tm cuenta los meses de 0 a 11, por lo que sumamos 1
            rtc.adjust(DateTime(
                timeinfo.tm_year + 1900,
                timeinfo.tm_mon + 1,
                timeinfo.tm_mday,
                timeinfo.tm_hour,
                timeinfo.tm_min,
                timeinfo.tm_sec
            ));
            Serial.println("RTC_OK: ¡Reloj HW-111 sincronizado exitosamente con la hora de Internet!");
        } else {
            Serial.println("RTC_AVISO: Hubo Wi-Fi pero falló el servidor NTP. No se actualizó el RTC.");
        }
    }
    else {
        // Si no hay Wi-Fi al arrancar, usamos la hora en la que diste clic a compilar en CLion
        Serial.println("RTC_AVISO: Sin Wi-Fi. Usando estampa de tiempo de compilación...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

