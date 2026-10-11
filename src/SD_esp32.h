#ifndef ALERTA_DE_BOMBAS_SD_ESP32_H
#define ALERTA_DE_BOMBAS_SD_ESP32_H

#include "SPI.h"
#include "SdFat.h"

extern SdFat sdFatCard;
extern bool tieneSD;
const int PIN_CS_SD = 5;

void guardar_En_SD();
void iniciarSD();
void procesarDatos();
void enviarDatosAGoogleSheets();

#endif //ALERTA_DE_BOMBAS_SD_ESP32_H
