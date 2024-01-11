#include <Arduino.h>
#include "../.pio/libdeps/esp32dev/ADS1X15/ADS1X15.h"
#include "../.pio/libdeps/esp32dev/HX711/src/HX711.h"
#include "Wire.h"
#include "LoadcellController.h"

#define ESC_CONTROLL_PIN 25
#define ESC_CONTROLL_CHANNEL 0
#define ADC_CLOCK_PIN 16
#define ADC_DATA_PIN 17
#define LOADCELL_DATA_PIN 22
#define LOADCELL_CLOCK_PIN 21

TwoWire adcConnection(0);
ADS1115 ADS(0x48, &adcConnection);

HX711 scale;
LoadcellController controller(&scale);

int dutyCycle = 3277;

void motorRamp(int topRSpeedPCT){
    // The esc is controlled by a 50Hz signal with a pulsewidth between 5% and 10%.
    // When expressed in 16bit that pulsewidth is between 3277 and 6553 bits.
    int bitval = 3277 + topRSpeedPCT / 100.0 * 3277.0;
    while (dutyCycle < bitval) {

        // ADC code
        int16_t val_0 = ADS.readADC(0);
        int16_t val_1 = ADS.readADC(1);

        // Prints out the motor speed in percent
        Serial.print((dutyCycle - 3277) / 3277.0 * 100.0);

        Serial.print(";");  // Spacer for splitting the data into separate columns

        // Prints out the values of the adc pins
        Serial.print(val_0 / 1231.0);
        Serial.print(";");
        Serial.print(val_1 / 19.0);

        Serial.print(";");

        // Prints out the value given by the loadcell
        Serial.println(scale.get_value());

        // ESC code
        for (int i = 0; i < 3; i++) {
            ledcWrite(ESC_CONTROLL_CHANNEL, dutyCycle);
            dutyCycle += 1;
            delay(50 / 3);
        }
    }
}

void setup() {
    Serial.begin(115200);


    // Setup for loadcell
    scale.begin(LOADCELL_DATA_PIN, LOADCELL_CLOCK_PIN);

    // PWM signal for ESC to controll motorspeed
    ledcAttachPin(ESC_CONTROLL_PIN, ESC_CONTROLL_CHANNEL);
    ledcSetup(ESC_CONTROLL_CHANNEL, 50, 16);
    ledcWrite(ESC_CONTROLL_CHANNEL, dutyCycle);

    // Setup for adc measuring the voltage and current passing to/ through the motor
    adcConnection.begin(ADC_DATA_PIN, ADC_CLOCK_PIN);
    ADS.begin();
    ADS.setGain(0);

    controller.calib();

    // countdown before program starts doing things
    for (int i = 0; i <= 5; i++) {
        Serial.println(5-i);
        delay(1000);
    }

    // Labels for the data that will be printed
    Serial.print("motorSpeed%");
    Serial.print(";");
    Serial.print("batteryVoltage");
    Serial.print(";");
    Serial.print("circuitCurrent");
    Serial.print(";");
    Serial.println("loadCellValue");

    // Motor ramp up sequence, including code making measurements
    motorRamp(50);
}



void loop() {
    // Variables for the readout of the adc pins
    int16_t val_0 = ADS.readADC(0);
    int16_t val_1 = ADS.readADC(1);

    // The calibration factor to adjust the voltage reading of the adc
    //float f = ADS.toVoltage(1/1231);

    //The calibration factor for the adc pin reading current
    //float c = ADS.toVoltage(1/1231);

    // Prints out the motor speed in percent
    Serial.print((dutyCycle - 3277) / 3277.0 * 100.0);

    Serial.print(";");  // Spacer for splitting the data into separate columns


    // Prints out the corrected values of the adc pins
    Serial.print(val_0 / 1231.0);
    Serial.print(";");
    Serial.print(val_1 / 19.0);

    Serial.print(";");

    // Prints out the value given by the loadcell
    Serial.println(scale.get_value());
    delay(50);
}

