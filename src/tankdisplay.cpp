#include "tankdisplay.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

#include "tankmodel.h"

const byte SCREEN_WIDTH = 128;  // OLED display width, in pixels
const byte SCREEN_HEIGHT = 64;  // OLED display height, in pixels

const uint32_t BITRATE = 2000000UL;

const short LINE_MARGIN = 2;
const short TANK_CODE_WIDTH = 16;
const short BATTERY_WIDTH = 7;
const short BATTERY_HEIGHT = BATTERY_LEVEL_COUNT * 2;
const short BATTERY_TERMINAL_WIDTH = 3;
const short BATTERY_TERMINAL_HEIGHT = 1;
const short GAP = 5;
const short RIGHT_MARGIN = 2;
const short LINE_HEIGHT = SCREEN_HEIGHT / TANK_COUNT;
const short RECT_HEIGHT = LINE_HEIGHT - LINE_MARGIN * 2;
const short RECT_CORNER_RADIUS = 2;

void drawTankSummary(Adafruit_SSD1306 display, int y, Tank* tank);
void drawTankDetail(Adafruit_SSD1306 display, Tank* tank);

TankDisplay::TankDisplay(uint8_t dataCommandPin, uint8_t chipSelectPin, uint8_t resetPin)
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, dataCommandPin, resetPin, chipSelectPin,
               BITRATE),
      mode(1) {}

void TankDisplay::begin() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
#if DEBUG
        Serial.println(F("SSD1306 memory allocation failed"));
#endif
        while (true)
            ;  // Don't proceed, loop forever
    }

    display.setTextSize(1);               // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);  // Draw white text
    display.cp437(true);                  // Use full 256 char 'Code Page 437' font
    display.clearDisplay();
}

void TankDisplay::showMessage(char* message) {
    display.println(message);
    display.display();
}

void TankDisplay::turnOff() {
    mode = 0;
    draw();
}

void TankDisplay::changeMode() {
    ++mode;
    if (mode > TANK_COUNT + 1) mode = 1;
    draw();
}

void TankDisplay::draw() {
    display.clearDisplay();
    if (mode == 1) {
        for (int i = 0; i < TANK_COUNT; i++) {
            int y = LINE_HEIGHT * i + LINE_MARGIN;
            drawTankSummary(display, y, Tank::getTank(i + 1));
        }
    } else {
        drawTankDetail(display, Tank::getTank(mode - 1));
    }
    display.display();
}

void drawTankLabel(Adafruit_SSD1306 display, int x, int y, Tank* tank) {
    display.setCursor(x, y + 4);  // Start at top-left corner
    display.write(tank->getCode());
}

void drawTankBattery(Adafruit_SSD1306 display, int x, int y, Tank* tank) {
    int battery = tank->getBattery();
#if DEBUG
    if (battery < 0) {
        Serial.print("Battery level ");
        Serial.println(battery);
    }
#endif
    // draw battery terminal
    display.fillRect(x + (BATTERY_WIDTH - BATTERY_TERMINAL_WIDTH) / 2, y,
                     BATTERY_TERMINAL_WIDTH, 1, SSD1306_WHITE);
    y += BATTERY_TERMINAL_HEIGHT;
    // draw battery outline
    display.drawRoundRect(x, y, BATTERY_WIDTH, BATTERY_HEIGHT, RECT_CORNER_RADIUS,
                          SSD1306_WHITE);
    y += 2 * (BATTERY_LEVEL_COUNT - battery) - 1;
    // draw battery level
    display.fillRect(x, y, BATTERY_WIDTH, battery * 2, SSD1306_WHITE);
}

void drawTankLevel(Adafruit_SSD1306 display, int x, int y, Tank* tank) {
    float fullness = tank->getFullness();
    short barWidth = fullness * (SCREEN_WIDTH - x - RIGHT_MARGIN);
    display.fillRoundRect(x, y, barWidth, RECT_HEIGHT, RECT_CORNER_RADIUS, SSD1306_WHITE);
}

void drawTankSummary(Adafruit_SSD1306 display, int y, Tank* tank) {
    if (tank != NULL) {
        int x = LINE_MARGIN;
        drawTankLabel(display, x, y, tank);
        if (tank->hasReading()) {
            x += TANK_CODE_WIDTH;
            drawTankBattery(display, x, y, tank);
            x += BATTERY_WIDTH + GAP;
            drawTankLevel(display, x, y, tank);
        } else {
            display.write("    - Unknown -");
        }
    }
}

int readingToY(Tank* tank, int i) {
    TankReading reading = tank->getReading(i);
    return map(reading.distance, tank->getHeight(), 0, SCREEN_HEIGHT, LINE_HEIGHT * 2);
}

void drawTankDetail(Adafruit_SSD1306 display, Tank* tank) {
    if (tank != NULL) {
        display.setCursor(20, 4);  // Start at top-left corner
        display.println(tank->getName());
        display.setCursor(20, LINE_HEIGHT);  // Start at top-left corner
        display.println(tank->getLastReadingTime().formatted());
        for (int i = 0; i < tank->getReadingCount() - 1; i++) {
            int x0 = SCREEN_WIDTH - 2 * i;
            int y0 = readingToY(tank, i);
            int x1 = SCREEN_WIDTH - 2 * (i + 1);
            int y1 = readingToY(tank, i + 1);
            display.drawLine(x0, y0, x1, y1, SSD1306_WHITE);
        }
    }
}
