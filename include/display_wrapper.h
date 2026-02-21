#ifndef DISPLAY_WRAPPER_H
#define DISPLAY_WRAPPER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

class DisplayWrapper : public Adafruit_SSD1306 {
public:
  static const uint16_t COLOR_WHITE = SSD1306_WHITE;
  static const uint16_t COLOR_BLACK = SSD1306_BLACK;

  DisplayWrapper() : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}

  void begin() {
    if(!Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      for(;;);
    }
    setTextColor(SSD1306_WHITE);
    setTextSize(1);
    clearDisplay();
    display();
  }
  
  void printf(const char* fmt, ...) {
    char buffer[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    print(buffer);
  }
};

extern DisplayWrapper display;

#endif
