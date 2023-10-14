#include <Arduino.h>
#include <Keyboard.h>
#include "classes/Button.h"
#include "classes/actions/PrintAction.h"

#define buttonPin 2        // analog input pin to use as a digital input
Button button;

void setup() {
   button.onClick = new PrintAction("git push");
   button.onDoubleClick = new PrintAction("git pull");
   button.onHold = new PrintAction("NICE :)))");
   button.onLongHold = new PrintAction("Source code of this button can be found on https://github.com/RGilyazov/GitPush");
   button.setup(buttonPin);
}

void loop() {
  button.loop();
}
