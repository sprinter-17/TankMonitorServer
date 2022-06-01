#ifndef RTC_H
#define RTC_H

#include <Arduino.h>

class DateTime {
   public:
    DateTime(const DateTime& dateTime);
    DateTime(uint8_t year_, uint8_t month_, uint8_t day_);
    DateTime(uint8_t year_, uint8_t month_, uint8_t day_, uint8_t hour_, uint8_t minute_,
             uint8_t second_);
    char* formatted();
    void addDays(int delta);
    void previousDay() { addDays(-1); }
    void nextDay() { addDays(+1); }

    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

   private:
    void previousMonth();
    void nextMonth();
};

class RTC_ {
   public:
    DateTime read(void);
};

extern RTC_ RealTimeClock;

#endif