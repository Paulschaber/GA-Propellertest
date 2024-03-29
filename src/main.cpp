#include <Arduino.h>
#include "../.pio/libdeps/esp32dev/ADS1X15/ADS1X15.h"
#include "../.pio/libdeps/esp32dev/HX711/src/HX711.h"
#include "Wire.h"
//#include "LoadcellController.h"

#define ESC_CONTROLL_PIN 25
#define ESC_CONTROLL_CHANNEL 0
#define ADC_CLOCK_PIN 16
#define ADC_DATA_PIN 17
#define LOADCELL_DATA_PIN 22
#define LOADCELL_CLOCK_PIN 21
#define I_CORRECTION_FOR_BAT (23 / 15)
#define VOLTAGE_CORRECTION (val_0 * 3.0 / 5380.0 + 0.02)
#define CURRENT_CORRECTION (val_1 * 61.0 / 28050.0 + 0.02)
#define WEIGHT_CORRECTION (fmax((scale.read() - tare_value) / 45.0, 0))


TwoWire adcConnection(0);
ADS1115 ADS(0x48, &adcConnection);

HX711 scale;
//LoadcellController controller(&scale);
double tare_value;

int dutyCycle = 3550;

void printMeasurements(long scaleValue) {
    // ADC code
    //int16_t val_0 = ADS.readADC(0);
    //int16_t val_1 = ADS.readADC(1);



    // print time
    Serial.print(millis());
    Serial.print(";");

    // Prints out the motor speed in percent
    Serial.print((dutyCycle - 3277) / 3277.0 * 100.0);

    Serial.print(";");  // Spacer for splitting the data into separate columns

    // Prints out the values of the adc pins
    Serial.print(ADS.readADC(0) * 3.0 / 5380.0 + 0.02);
    Serial.print(";");
    Serial.print((ADS.readADC(1) * 61.0 / 28050.0 + 0.02) * I_CORRECTION_FOR_BAT);

    Serial.print(";");

    // Prints out the value given by the loadcell
    //Serial.print(scale.get_value());
    //Serial.print(";");
    Serial.println(fmax((scaleValue - tare_value) / 45.0, 0));
}

void motorRamp(int topRSpeedPCT) {
    // The esc is controlled by a 50Hz signal with a pulsewidth between 5% and 10%.
    // When expressed in 16bit that pulsewidth is between 3277 and 6553 bits.
    int bitval = 3277 + topRSpeedPCT / 100.0 * 3277.0;
    while (dutyCycle < bitval) {
        long scaleValue = scale.read();
        printMeasurements(scaleValue);

        // ESC code
        for (int i = 0; i < 3; i++) {
            ledcWrite(ESC_CONTROLL_CHANNEL, dutyCycle);
            dutyCycle += 1;
            delay(50 / 3);
        }
    }
}

void rampUntilThrust(int thrustValue, int maxRotationPCT) { // thrustValue in grams
    //Checks that motor is set to spin slower than a certain value expressed in percent.
    //This is to avoid uncontrolled acceleration of the motor that could be caused from bad load cell readings.
    if (dutyCycle > 3277.0 + 3277.0 / 100.0 * maxRotationPCT) return;

    long scaleBuf[10];
    for (int i = 0; i < 10; i++) {
        scaleBuf[i] = 0;
    }

    while (dutyCycle < 3277.0 + 3277.0 / 100.0 * maxRotationPCT) {
        //Serial.println("I rolig funktion");
        long scaleValue = scale.read();
        long scaleAvg = scaleValue;
        for (int i = 8; i >= 0; i--) {
            scaleBuf[i + 1] = scaleBuf[i];
            scaleAvg += scaleBuf[i];
        }
        scaleBuf[0] = scaleValue;
        if ((scaleAvg / 10.0 - tare_value) / 45.0 >= thrustValue) break;
        //Serial.println((scaleAvg / 10.0 - tare_value) / 45.0);
        printMeasurements(scaleValue);

        // ESC code
        for (int i = 0; i < 3; i++) {
            ledcWrite(ESC_CONTROLL_CHANNEL, dutyCycle);
            dutyCycle += 1;
            delay(50 / 3);
        }

        //if (dutyCycle > 3277.0 + 3277.0 / 100.0 * maxRotationPCT) break;

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
    ADS.setGain(16);

    //controller.calib();
    Serial.println("please wait for the load cell to tare");
    tare_value = scale.read_average(50);

    // countdown before program starts doing things
    for (int i = 5; i >= 0; i--) {
        Serial.println(i);
        delay(1000);
    }


    // Labels for the data that will be printed
    Serial.print("time");
    Serial.print(";");
    Serial.print("motorSpeed%");
    Serial.print(";");
    Serial.print("batteryVoltage");
    Serial.print(";");
    Serial.print("circuitCurrent");
    Serial.print(";");
    Serial.print("loadCellValue");
    Serial.print(";");
    Serial.println("weight");

    // Motor ramp up sequence, including code making measurements
    //motorRamp(75);
    rampUntilThrust(400, 90);

    // Continues measurements for set time (t[minutes] must be multiplied with sample rate)
    int t;
    for (t = 0; t < 5 * 20; t++) {
        long scaleValue = scale.read();
        printMeasurements(scaleValue);

        delay(50);
    }
    dutyCycle = 3277;
    ledcWrite(ESC_CONTROLL_CHANNEL, dutyCycle);
    Serial.println("Done");
}

void loop() {

}