#include <Arduino.h>
#include <Keyboard.h>
#include "classes/Button.h"
#include "classes/actions/PrintAction.h"
#include "version.h"

#define buttonPin 3 // analog input pin to use as a digital input
#define ledPin1 7   // digital output pin for the LED1 (GREEN)
#define ledPin2 9   // digital output pin for the LED2 (RED)
Button button;

void setup()
{
  button.onClick = new PrintAction("git push");
  button.onDoubleClick = new PrintAction("git pull");
  button.onHold = new PrintAction("NICE :)))");
  button.onLongHold = new PrintAction(String("Version: ") + VERSION + ". Source code can be found on https://github.com/RGilyazov/ArduinoButtons/tree/" + PROJECT_NAME + "/v" + VERSION);
  button.setup(buttonPin);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  randomSeed(analogRead(0));
  if (random(2) == 0)
  {
    digitalWrite(ledPin1, HIGH);
  }
  else
  {
    digitalWrite(ledPin2, HIGH);
  }
}

void loop()
{
  button.loop();
}
