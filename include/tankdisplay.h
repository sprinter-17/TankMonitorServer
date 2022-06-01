#ifndef TANKDISPLAY_H
#define TANKDISPLAY_H

#include <Adafruit_SSD1306.h>

class TankDisplay_ {
   public:
    TankDisplay_() : mode(1) {}
    void begin();
    void showMessage(char* message);
    void draw();
    void turnOff();
    void changeMode();

   private:
    // mode 0: display off, 1: summary: 2+: tank details
    int mode;
};

extern TankDisplay_ TankDisplay;

#endif