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

static const uint16_t C_ROJO_OSC = 0x7800;

enum { EST_APAGADA = 0, EST_ENCENDIDA = 1, EST_CORTO = 2 };

static int8_t estadoPrev[4] = {-1, -1, -1, -1};
static int8_t alarmaPrev = -1;
static int8_t wifiPrev = -1, sdPrev = -1, webPrev = -1;

void color_led(int led, uint32_t color) {
    tira.setBrightness(50); // Brillo al 20% (Ahorro de energía)
    tira.setPixelColor(led, color);
    tira.show();
}

static void insignia(int x, const char* texto, bool ok) {
    uint16_t fondo = ok ? ST7735_GREEN : ST7735_RED;
    uint16_t letra = ok ? ST7735_BLACK : ST7735_WHITE;
    int anchoTexto = strlen(texto) * 6;

    tft.fillRoundRect(x, 108, 48, 16, 3, fondo);
    tft.setTextSize(1);
    tft.setTextColor(letra, fondo);
    tft.setCursor(x + (48 - anchoTexto) / 2, 112);
    tft.print(texto);
}

void iniciarPantalla() {
    tft.initR(INITR_18BLACKTAB);
    tft.setSPISpeed(4000000);
    tft.setRotation(1); // 160 x 128
    tft.fillScreen(ST7735_BLACK);

    // Separadores fijos
    tft.drawFastHLine(0, 60, tft.width(), ST7735_DARKGREY);       // entre filas de bombas
    tft.drawFastVLine(80, 17, 87, ST7735_DARKGREY);               // entre columnas
    tft.drawFastHLine(0, 104, tft.width(), ST7735_BLUE);          // sobre la barra de estado

    // Forzar el redibujo completo en la siguiente actualización
    for (int i = 0; i < 4; i++) estadoPrev[i] = -1;
    alarmaPrev = wifiPrev = sdPrev = webPrev = -1;
}

void actualizarPantalla() {
    const double corriente[4] = {Irms_1, Irms_2, Irms_3, Irms_4};
    const int16_t colX[2] = {4, 86};     // texto
    const int16_t fillX[2] = {0, 81};    // fondo del recuadro
    const int16_t filaY[2] = {17, 61};

    // ---- Calcular estado de cada bomba ----
    int8_t estado[4];
    bool hayAlarma = false;
    for (int i = 0; i < 4; i++) {
        if (corriente[i] > CORRIENTE_MAXIMA)      { estado[i] = EST_CORTO; hayAlarma = true; }
        else if (corriente[i] < CORRIENTE_MINIMA) { estado[i] = EST_APAGADA; }
        else                                      { estado[i] = EST_ENCENDIDA; }
    }

    // ==========================================
    // 1. ENCABEZADO
    // ==========================================
    uint16_t fondoHeader = hayAlarma ? C_ROJO_OSC : ST7735_DARKBLUE;

    if (alarmaPrev != (int8_t)hayAlarma) {
        alarmaPrev = hayAlarma;
        tft.fillRect(0, 0, tft.width(), 16, fondoHeader);
        tft.drawFastHLine(0, 16, tft.width(), hayAlarma ? ST7735_RED : ST7735_BLUE);
        tft.setTextSize(1);
        tft.setTextColor(ST7735_WHITE, fondoHeader);
        tft.setCursor(4, 4);
        tft.print(hayAlarma ? "!! ALARMA !!" : "MONITOR BOMBAS");
    }

    String fechaHoraCompleta = obtenerFechaHoraRTC();
    String soloHora = "--:--:--";
    if (fechaHoraCompleta.length() >= 19) {
        soloHora = fechaHoraCompleta.substring(11, 19);
    }
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE, fondoHeader);
    tft.setCursor(108, 4);
    tft.print(soloHora);

    // ==========================================
    // 2. CUADRÍCULA DE BOMBAS
    // ==========================================
    for (int i = 0; i < 4; i++) {
        int col = i % 2;
        int fila = i / 2;
        int16_t cx = colX[col];
        int16_t cy = filaY[fila];

        uint16_t colorTexto, colorFondo;
        const char* textoEstado;
        switch (estado[i]) {
            case EST_CORTO:
                colorTexto = ST7735_WHITE; colorFondo = ST7735_RED;   textoEstado = "CORTO CIRC."; break;
            case EST_APAGADA:
                colorTexto = ST7735_CYAN;  colorFondo = ST7735_BLACK; textoEstado = "APAGADA";     break;
            default:
                colorTexto = ST7735_GREEN; colorFondo = ST7735_BLACK; textoEstado = "ENCENDIDA";   break;
        }

        // Si cambió el estado, se redibuja todo el recuadro
        if (estadoPrev[i] != estado[i]) {
            estadoPrev[i] = estado[i];
            tft.fillRect(fillX[col], cy, 79, 43, colorFondo);

            tft.setTextSize(1);
            tft.setTextColor(colorTexto, colorFondo);
            tft.setCursor(cx, cy + 4);
            tft.print("BOMBA " + String(i + 1));

            tft.setCursor(cx, cy + 32);
            tft.print(textoEstado);
        }

        // Valor de corriente (se reescribe siempre, con relleno para no dejar restos)
        tft.setTextSize(2);
        tft.setTextColor(colorTexto, colorFondo);
        tft.setCursor(cx, cy + 15);
        if (corriente[i] >= 100.0) tft.printf("%5.1f", corriente[i]);
        else                       tft.printf("%5.2f", corriente[i]);

        tft.setTextSize(1);
        tft.setCursor(cx + 62, cy + 23);
        tft.print("A");
    }

    // ==========================================
    // 3. BARRA DE ESTADO (solo redibuja si cambia)
    // ==========================================
    int8_t wifiOk = (WiFi.status() == WL_CONNECTED);
    int8_t sdOk   = Guardado_microSD ? 1 : 0;
    int8_t webOk  = envio_GoogleSheets ? 1 : 0;

    if (wifiOk != wifiPrev) {
        wifiPrev = wifiOk;
        insignia(4, "WIFI", wifiOk);
        color_led(7, wifiOk ? tira.Color(255, 0, 255) : tira.Color(255, 0, 0));
    }
    if (sdOk != sdPrev) {
        sdPrev = sdOk;
        insignia(56, "SD", sdOk);
        color_led(6, sdOk ? tira.Color(255, 122, 0) : tira.Color(255, 0, 0));
    }
    if (webOk != webPrev) {
        webPrev = webOk;
        insignia(108, "WEB", webOk);
        color_led(5, webOk ? tira.Color(255, 255, 0) : tira.Color(255, 0, 0));
    }
}