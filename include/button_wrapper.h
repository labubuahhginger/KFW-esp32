#ifndef BUTTON_WRAPPER_H
#define BUTTON_WRAPPER_H

#include "config.h"

class ButtonWrapper {
private:
  unsigned long lastPressNext = 0;
  unsigned long lastPressOK = 0;
  unsigned long holdStartTime = 0;
  const int debounceDelay = 220;
  const int holdDelay = 500;

public:
  void begin() {
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_OK_PIN, INPUT_PULLUP);
  }

  void update() {
    bool nextLow = (digitalRead(BTN_NEXT_PIN) == LOW);
    bool okLow = (digitalRead(BTN_OK_PIN) == LOW);
    
    // Сбрасываем таймер удержания, если кнопка отпущена
    if (!nextLow) holdStartTime = 0;
    if (!okLow) holdStartTime = 0;
    
    // Запоминаем время начала удержания
    if (nextLow && holdStartTime == 0) holdStartTime = millis();
    if (okLow && holdStartTime == 0) holdStartTime = millis();
  }

  bool nextPressed() {
    bool pressed = (digitalRead(BTN_NEXT_PIN) == LOW);
    if (pressed && (millis() - lastPressNext > debounceDelay)) {
      lastPressNext = millis();
      return true;
    }
    return false;
  }

  bool okPressed() {
    bool pressed = (digitalRead(BTN_OK_PIN) == LOW);
    if (pressed && (millis() - lastPressOK > debounceDelay)) {
      lastPressOK = millis();
      return true;
    }
    return false;
  }

  bool powerPressed() { return false; }
  
  bool nextHeld() {
    return (digitalRead(BTN_NEXT_PIN) == LOW) && (holdStartTime > 0) && (millis() - holdStartTime > holdDelay);
  }
  
  bool okHeld() {
    return (digitalRead(BTN_OK_PIN) == LOW) && (holdStartTime > 0) && (millis() - holdStartTime > holdDelay);
  }
};

extern ButtonWrapper buttons;

#endif
