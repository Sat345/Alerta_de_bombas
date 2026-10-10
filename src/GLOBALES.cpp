//
// Created by sait1 on 03/10/2026.
//

#include "GLOBALES.h"

bool Guardado_microSD = false;
bool rtcConectado = false;
bool envio_GoogleSheets = false;

unsigned long tiempo = 0;
const int TIEMPO_CORTO = 1000; // 1000 = 1s

double Irms_1, Irms_2, Irms_3, Irms_4;
int voltaje1, voltaje2, voltaje3, voltaje4;
