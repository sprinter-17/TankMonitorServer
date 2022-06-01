#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include "nrf905.h"
#include "rtc.h"
#include "storage.h"
#include "tankdisplay.h"
#include "tankmodel.h"

// water tank level detection server
// takes RF readings from clients and displays them on OLED screen
// use Nano Every 
//
// pinouts ([n] = level shifter channel):
// D2 - BTN
// D3 - [3] - OLED4
// D4 - [4] - OLED15
// D5 - [5] - OLED16
// D6 - uSD CS
// D7 - RF DR
// D8 - [2] - RF CE
// D9 - [1] - RF TxEN
// D10 -[8] - RF CSN
// D11 - uSD MOSI - [6] - RF MOSI / OLED8
// D12 - RF MISO / uSD MISO
// D13 - uSD CLK [7] - RF SCK / OLED7
// A4 - RTC SCL
// A5 - RTC SDA

const uint8_t FULL_BATTERY = 188;   // 1.4v per battery
const uint8_t EMPTY_BATTERY = 115;  // 0.87v per battery
const uint32_t SERVER_ADDRESS = 0xAD98C4;
const uint32_t CLIENT_ADDRESS = 0x87F4E9;
const uint8_t ACK = 0xA7;

const uint8_t SD_CS = 6;
const uint8_t BTN_PIN = 2;

// Declaration for SSD1306 display using HW SPI
const byte OLED_DATA_COMMAND = 3;  // pin 4 on OLED 3v - 3 on shifter
const byte OLED_CHIP_SELECT = 4;   // pin 15 on OLED 3v - 4 on shifter
const byte OLED_RESET = 5;         // pin 16 on OLED 3v - 5 on shifter

TankDisplay display(OLED_DATA_COMMAND, OLED_CHIP_SELECT, OLED_RESET);
Storage storage(display, SD_CS);

const unsigned long TIMEOUT = 10000;
const unsigned long DEBOUNCE_DELAY = 200;
const unsigned long SCREEN_OFF_TIME = 30000;
unsigned long lastPush = 0;

void buttonPushed();

void setup() {
#if DEBUG
    Serial.begin(115200);
#endif

    // initialise OLED display
    display.begin();
    display.showMessage("Starting server");

    NRF905.begin(SERVER_ADDRESS);
    Wire.begin();

    // initialise SD adaptor and read history
    storage.begin();
    storage.retrieveRecentReadings();
    display.showMessage("Load complete");

    // set up button for changing display
    pinMode(BTN_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BTN_PIN), buttonPushed, FALLING);

#if DEBUG
    NRF905.printConfig();
    // Storage.listFiles();
#endif

#if GENERATE_TEST_DATA
    Storage.generateTestData();
    Serial.println("Finished generating data");
    Serial.println("Upload production firmware");
    while (true)
        ;
#endif

    delay(1000);
    display.draw();
}

void loop() {
    if (millis() > lastPush + SCREEN_OFF_TIME) {
        display.turnOff();
    }
    uint8_t buffer[4];
    if (NRF905.receive(buffer, 4, TIMEOUT)) {
        uint8_t tank = buffer[0];
        uint8_t distance = buffer[1];
        uint8_t voltage = buffer[2];
        uint8_t retries = buffer[3];
#if DEBUG
        char text[100];
        sprintf(text, "received tank %d, distance %d, voltage %d, retries %d", tank,
                distance, voltage, retries);
        Serial.println(text);
#endif
        buffer[0] = ACK;
        buffer[1] = tank;
        for (int i = 0; i < 10; i++) {
            NRF905.transmit(CLIENT_ADDRESS, buffer, 2);
            delay(50);
        }
        TankReading reading(distance, voltage, retries);
        Tank::addReading(tank, reading);
        storage.storeReading(tank, reading);
        display.draw();
    } else {
#if DEBUG
        Serial.println("Timeout");
#endif
    }
}

void buttonPushed() {
    unsigned long time = millis();
    if (time > lastPush + DEBOUNCE_DELAY) {
#if DEBUG
        Serial.println("Pushed button");
#endif
        lastPush = millis();
        display.changeMode();
    }
}