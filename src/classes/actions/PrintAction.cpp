#include "PrintAction.h"
#include <Arduino.h>
#include <Keyboard.h>

PrintAction::PrintAction(String message) {
    this->message = message;
}

void PrintAction::execute() {
    typeString(message);
}

void PrintAction::typeString(String string) {
  for (int i = 0; i < string.length(); i++) {
    char character = string.charAt(i);
    Keyboard.write(character);
    delay(10);
  }
  Keyboard.write(KEY_RETURN);
}
