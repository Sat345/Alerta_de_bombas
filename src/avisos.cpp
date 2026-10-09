//
// Created by sait1 on 03/10/2026.
//

#include "avisos.h"

Adafruit_NeoPixel tira = Adafruit_NeoPixel(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


void color_led(int led, uint32_t color) {
    tira.setBrightness(50); // Brillo al 20% (Ahorro de energía)
    tira.setPixelColor(led, color);
    tira.show();
}

void iniciarPantalla() {
    // PRUEBA 1: Cambiar BLACKTAB por REDTAB (Es el más común en pantallas de fondo rojo)
    //tft.initR(INITR_REDTAB);

    // Si sigue en blanco, compila probando esta otra línea:
    tft.initR(INITR_18BLACKTAB);

    tft.setSPISpeed(4000000);  // después de initR

    tft.setRotation(1);
    tft.fillScreen(ST7735_BLACK);

    tft.setCursor(10, 10);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextSize(1);
    tft.println("MONITOR DE BOMBAS");
    tft.drawFastHLine(0, 22, tft.width(), ST7735_WHITE);
}


void actualizarPantalla(double i1, int v1, double i2, int v2) {
    // Es mejor sobreescribir texto con un fondo de color para evitar el parpadeo ("flicker")
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setTextSize(1);

    // Imprimir datos de Bomba 1
    tft.setCursor(5, 35);
    tft.printf("Bomba 1: %.2f A  | V: %d  ", i1, v1);

    // Imprimir datos de Bomba 2
    tft.setCursor(5, 50);
    tft.printf("Bomba 2: %.2f A  | V: %d  ", i2, v2);

    // Línea de estado inferior
    tft.setCursor(5, 110);
    tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
    tft.print("Estado: Sistema OK    ");
}

void prueba() {
    tft.fillScreen(ST7735_RED);   delay(1000);
    tft.fillScreen(ST7735_GREEN); delay(1000);
    tft.fillScreen(ST7735_BLUE);  delay(1000);
}