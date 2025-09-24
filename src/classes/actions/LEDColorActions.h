#ifndef LED_COLOR_ACTIONS_H
#define LED_COLOR_ACTIONS_H

#include <Arduino.h>
#include "InstantAction.h"
#include "../leds/RGLed.h"

class LEDRedAction : public InstantAction {
public:
    explicit LEDRedAction(RGLed* led) : led(led) {}
    bool isValid() const override { return led != nullptr; }

protected:
    void execute() override { led->showRed(); }

private:
    RGLed* led;
};

class LEDYellowAction : public InstantAction {
public:
    explicit LEDYellowAction(RGLed* led) : led(led) {}
    bool isValid() const override { return led != nullptr; }

protected:
    void execute() override { led->showYellow(); }

private:
    RGLed* led;
};

class LEDGreenAction : public InstantAction {
public:
    explicit LEDGreenAction(RGLed* led) : led(led) {}
    bool isValid() const override { return led != nullptr; }

protected:
    void execute() override { led->showGreen(); }

private:
    RGLed* led;
};

#endif