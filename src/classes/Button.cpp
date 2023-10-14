/* 
*   BASED ON:
*   4-Way Button By Jeff Saltzman 
*/

#include "Button.h"
#include <Arduino.h>

constexpr int debounce = 20;          // ms debounce period to prevent flickering when pressing or releasing the button
constexpr int DCgap = 250;            // max ms between clicks for a double click event
constexpr int holdTime = 1000;        // ms hold period: how long to wait for press+hold event
constexpr int longHoldTime = 3000;    // ms long hold period: how long to wait for press+hold event

Button::Button() {
}

void Button::setup(int buttonPin) {
    this->buttonPin = buttonPin;
    pinMode(buttonPin, INPUT);
    digitalWrite(buttonPin, HIGH );
}

void Button::loop() {
   int b = checkButton();
   if (b == 1) clickEvent();
   if (b == 2) doubleClickEvent();
   if (b == 3) holdEvent();
   if (b == 4) longHoldEvent();
}

void Button::clickEvent() {
  if (onClick != nullptr) {
    onClick->execute();
  }
}
void Button::doubleClickEvent() {
  if (onDoubleClick != nullptr) {
    onDoubleClick->execute();
  }
}
void Button::holdEvent() {
  if (onHold != nullptr) {
    onHold->execute();
  }
}
void Button::longHoldEvent() {
  if (onLongHold != nullptr) {
    onLongHold->execute();
  }
}

int Button::checkButton() {    
   int event = 0;
   buttonVal = digitalRead(buttonPin);
   // Button pressed down
   if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
   {
       downTime = millis();
       ignoreUp = false;
       waitForUp = false;
       singleOK = true;
       holdEventPast = false;
       longHoldEventPast = false;
       if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
   {        
       if (not ignoreUp)
       {
           upTime = millis();
           if (DConUp == false) DCwaiting = true;
           else
           {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
   {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast)
       {
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       }
       // Trigger "long" hold
       if ((millis() - downTime) >= longHoldTime)
       {
           if (not longHoldEventPast)
           {
               event = 4;
               longHoldEventPast = true;
           }
       }
   }
   buttonLast = buttonVal;
   return event;
}
