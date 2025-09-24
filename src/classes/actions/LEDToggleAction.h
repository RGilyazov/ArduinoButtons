#ifndef LED_TOGGLE_ACTION_H
#define LED_TOGGLE_ACTION_H

#include <Arduino.h>
#include "InstantAction.h"
#include "../leds/ILed.h"

class LEDToggleAction : public InstantAction {
public:
    explicit LEDToggleAction(ILed* led) : led(led) {}
    
    bool isValid() const override { return led != nullptr; }

protected:
    void execute() override {
        LEDState current = led->getState();
        
        if (current.type == LEDState::INTENT) {
            switch (current.intent) {
                case LEDIntent::RED:
                    led->setState(LEDState::green());
                    break;
                case LEDIntent::GREEN:
                    led->setState(LEDState::yellow());
                    break;
                case LEDIntent::YELLOW:
                    led->setState(LEDState::red());
                    break;
                default:
                    led->setState(LEDState::red());
                    break;
            }
        } else {
            led->setState(LEDState::red());
        }
    }

private:
    ILed* led;
};

#endif