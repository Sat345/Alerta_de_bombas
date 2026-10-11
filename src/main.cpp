#include "GLOBALES.h"
#include "wifi_esp32.h"
#include "SD_esp32.h"
#include "avisos.h"
#include "reloj_esp32.h"

#include <WiFi.h>
#include <Arduino.h>
#include <Wire.h>
#include "EmonLib.h"

EnergyMonitor emon_1;
EnergyMonitor emon_2;
EnergyMonitor emon_3;
EnergyMonitor emon_4;

const int Bomba_1 = 34;
const int Bomba_2 = 35;
const int Bomba_3 = 36;
const int Bomba_4 = 39;

void corriente();
void datos();

void setup() {
    Serial.begin(115200);

    tira.begin();

    pinMode(PIN_CS_SD, OUTPUT);
    digitalWrite(PIN_CS_SD, HIGH);
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    pinMode(ALARMA, OUTPUT);

    emon_1.current(Bomba_1, 30.0);
    emon_2.current(Bomba_2, 30.0);
    emon_3.current(Bomba_3, 30.0);
    emon_4.current(Bomba_4, 30.0);

    iniciarPantalla();
    conectarWifi();
    iniciarRTC();
    iniciarSD();


}

void loop() {
    if (Serial.available() > 0) {
        char comando = Serial.read(); //leer
        switch (comando) {
            case 'V':
                Serial.println("V: Visualizar las opciones");
                Serial.println("F: Mostrar fecha");
                Serial.println("W: Conectar wifi");
                Serial.println("I: Cambiar nombre y contraseña de el wifi");
                break;
            case 'F':
                Serial.print("La fecha es: ");  Serial.println(obtenerFechaHoraWifi());
                break;
            case 'W':
                conectarWifi();
                if (WiFi.status() == WL_CONNECTED) {
                    Serial.println("¡Estas conectado a internet!");
                } else {
                    Serial.println("Se perdio la coneccion");
                }
                break;
            case 'I':
                configurarWifiManual();
                break;
            default: ;
        }
    }

    if (millis() - tiempo_medio >= TIEMPO_MEDIO && prendidas ) {
        procesarDatos();
        enviarDatosAGoogleSheets();

        tiempo_medio = millis();
    }

    if (millis() - tiempo_corto >= TIEMPO_CORTO) {
        corriente();
        actualizarPantalla();

        tiempo_corto = millis();
    }

    delay(100);
}

void corriente() {

    Irms_1 = emon_1.calcIrms(1480);
    Irms_2 = emon_2.calcIrms(1480);
    Irms_3 = emon_3.calcIrms(1480);
    Irms_4 = emon_4.calcIrms(1480);

    bool alarma = false;
    prendidas = false;

    int corriente_maxima = 20;
    int corriente_minima = 5;

    double corriente[4] = {Irms_1, Irms_2, Irms_3, Irms_4};
    for(int i = 0; i < 4; i++) {
        if (corriente[i] > corriente_maxima) {
            color_led(i, tira.Color(255,0,0));
            digitalWrite(ALARMA, HIGH);
            alarma = true;
            prendidas = true;
        } else if (corriente[i] < corriente_minima) {
            color_led(i, tira.Color(0,255,255));
        }else {
            color_led(i, tira.Color(0,255,0));
            prendidas = true;
        }
    }

    if (!alarma) {
        digitalWrite(ALARMA, LOW);
    }

}

void datos() {

    Serial.print("\n--------------------------\n");

    Serial.println(obtenerFechaHoraRTC());
    Serial.print("Corriente Bomba 1: "); Serial.println(Irms_1);
    Serial.print("Corriente Bomba 2: "); Serial.println(Irms_2);
    Serial.print("Corriente Bomba 3: "); Serial.println(Irms_3);
    Serial.print("Corriente Bomba 4: "); Serial.println(Irms_4);
}