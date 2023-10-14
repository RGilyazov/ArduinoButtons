#include <Arduino.h>
#include <Keyboard.h>
#include "classes/Button.h"
#include "classes/actions/PrintAction.h"
#include "version.h"

#define buttonPin 2        // analog input pin to use as a digital input
#define ledPin 13          // digital output pin for the LED
Button button;

void setup() {
   button.onClick = new PrintAction("git push");
   button.onDoubleClick = new PrintAction("git pull");
   button.onHold = new PrintAction("NICE :)))");
   button.onLongHold = new PrintAction(String("Version: ") 
    + VERSION
    + ". Source code can be found on https://github.com/RGilyazov/ArduinoButtons/tree/" 
    + PROJECT_NAME
    + "/v"
     + VERSION);
   button.setup(buttonPin);
   digitalWrite(ledPin, HIGH);
}

void loop() {
  button.loop();
}
