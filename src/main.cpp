#include <Arduino.h>
#include "../.pio/libdeps/esp32dev/ADS1X15/ADS1X15.h"
#include "../.pio/libdeps/esp32dev/HX711/src/HX711.h"
#include "Wire.h"
#include "LoadcellController.h"

#define BUTTON_PIN 15
#define ESC_CONTROLL_PIN 26
#define ESC_CONTROLL_CHANNEL 0
#define ADC_CLOCK_PIN 16
#define ADC_DATA_PIN 17
#define LOADCELL_DATA_PIN 22
#define LOADCELL_CLOCK_PIN 21

TwoWire adcConnection(0);
ADS1115 ADS(0x48, &adcConnection);

HX711 scale;
LoadcellController controller(&scale);

int dutyCycle = 204;


void setup() {
    Serial.begin(115200);

// PWM signal for ESC to controll motorspeed
    ledcAttachPin(ESC_CONTROLL_PIN, ESC_CONTROLL_CHANNEL);
    ledcSetup(ESC_CONTROLL_CHANNEL, 50, 16);
    ledcWrite(ESC_CONTROLL_CHANNEL, 204);

// Setup for adc measuring the voltage and current passing to/ through the motor

    Serial.println(__FILE__);
    Serial.print("ADS1X15_LIB_VERSION: ");
    Serial.println(ADS1X15_LIB_VERSION);
    adcConnection.begin(ADC_DATA_PIN, ADC_CLOCK_PIN);
    ADS.begin();
    ADS.setGain(0);

// Setup for loadcell

    scale.begin(LOADCELL_DATA_PIN, LOADCELL_CLOCK_PIN);

    if (scale.is_ready()) {
        scale.set_scale();
    } else {
    Serial.println("HX711 not found.");
    }

    // motor ramp up sequence, including code making measurements
    while (dutyCycle < 6553) {

        // ADC code

        int16_t val_0 = ADS.readADC(0);
        int16_t val_1 = ADS.readADC(1);
        int16_t val_2 = ADS.readADC(2);
        int16_t val_3 = ADS.readADC(3);

        float f = ADS.toVoltage(2);  // voltage factor

        // prints out the values of the adc pins
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
    /*for (int i = 0; i < 163; ++i) {
        dutyCycle += i;
    }

    // ADC code

    int16_t val_0 = ADS.readADC(0);
    int16_t val_1 = ADS.readADC(1);
    int16_t val_2 = ADS.readADC(2);
    int16_t val_3 = ADS.readADC(3);

    float f = ADS.toVoltage(2);  // voltage factor

    // prints out the values of the adc pins
    Serial.print("\tAnalog0: "); Serial.print(val_0); Serial.print('\t'); Serial.println((double) val_0 / 0x7fff * 6.144);
    Serial.print("\tAnalog1: "); Serial.print(val_1); Serial.print('\t'); Serial.println(val_1 * f, 3);
    Serial.print("\tAnalog2: "); Serial.print(val_2); Serial.print('\t'); Serial.println(val_2 * f, 3);
    Serial.print("\tAnalog3: "); Serial.print(val_3); Serial.print('\t'); Serial.println(val_3 * f, 3);
    Serial.print("read: ");
    Serial.println(scale.read());      // print a raw reading from the ADC
    Serial.println();

    delay(1000);*/
    Serial.print(":loadCellValue:");
    Serial.println(scale.read());
    delay(50);
}

