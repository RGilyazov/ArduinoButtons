#ifndef LED_COLOR_ACTIONS_H
#define LED_COLOR_ACTIONS_H

#include <Arduino.h>
#include "InstantAction.h"
#include "../leds/ILed.h"

class LEDRedAction : public InstantAction {
public:
    explicit LEDRedAction(ILed* led) : led(led) {}
    bool isValid() const override { return led != nullptr; }

protected:
    void execute() override { led->setState(LEDState::red()); }

private:
    ILed* led;
};

class LEDYellowAction : public InstantAction {
public:
    explicit LEDYellowAction(ILed* led) : led(led) {}
    bool isValid() const override { return led != nullptr; }

protected:
    void execute() override { led->setState(LEDState::yellow()); }

private:
    ILed* led;
};

class LEDGreenAction : public InstantAction {
public:
    explicit LEDGreenAction(ILed* led) : led(led) {}
    bool isValid() const override { return led != nullptr; }

protected:
    void execute() override { led->setState(LEDState::green()); }

private:
    ILed* led;
};

#endif