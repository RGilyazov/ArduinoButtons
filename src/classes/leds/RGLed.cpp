#include "RGLed.h"

RGLed::RGLed() : initialized(false) {
    currentState = LEDState::off();
}

bool RGLed::setup(uint8_t redPin, uint8_t greenPin) {
    bool redOk = redLED.setup(redPin);
    bool greenOk = greenLED.setup(greenPin);
    initialized = redOk && greenOk;
    return initialized;
}

bool RGLed::setState(const LEDState& state) {
    if (!initialized) return false;
    
    currentState = state;
    
    if (state.type == LEDState::INTENT) {
        switch (state.intent) {
            case LEDIntent::OFF:
                showOff();
                break;
            case LEDIntent::RED:
                showRed();
                break;
            case LEDIntent::GREEN:
                showGreen();
                break;
            case LEDIntent::YELLOW:
                showYellow();
                break;
            case LEDIntent::WHITE:
                showYellow();  // Best approximation for RG LED
                break;
        }
    } else {
        if (state.precise.count >= 2) {
            setRG(state.precise.values[0], state.precise.values[1]);
        } else if (state.precise.count == 1) {
            setRed(state.precise.values[0]);
            setGreen(0);
        }
    }
    
    return true;
}

LEDState RGLed::getState() const {
    return currentState;
}

bool RGLed::isSetup() const {
    return initialized && redLED.isSetup() && greenLED.isSetup();
}

void RGLed::setRed(uint8_t intensity) {
    if (!initialized) return;
    redLED.setIntensity(intensity);
}

void RGLed::setGreen(uint8_t intensity) {
    if (!initialized) return;
    greenLED.setIntensity(intensity);
}

void RGLed::setRG(uint8_t red, uint8_t green) {
    if (!initialized) return;
    redLED.setIntensity(red);
    greenLED.setIntensity(green);
}

void RGLed::showRed() {
    if (!initialized) return;
    redLED.turnOn();
    greenLED.turnOff();
}

void RGLed::showGreen() {
    if (!initialized) return;
    redLED.turnOff();
    greenLED.turnOn();
}

void RGLed::showYellow() {
    if (!initialized) return;
    redLED.turnOn();
    greenLED.turnOn();
}

void RGLed::showOff() {
    if (!initialized) return;
    redLED.turnOff();
    greenLED.turnOff();
}

void RGLed::turnOff() {
    if (!initialized) return;
    redLED.turnOff();
    greenLED.turnOff();
}

void RGLed::turnOn() {
    if (!initialized) return;
    redLED.turnOn();
    greenLED.turnOn();
}

void RGLed::toggle() {
    if (!initialized) return;
    
    // Toggle both LEDs - if either is on, turn both off
    // If both are off, turn both on to remembered intensities
    if (isOn()) {
        turnOff();
    } else {
        turnOn();
    }
}

uint8_t RGLed::getCurrentRed() const {
    return redLED.getCurrentIntensity();
}

uint8_t RGLed::getCurrentGreen() const {
    return greenLED.getCurrentIntensity();
}

bool RGLed::isOn() const {
    return redLED.isOn() || greenLED.isOn();
}

LED& RGLed::getRed() {
    return redLED;
}

LED& RGLed::getGreen() {
    return greenLED;
}