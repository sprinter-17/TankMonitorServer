#include "tankmodel.h"

#include <limits.h>

const int BATTERY_VOLTAGE_LEVELS[BATTERY_LEVEL_COUNT] = {180, 185, 190, 195, INT_MAX};

Tank::Tank(const uint8_t id_, const char *code_, const char *name_, uint8_t height_)
    : id(id_), code(code_), name(name_), height(height_), readingCount(0) {
    readings = (TankReading *)malloc(sizeof(TankReading) * MAX_READING_COUNT);
}

Tank::~Tank() {
    free(readings);
}

void Tank::addReading(uint8_t id, TankReading reading) {
    Tank *tank = getTank(id);
    if (tank != NULL) {
        if (tank->readingCount < MAX_READING_COUNT)
            ++tank->readingCount;
        for (int i = tank->readingCount - 1; i > 0; i--)
            tank->readings[i] = tank->readings[i - 1];
        tank->readings[0] = reading;
    } else {
    }
}

DateTime Tank::getLastReadingTime() { return readings[0].dateTime; }

float Tank::getFullness() { return 1.0 - (float)readings[0].distance / height; }

int Tank::getBattery() {
    for (int lev = 0; lev < BATTERY_LEVEL_COUNT; lev++) {
        if (readings[0].voltage < BATTERY_VOLTAGE_LEVELS[lev]) {
            return lev;
        }
    }
    return -1;
};

Tank tanks[TANK_COUNT] = {
    Tank(1, "CN", "North Concrete", 200), Tank(2, "CS", "South Concrete", 200),
    Tank(3, "PN", "North Poly", 200), Tank(4, "PS", "South Poly", 200)};

Tank *Tank::getTank(int id) {
    for (int i = 0; i < TANK_COUNT; i++) {
        if (tanks[i].id == id) return tanks + i;
    }
    return NULL;
}