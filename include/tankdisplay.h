#ifndef TANKDISPLAY_H
#define TANKDISPLAY_H

#include <Adafruit_SSD1306.h>

class TankDisplay {
   public:
    TankDisplay(uint8_t dataCommandPin, uint8_t chipSelectPin, uint8_t resetPin);
    void begin();
    void showMessage(char* message);
    void draw();
    void turnOff();
    void changeMode();

   private:
    Adafruit_SSD1306 display;
    // mode 0: display off, 1: summary: 2+: tank details
    int mode;
};

#endif