//
// Created by sait1 on 03/10/2026.
//

#ifndef ALERTA_DE_BOMBAS_AVISOS_H
#define ALERTA_DE_BOMBAS_AVISOS_H

#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>    // Librería de gráficos
#include <Adafruit_ST7735.h> // Librería del driver de la pantalla
#include <SPI.h>

#define TFT_CS         27
#define TFT_RST        26
#define TFT_DC         15

// Definición de Colores adicionales para una interfaz moderna
#define ST7735_DARKGREY 0x39E7
#define ST7735_DARKBLUE 0x0010

const int ALARMA = 13;

const int PIN_LEDS = 4;
const int NUM_LEDS = 8;

extern Adafruit_NeoPixel tira;
extern Adafruit_ST7735 tft;

void color_led(int led, uint32_t color);
void iniciarPantalla();
void actualizarPantalla(double i1, int v1, double i2, int v2, double i3, int v3, double i4, int v4);
void prueba();


#endif //ALERTA_DE_BOMBAS_AVISOS_H