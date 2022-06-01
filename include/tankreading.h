#ifndef TANKREADING_H
#define TANKREADING_H

#include "rtc.h"

class TankReading {
   public:
    TankReading(DateTime dateTime, uint8_t distance_, uint8_t voltage_, uint8_t retries_);
    TankReading(uint8_t distance_, uint8_t voltage_, uint8_t retries_);
    DateTime dateTime;
    uint8_t distance;
    uint8_t voltage;
    uint8_t retries;
};

#endif
