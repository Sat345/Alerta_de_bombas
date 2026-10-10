//
// Created by sait1 on 03/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_GLOBALES_H
#define ALERTA_DE_BOMBAS_GLOBALES_H

extern bool Guardado_microSD;
extern bool rtcConectado;
extern bool envio_GoogleSheets;

extern unsigned long tiempo;
extern const int TIEMPO_CORTO; // 1000 = 1s

extern double Irms_1, Irms_2, Irms_3, Irms_4;
extern int voltaje1, voltaje2, voltaje3, voltaje4;

#endif //ALERTA_DE_BOMBAS_GLOBALES_H