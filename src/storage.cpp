#include "storage.h"

#include <Arduino.h>
#include <SD.h>

#include "rtc.h"
#include "serialprint.h"
#include "tankdisplay.h"
#include "tankmodel.h"

const int LINE_SIZE = 33;

void Storage_::begin(byte chipSelectPin) {
    if (!SD.begin(chipSelectPin)) {
#if DEBUG
        Serial.println("initialization failed!");
#endif
    }
}

void Storage_::listFiles() {
    File root = SD.open("/");
    while (File file = root.openNextFile()) {
        char* file_name = file.name();
        int dot_pos = strlen(file_name) - 4;
        if (dot_pos > 0 && file_name[0] == 'T' &&
            strcmp(file_name + dot_pos, ".DAT") == 0) {
            uint32_t file_size = file.size();
            Serial.print(file_name);
            Serial.print(": ");
            Serial.println(file_size);
            if (file_size > LINE_SIZE * 6) {
                file.seek(file_size - LINE_SIZE * 6);
            }
            while (file.available()) {
                Serial.write(file.read());
            }
        }
        file.close();
        delay(100);
    }
}

char* tankFileName(int tank) {
    static char filename[10];
    sprintf(filename, "T%d.DAT", tank);
    return filename;
}

void Storage_::storeReading(int tank, TankReading reading) {
    File file = SD.open(tankFileName(tank), FILE_WRITE);
    if (file) {
        sprintf(print_text, "%02d %02d %02d %02d %02d %02d - %04d %04d %02d\n",
                reading.dateTime.year, reading.dateTime.month, reading.dateTime.day,
                reading.dateTime.hour, reading.dateTime.minute, reading.dateTime.second,
                reading.distance, reading.voltage, reading.retries);
        file.write(print_text);
        file.close();
        delay(100);
    } else {
#if DEBUG
        sprintf(print_text, "Could not open file for tank %d for writing", tank);
        Serial.println(print_text);
#endif
    }
}

void Storage_::retrieveRecentReadings() {
    for (int tank = 1; tank <= TANK_COUNT; tank++) {
        File file = SD.open(tankFileName(tank), FILE_READ);
        if (file) {
            TankDisplay.showMessage(file.name());
#if DEBUG
            Serial.print("Reading file ");
            Serial.println(file.name());
#endif
            uint32_t file_size = file.size();
            if (file_size > LINE_SIZE * MAX_READING_COUNT) {
                file.seek(file_size - LINE_SIZE * MAX_READING_COUNT);
            }
            while (file.available()) {
                int year, month, day, hour, minute, second;
                int distance, voltage, retries;
                char line[LINE_SIZE];
                file.readBytesUntil('\n', line, LINE_SIZE);
                if (sscanf(line, "%02d %02d %02d %02d %02d %02d - %04d %04d %02d\n",
                           &year, &month, &day, &hour, &minute, &second, &distance,
                           &voltage, &retries) == 9) {
                    DateTime dateTime(year, month, day, hour, minute, second);
                    TankReading reading(dateTime, distance, voltage, retries);
                    Tank::addReading(tank, reading);
                    // file.read();
                } else {
#if DEBUG
                    Serial.print("Unrecognised reading log ");
                    Serial.println(line);
#endif
                }
            }
            file.close();
        } else {
#if DEBUG
            sprintf(print_text, "No log file for tank %d", tank);
            Serial.println(print_text);
#endif
        }
    }
}

void Storage_::generateTestData() {
    for (int tank = 1; tank <= TANK_COUNT; tank++) {
        sprintf(print_text, "Test data tank %d", tank);
        TankDisplay.showMessage(print_text);
        int height = Tank::getTank(tank)->getHeight();
        int fullness = random(height);
        char* fileName = tankFileName(tank);
        SD.remove(fileName);
        if (!SD.exists(fileName)) {
            File file = SD.open(fileName, FILE_WRITE);
            if (file) {
                DateTime dateTime(RealTimeClock.read());
                dateTime.addDays(-56);
                for (int i = 0; i < 56; i++) {
                    sprintf(print_text, "Generate data for tank %d on %s", tank,
                            dateTime.formatted());
                    Serial.println(print_text);
                    for (int hour = 4; hour < 24; hour += 8) {
                        dateTime.hour = hour;
                        dateTime.minute = 0;
                        dateTime.second = 0;
                        fullness = constrain(fullness + random(11) - 6, 0, height);
                        TankReading reading(dateTime, fullness, 170 + random(30), 0);
                        storeReading(tank, reading);
                    }
                    dateTime.nextDay();
                }
            }
        }
    }
}

Storage_ Storage;