#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

#include "tankdisplay.h"
#include "tankreading.h"

class Storage {
   public:
    Storage(TankDisplay display_, uint8_t chipSelectPin_)
        : display(display_), chipSelectPin(chipSelectPin_) {}
    void begin();
    void listFiles();
    void retrieveRecentReadings();
    void storeReading(int tank, TankReading reading);
    void generateTestData();

   private:
    uint8_t chipSelectPin;
    TankDisplay display;
};

#endif