#ifndef BUTTONHANDLER
#define BUTTONHANDLER

#include <Arduino.h>

class Button
{
  private:
    bool pullupMode;
    int buttonPin;
    int outputState;
    int buttonState;
    int lastButtonState;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay; 

  public:
    Button(int btnPin, bool pullup = false);
    void scan();
    int getOutput(bool reset = true);
};
#endif