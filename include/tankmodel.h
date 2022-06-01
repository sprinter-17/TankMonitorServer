#ifndef TANKMODEL_H
#define TANKMODEL_H

#include <Arduino.h>

#include "tankreading.h"

const int MAX_READING_COUNT = 60;
const int TANK_COUNT = 4;
const int BATTERY_LEVEL_COUNT = 5;

class Tank {
   public:
    Tank(const uint8_t id, const char *code, const char *name, uint8_t height);
    ~Tank();
    static void addReading(uint8_t id, TankReading reading);
    static Tank *getTank(const int id);
    const char *getCode() { return code; }
    const char *getName() { return name; }
    const int getReadingCount() { return readingCount; }
    const TankReading getReading(int i) { return readings[i]; }

    int getHeight() { return height; }
    bool hasReading() { return readingCount > 0; }
    DateTime getLastReadingTime();
    float getFullness();
    int getBattery();

   private:
    const uint8_t id;
    const char *code;
    const char *name;
    const uint8_t height;
    int readingCount;
    TankReading *readings;
};

#endif