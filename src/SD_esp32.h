//
// Created by sait1 on 03/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_SD_ESP32_H
#define ALERTA_DE_BOMBAS_SD_ESP32_H

const int PIN_CS_SD = 5;

void datos();
void guardar_En_SD();
void iniciar_SD();
void procesarDatos();
void  enviarDatosAGoogleSheets();

#endif //ALERTA_DE_BOMBAS_SD_ESP32_H