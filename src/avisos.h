//
// Created by sait1 on 03/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_AVISOS_H
#define ALERTA_DE_BOMBAS_AVISOS_H

#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>    // Librería de gráficos
#include <Adafruit_ST7735.h> // Librería del driver de la pantalla
#include <SPI.h>

#define TFT_CS         22
#define TFT_RST        21
#define TFT_DC         15


const int PIN_CS_SD = 5;
const int ALARMA = 13;

const int PIN_LEDS = 4;
const int NUM_LEDS = 8;

extern Adafruit_NeoPixel tira;
extern Adafruit_ST7735 tft;

void color_led(int led, uint32_t color);
void iniciarPantalla();
void actualizarPantalla(double i1, int v1, double i2, int v2);
void prueba();


#endif //ALERTA_DE_BOMBAS_AVISOS_H