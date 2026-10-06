//
// Created by sait1 on 03/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_AVISOS_H
#define ALERTA_DE_BOMBAS_AVISOS_H

#include <Adafruit_NeoPixel.h>

const int ALARMA = 13;

const int PIN_LEDS = 4;
const int NUM_LEDS = 8;

extern Adafruit_NeoPixel tira;

void color_led(int led, uint32_t color);


#endif //ALERTA_DE_BOMBAS_AVISOS_H