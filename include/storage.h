#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

#include "tankreading.h"

class Storage_ {
   public:
    void begin(byte chipSelectPin);
    void listFiles();
    void retrieveRecentReadings();
    void storeReading(int tank, TankReading reading);
    void generateTestData();
};

extern Storage_ Storage;

#endif