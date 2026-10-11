//
// Created by sait1 on 03/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_GLOBALES_H
#define ALERTA_DE_BOMBAS_GLOBALES_H

extern bool wifi;
extern bool Guardado_microSD;
extern bool envio_GoogleSheets;

extern const int TIEMPO_MEDIO; // 1000 = 1s
extern unsigned long tiempo_medio;
extern const int TIEMPO_CORTO;
extern unsigned long tiempo_corto;

extern double Irms_1, Irms_2, Irms_3, Irms_4;

#endif //ALERTA_DE_BOMBAS_GLOBALES_H