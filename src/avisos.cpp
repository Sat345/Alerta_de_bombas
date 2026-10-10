//
// Created by sait1 on 03/10/2026.
//

#include "GLOBALES.h"
#include "avisos.h"
#include "reloj_esp32.h"
#include "wifi_esp32.h"
#include <WiFi.h>

Adafruit_NeoPixel tira = Adafruit_NeoPixel(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


void color_led(int led, uint32_t color) {
    tira.setBrightness(50); // Brillo al 20% (Ahorro de energía)
    tira.setPixelColor(led, color);
    tira.show();
}

void iniciarPantalla() {
    tft.initR(INITR_18BLACKTAB);
    tft.setSPISpeed(4000000);
    tft.setRotation(1); // Modo horizontal (160 de ancho x 128 de alto)
    tft.fillScreen(ST7735_BLACK);

    // Dibujar Estructura Fija Base (Para no redibujarla en el loop y ahorrar procesador)
    // Encabezado
    tft.fillRect(0, 0, tft.width(), 16, ST7735_DARKBLUE);
    tft.drawFastHLine(0, 16, tft.width(), ST7735_BLUE);

    // Separadores de Bombas
    tft.drawFastHLine(0, 70, tft.width(), ST7735_DARKGREY);
    tft.drawFastVLine(80, 16, 88, ST7735_DARKGREY); // Línea vertical central

    // Barra de Estado Inferior
    tft.drawFastHLine(0, 104, tft.width(), ST7735_BLUE);
    tft.fillRect(0, 105, tft.width(), 23, ST7735_BLACK);
}

void actualizarPantalla(double i1, int v1, double i2, int v2, double i3, int v3, double i4, int v4) {
    tft.setTextSize(1);

    // ==========================================
    // 1. ACTUALIZAR ENCABEZADO (Reloj en vivo)
    // ==========================================
    tft.setTextColor(ST7735_WHITE, ST7735_DARKBLUE);
    tft.setCursor(4, 4);
    tft.print("MONITOR BOMBAS");

    // Extraer solo la hora (HH:MM:SS) de la cadena del RTC para el encabezado
    String fechaHoraCompleta = obtenerFechaHoraRTC();
    String soloHora = "00:00:00";
    if(fechaHoraCompleta.length() >= 19) {
        soloHora = fechaHoraCompleta.substring(11, 19);
    }
    tft.setCursor(108, 4);
    tft.print(soloHora);

    // ==========================================
    // 2. CUADRÍCULA DE BOMBAS (Lecturas Eléctricas)
    // ==========================================
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

    // BOMBA 1 (Superior Izquierda)
    tft.setCursor(4, 24); tft.setTextColor(ST7735_CYAN, ST7735_BLACK); tft.print("BOMBA 1");
    tft.setCursor(4, 38); tft.setTextColor(ST7735_WHITE, ST7735_BLACK); tft.printf("I: %.2f A ", i1);
    tft.setCursor(4, 50); tft.printf("V: %d V   ", v1);

    // BOMBA 2 (Superior Derecha)
    tft.setCursor(86, 24); tft.setTextColor(ST7735_CYAN, ST7735_BLACK); tft.print("BOMBA 2");
    tft.setCursor(86, 38); tft.setTextColor(ST7735_WHITE, ST7735_BLACK); tft.printf("I: %.2f A ", i2);
    tft.setCursor(86, 50); tft.printf("V: %d V   ", v2);

    // BOMBA 3 (Inferior Izquierda)
    tft.setCursor(4, 76); tft.setTextColor(ST7735_CYAN, ST7735_BLACK); tft.print("BOMBA 3");
    tft.setCursor(4, 88); tft.setTextColor(ST7735_WHITE, ST7735_BLACK); tft.printf("I: %.2f A ", i3);
    tft.setCursor(4, 100); tft.printf("V: %d V   ", v3);

    // BOMBA 4 (Inferior Derecha)
    tft.setCursor(86, 76); tft.setTextColor(ST7735_CYAN, ST7735_BLACK); tft.print("BOMBA 4");
    tft.setCursor(86, 88); tft.setTextColor(ST7735_WHITE, ST7735_BLACK); tft.printf("I: %.2f A ", i4);
    tft.setCursor(86, 100); tft.printf("V: %d V   ", v4);
    tft.setCursor(4, 110);

    // ==========================================
    // 3. BARRA DE DIAGNÓSTICO INFERIOR (Status)
    // ==========================================

    // --- ESTADO WIFI ---
    if (WiFi.status() == WL_CONNECTED) {
        tft.setTextColor(ST7735_GREEN, ST7735_BLACK); tft.print("WIFI: OK ");
    } else {
        tft.setTextColor(ST7735_RED, ST7735_BLACK); tft.print("WIFI: ER ");
    }

    // --- ESTADO SD ---
    if (Guardado_microSD) {
        tft.setTextColor(ST7735_GREEN, ST7735_BLACK); tft.print("SD: OK ");
    } else {
        tft.setTextColor(ST7735_RED, ST7735_BLACK); tft.print("SD: ER ");
    }

    // --- ESTADO NUBE (GOOGLE) ---
    if (envio_GoogleSheets) {
        tft.setTextColor(ST7735_GREEN, ST7735_BLACK); tft.print("WEB: OK");
    } else {
        tft.setTextColor(ST7735_RED, ST7735_BLACK); tft.print("WEB: ER");
    }
}