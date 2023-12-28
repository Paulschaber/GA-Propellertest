//
// Created by Paul Schaber-Wallis on 2023-10-26.
//

#include "LoadcellController.h"

void LoadcellController::calib() {
    while (1) {
        if (scale->is_ready()) {
            scale->set_scale();
            Serial.println("Tare... remove any weights from the scale.");
            delay(5000);
            scale->tare(50);
            Serial.println("Tare done...");
            Serial.print("Place a known weight on the scale...");
            delay(5000);
            long reading = scale->get_units(50);
            Serial.print("Result: ");
            Serial.println(reading);
            //Serial.print("please enter your calibration factor (your known weight divided by 'Result')");
           // while(!Serial.available()){}
          //  string calibration_input = Serial.readStringUntil('\n');
         //   float calibration_value = strtof(calibration_input, nullptr);
        } else {
            Serial.println("HX711 not found.");
        }
        delay(1000);

        Serial.println("Enter 'ext' to exit calibration, enter nothing to re run");
        while(!Serial.available()){}
        String user_input = Serial.readStringUntil('\n');
        if (user_input.equals("ext")){
            return;
        }
    }
}

LoadcellController::LoadcellController(HX711 *newScale) {
    scale = newScale;
}