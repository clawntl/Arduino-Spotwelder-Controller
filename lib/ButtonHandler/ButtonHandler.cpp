#include "ButtonHandler.h"

Button::Button(int btnPin, bool pullup)
{
  
  if (pullup == true)
  {
    pullupMode = LOW;
    pinMode(buttonPin, INPUT);
  }
  else
  {
    pullupMode = HIGH;
    pinMode(buttonPin, INPUT);
  }
  buttonPin = btnPin;
  outputState = LOW;
  lastButtonState = !pullupMode;
  lastDebounceTime = 0;
  debounceDelay = 0;
}

void Button::scan()
{
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == pullupMode) {
        outputState = !outputState;
      }
    }
  }

  lastButtonState = reading;
}

int Button::getOutput(bool reset)
{
  int temp = outputState;
  if (reset == true)
  {
    outputState = 0;
  }
  return temp;
}