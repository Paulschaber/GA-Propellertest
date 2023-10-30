//
// Created by Paul Schaber-Wallis on 2023-10-26.
//

#ifndef UNTITLED_LOADCELLCONTROLLER_H
#define UNTITLED_LOADCELLCONTROLLER_H

#include "HX711.h"

class LoadcellController {
private:
    HX711* scale;
public:
    void calib();
    LoadcellController(HX711* newScale);
};


#endif //UNTITLED_LOADCELLCONTROLLER_H
