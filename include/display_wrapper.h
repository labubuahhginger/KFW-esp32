#ifndef DISPLAY_WRAPPER_H
#define DISPLAY_WRAPPER_H

#include "config.h"

#ifdef PLATFORM_M5STICKC_PLUS2
  #include <M5StickCPlus2.h>
  
  class DisplayWrapper {
  public:
    void begin() {
      M5.begin();
      M5.Lcd.setRotation(DISPLAY_ROTATION);
      M5.Lcd.fillScreen(COLOR_BG);
      M5.Lcd.setTextColor(COLOR_TEXT);
      M5.Lcd.setTextSize(1);
    }
    
    void clearDisplay() { M5.Lcd.fillScreen(COLOR_BG); }
    void display() { }  // M5 draws immediately
    void setCursor(int16_t x, int16_t y) { M5.Lcd.setCursor(x, y); }
    void setTextSize(uint8_t s) { M5.Lcd.setTextSize(s); }
    void setTextColor(uint16_t c) { M5.Lcd.setTextColor(c); }
    void setTextColor(uint16_t fg, uint16_t bg) { M5.Lcd.setTextColor(fg, bg); }
    
    void print(const char* str) { M5.Lcd.print(str); }
    void print(String str) { M5.Lcd.print(str); }
    void print(int val) { M5.Lcd.print(val); }
    void println(const char* str) { M5.Lcd.println(str); }
    
    void printf(const char* fmt, ...) {
      va_list args;
      va_start(args, fmt);
      M5.Lcd.vprintf(fmt, args);
      va_end(args);
    }
    
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
      M5.Lcd.fillRect(x, y, w, h, color);
    }
    
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
      M5.Lcd.drawRect(x, y, w, h, color);
    }
    
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
      M5.Lcd.drawLine(x0, y0, x1, y1, color);
    }
    
    void drawPixel(int16_t x, int16_t y, uint16_t color) {
      M5.Lcd.drawPixel(x, y, color);
    }
    
    void drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, uint16_t color) {
      for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
          if (bitmap[j] & (0x80 >> i)) {
            M5.Lcd.drawPixel(x + i, y + j, color);
          }
        }
      }
    }
    
    void ssd1306_command(uint8_t cmd) {
      if (cmd == 0x8F) M5.Lcd.setBrightness(255);
    }
    
    static const uint16_t BLACK = 0x0000;
    static const uint16_t WHITE = 0xFFFF;
  };
  
#else
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  
  class DisplayWrapper : public Adafruit_SSD1306 {
  public:
    DisplayWrapper() : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}
    
    void begin() {
      Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, 0x3C);
      setTextColor(WHITE);
      setTextSize(1);
      clearDisplay();
    }
    
    void printf(const char* fmt, ...) {
      char buffer[128];
      va_list args;
      va_start(args, fmt);
      vsnprintf(buffer, sizeof(buffer), fmt, args);
      va_end(args);
      print(buffer);
    }
    
    using Adafruit_SSD1306::BLACK;
    using Adafruit_SSD1306::WHITE;
  };
#endif

extern DisplayWrapper display;

#endif
