#include "rtc.h"

#include <Wire.h>

const uint8_t RTC_DATA_LENGTH = 7;

const uint8_t RTC_ADDRESS = 0x68;
const uint8_t RTC_SECOND = 0;
const uint8_t RTC_MINUTE = 1;
const uint8_t RTC_HOUR = 2;
const uint8_t RTC_DATE = 4;
const uint8_t RTC_MONTH = 5;
const uint8_t RTC_YEAR = 6;

const uint8_t DAYS_IN_MONTH[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

DateTime::DateTime(const DateTime& dateTime)
    : year(dateTime.year),
      month(dateTime.month),
      day(dateTime.day),
      hour(dateTime.hour),
      minute(dateTime.minute),
      second(dateTime.second) {}

DateTime::DateTime(uint8_t year_, uint8_t month_, uint8_t day_)
    : year(year_), month(month_), day(day_), hour(0), minute(0), second(0) {}

DateTime::DateTime(uint8_t year_, uint8_t month_, uint8_t day_, uint8_t hour_,
                   uint8_t minute_, uint8_t second_)
    : year(year_),
      month(month_),
      day(day_),
      hour(hour_),
      minute(minute_),
      second(second_) {}

int daysInMonth(int year, int month) {
    int days = DAYS_IN_MONTH[month];
    if (month == 2 && year % 4 == 0) ++days;
    return days;
}

void DateTime::addDays(int delta) {
    int current = day + delta;
    // increment months until days are in range
    while (current > daysInMonth(year, month)) {
        current -= daysInMonth(year, month);
        nextMonth();
    }
    // decrement months until days are in range
    while (current < 1) {
        previousMonth();
        current += daysInMonth(year, month);
    }
    day = current;
}

void DateTime::previousMonth() {
    --month;
    if (month < 1) {
        --year;
        month = 12;
    }
}

void DateTime::nextMonth() {
    ++month;
    if (month > 12) {
        ++year;
        month = 1;
    }
}

char* DateTime::formatted() {
    static char text[40];
    sprintf(text, "%d/%d/%d %d:%02d:%02d", day, month, year, hour, minute, second);
    return text;
}

int decodeBCD(uint8_t value) { return 10 * (value >> 4) + (value & 0x0f); }

DateTime RTC_::read() {
    uint8_t data[RTC_DATA_LENGTH];

    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0);
    Wire.endTransmission();
    Wire.requestFrom(RTC_ADDRESS, RTC_DATA_LENGTH, (uint8_t) true);
    Wire.readBytes(data, RTC_DATA_LENGTH);

    return DateTime(decodeBCD(data[RTC_YEAR]), decodeBCD(data[RTC_MONTH]),
                    decodeBCD(data[RTC_DATE]), decodeBCD(data[RTC_HOUR]),
                    decodeBCD(data[RTC_MINUTE]), decodeBCD(data[RTC_SECOND]));
}

RTC_ RealTimeClock;