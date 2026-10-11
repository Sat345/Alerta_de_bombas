//
// Created by sait1 on 03/10/2026.
//

#include "GLOBALES.h"

bool wifi = false;
bool Guardado_microSD = false;
bool envio_GoogleSheets = false;

const int TIEMPO_MEDIO = 1000; // 1000 = 1s
unsigned long tiempo_medio = 0;
const int TIEMPO_CORTO = 500;  // 500 = 0.5s
unsigned long tiempo_corto;

double Irms_1, Irms_2, Irms_3, Irms_4;
