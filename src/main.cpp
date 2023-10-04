#include <Arduino.h>
#include "../.pio/libdeps/esp32dev/ADS1X15/ADS1X15.h"
#include "../.pio/libdeps/esp32dev/HX711/src/HX711.h"
#include "Wire.h"
TwoWire adcConnection(0);
ADS1115 ADS(0x48, &adcConnection);

HX711 loadcell;
int dutyCycle = 204;

void setup() {
// PWM signal for ESC to controll motorspeed
    ledcAttachPin(26, 0);
    ledcSetup(0, 50, 16);
    ledcWrite(0, 204);

// Setup for adc measuring the voltage and current passing to/ through the motor
    loadcell.begin(21, 22);

    Serial.begin(115200);
    Serial.println(__FILE__);
    Serial.print("ADS1X15_LIB_VERSION: ");
    Serial.println(ADS1X15_LIB_VERSION);
    adcConnection.begin(17, 16);
    ADS.begin();
    ADS.setGain(0);
// Setup for loadcell
    if (loadcell.is_ready()) {
        loadcell.set_scale();
        Serial.println("Tare... remove any weights from the scale.");
        delay(5000);
        loadcell.tare();
        Serial.println("Tare done...");
        Serial.print("Place a known weight on the scale...");
        delay(5000);
        long reading = loadcell.get_units(10);
        Serial.print("Result: ");
        Serial.println(reading);
    }
    else {
        Serial.println("HX711 not found.");
    }
    delay(1000);


    // motor ramp up sequence, including code making measurements
    while (dutyCycle < 6553) {

        // ADC code

        int16_t val_0 = ADS.readADC(0);
        int16_t val_1 = ADS.readADC(1);
        int16_t val_2 = ADS.readADC(2);
        int16_t val_3 = ADS.readADC(3);

        float f = ADS.toVoltage(2);  // voltage factor

        Serial.print("\tAnalog0: "); Serial.print(val_0); Serial.print('\t'); Serial.println((double) val_0 / 0x7fff * 6.144);
        Serial.print("\tAnalog1: "); Serial.print(val_1); Serial.print('\t'); Serial.println(val_1 * f, 3);
        Serial.print("\tAnalog2: "); Serial.print(val_2); Serial.print('\t'); Serial.println(val_2 * f, 3);
        Serial.print("\tAnalog3: "); Serial.print(val_3); Serial.print('\t'); Serial.println(val_3 * f, 3);
        Serial.println();

        // ESC code
        for (int i = 0; i < 163; ++i) {
            dutyCycle += i;
            delay(1000 / 163);
        }
    }
}


void loop() {

    // ESC code
    for (int i = 0; i < 163; ++i) {
        dutyCycle += i;
    }

    // ADC code

    int16_t val_0 = ADS.readADC(0);
    int16_t val_1 = ADS.readADC(1);
    int16_t val_2 = ADS.readADC(2);
    int16_t val_3 = ADS.readADC(3);

    float f = ADS.toVoltage(2);  // voltage factor

    Serial.print("\tAnalog0: "); Serial.print(val_0); Serial.print('\t'); Serial.println((double) val_0 / 0x7fff * 6.144);
    Serial.print("\tAnalog1: "); Serial.print(val_1); Serial.print('\t'); Serial.println(val_1 * f, 3);
    Serial.print("\tAnalog2: "); Serial.print(val_2); Serial.print('\t'); Serial.println(val_2 * f, 3);
    Serial.print("\tAnalog3: "); Serial.print(val_3); Serial.print('\t'); Serial.println(val_3 * f, 3);
    Serial.println();

    delay(1000);
}