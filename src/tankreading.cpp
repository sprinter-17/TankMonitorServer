#include "tankreading.h"

#include "rtc.h"

TankReading::TankReading(DateTime dateTime_, uint8_t distance_, uint8_t voltage_,
                         uint8_t retries_)
    : dateTime(dateTime_), distance(distance_), voltage(voltage_), retries(retries_) {}

TankReading::TankReading(uint8_t distance_, uint8_t voltage_, uint8_t retries_)
    : dateTime(RealTimeClock.read()),
      distance(distance_),
      voltage(voltage_),
      retries(retries_) {}
