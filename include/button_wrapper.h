#ifndef BUTTON_WRAPPER_H
#define BUTTON_WRAPPER_H

#include "config.h"

class ButtonWrapper {
private:
  unsigned long lastPress = 0;
  const int debounceDelay = 220;
  
public:
  void begin() {
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_OK_PIN, INPUT_PULLUP);
  }
  
  void update() { }
  
  bool nextPressed() {
    bool pressed = (digitalRead(BTN_NEXT_PIN) == LOW);
    if (pressed && (millis() - lastPress > debounceDelay)) {
      lastPress = millis();
      return true;
    }
    return false;
  }
  
  bool okPressed() {
    bool pressed = (digitalRead(BTN_OK_PIN) == LOW);
    if (pressed && (millis() - lastPress > debounceDelay)) {
      lastPress = millis();
      return true;
    }
    return false;
  }
  
  bool powerPressed() { return false; }
  bool nextHeld() { return (digitalRead(BTN_NEXT_PIN) == LOW); }
  bool okHeld() { return (digitalRead(BTN_OK_PIN) == LOW); }
};

extern ButtonWrapper buttons;

#endif
