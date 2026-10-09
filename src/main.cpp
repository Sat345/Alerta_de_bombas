#include "GLOBALES.h"
#include "wifi_esp32.h"
#include "SD_esp32.h"
#include "avisos.h"

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

void setup() {
    Serial.begin(115200);

    tira.begin();

    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(PIN_CS_SD, OUTPUT);
    digitalWrite(PIN_CS_SD, HIGH);

    iniciarPantalla();
    conectar_wifi();
    pinMode(ALARMA, OUTPUT);

    emon_1.current(Bomba_1, 30.0);
    emon_2.current(Bomba_2, 30.0);
    emon_3.current(Bomba_3, 30.0);
    emon_4.current(Bomba_4, 30.0);

    iniciar_SD();


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
                Serial.print("La fecha es: ");  Serial.println(obtenerFechaHora());
                break;
            case 'W':
                conectar_wifi();
                if (WiFi.status() == WL_CONNECTED) {
                    Serial.println("¡Estas conectado a internet!");
                } else {
                    Serial.println("Se perdio la coneccion");
                }
                break;
            case 'I':
                configurar_wifi_manual();
                break;
            default: ;
        }
    }

    if (millis() - tiempo >= TIEMPO_CORTO) {
        corriente();
        procesarDatos();
        enviarDatosAGoogleSheets();
        actualizarPantalla(Irms_1, voltaje1, Irms_2, voltaje2);
        tiempo = millis();
    }
    delay(100);
}

void corriente() {

    // Realizar 1480 muestras (aprox. 20 ciclos a 60Hz)
    Irms_1 = emon_1.calcIrms(1480);
    Irms_2 = emon_2.calcIrms(1480);
    Irms_3 = emon_3.calcIrms(1480);
    Irms_4 = emon_4.calcIrms(1480);

    voltaje1 = analogRead(Bomba_1);
    voltaje2 = analogRead(Bomba_2);
    voltaje3 = analogRead(Bomba_3);
    voltaje4 = analogRead(Bomba_4);


    bool alarma = false;

    int corriente_limite = 2000;

    int voltajes[4] = {voltaje1, voltaje2, voltaje3, voltaje4};
    for(int i = 0; i < 4; i++) {
        if (voltajes[i] > corriente_limite) {
            color_led(i, tira.Color(255,0,0));
            digitalWrite(ALARMA, HIGH);
            alarma = true;
        } else {
            color_led(i, tira.Color(0,0,255));
        }
    }

    if (!alarma) {
        digitalWrite(ALARMA, LOW);
    }

}

