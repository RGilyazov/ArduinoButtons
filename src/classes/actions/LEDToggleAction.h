#ifndef LED_TOGGLE_ACTION_H
#define LED_TOGGLE_ACTION_H

#include <Arduino.h>
#include "InstantAction.h"
#include "../leds/RGLed.h"

class LEDToggleAction : public InstantAction {
public:
    explicit LEDToggleAction(RGLed* led) : led(led) {}
    
    bool isValid() const override { return led != nullptr; }
    const char* getCurrentColorName() const;

protected:
    void execute() override {
        if (isCurrentlyRed()) {
            led->showGreen();
        } else if (isCurrentlyGreen()) {
            led->showYellow();
        } else if (isCurrentlyYellow()) {
            led->showRed();
        } else {
            led->showRed();
        }
    }

private:
    RGLed* led;
    
    bool isCurrentlyRed() const {
        if (!led) return false;
        return led->getCurrentRed() > 0 && led->getCurrentGreen() == 0;
    }
    
    bool isCurrentlyGreen() const {
        if (!led) return false;
        return led->getCurrentGreen() > 0 && led->getCurrentRed() == 0;
    }
    
    bool isCurrentlyYellow() const {
        if (!led) return false;
        return led->getCurrentRed() > 0 && led->getCurrentGreen() > 0;
    }
};

inline const char* LEDToggleAction::getCurrentColorName() const {
    if (!led || !led->isOn()) {
        return "Off";
    }
    
    uint8_t red = led->getCurrentRed();
    uint8_t green = led->getCurrentGreen();
    
    if (red > 0 && green > 0) {
        return "Yellow";
    } else if (red > 0) {
        return "Red";
    } else if (green > 0) {
        return "Green";
    } else {
        return "Off";
    }
}

#endif