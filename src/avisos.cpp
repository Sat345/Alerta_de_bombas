//
// Created by sait1 on 03/10/2026.
//

#include "avisos.h"

Adafruit_NeoPixel tira = Adafruit_NeoPixel(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

void color_led(int led, uint32_t color) {
    tira.setBrightness(50); // Brillo al 20% (Ahorro de energía)
    tira.setPixelColor(led, color);
    tira.show();
}