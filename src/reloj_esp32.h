//
// Created by sait1 on 09/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_RELOJ_ESP32_H
#define ALERTA_DE_BOMBAS_RELOJ_ESP32_H

#include <RTClib.h>

extern RTC_DS3231 rtc;

void iniciar_RTC();
String obtenerFechaHoraRTC();
void sincronizarRelojConInternet();

#endif //ALERTA_DE_BOMBAS_RELOJ_ESP32_H