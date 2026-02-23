#ifndef DISPLAY_WRAPPER_H
#define DISPLAY_WRAPPER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

// Частота I2C для OLED
#define OLED_I2C_FREQ 400000UL

class DisplayWrapper : public Adafruit_SSD1306 {
private:
  TwoWire* i2cPort;

public:
  static const uint16_t COLOR_WHITE = SSD1306_WHITE;
  static const uint16_t COLOR_BLACK = SSD1306_BLACK;

  DisplayWrapper() : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1), i2cPort(&Wire) {}

  void begin() {
    // Инициализация I2C с правильными пинами для каждой платы
#if defined(CHIP_ESP32S3)
    i2cPort->begin(OLED_SDA_PIN, OLED_SCL_PIN, OLED_I2C_FREQ);
#elif defined(CHIP_ESP32C3)
    i2cPort->begin(OLED_SDA_PIN, OLED_SCL_PIN, OLED_I2C_FREQ);
#else
    i2cPort->begin(OLED_SDA_PIN, OLED_SCL_PIN);
#endif

    if(!Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
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
