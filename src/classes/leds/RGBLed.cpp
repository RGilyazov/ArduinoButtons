#include "RGBLed.h"

RGBLed::RGBLed() : initialized(false) {
    currentState = LEDState::off();
}

bool RGBLed::setup(uint8_t redPin, uint8_t greenPin, uint8_t bluePin) {
    bool redOk = redLED.setup(redPin);
    bool greenOk = greenLED.setup(greenPin);
    bool blueOk = blueLED.setup(bluePin);
    initialized = redOk && greenOk && blueOk;
    return initialized;
}

bool RGBLed::setState(const LEDState& state) {
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
                showWhite();
                break;
        }
    } else {
        uint8_t r = state.precise.count >= 1 ? state.precise.values[0] : 0;
        uint8_t g = state.precise.count >= 2 ? state.precise.values[1] : 0;
        uint8_t b = state.precise.count >= 3 ? state.precise.values[2] : 0;
        setRGB(r, g, b);
    }
    
    return true;
}

LEDState RGBLed::getState() const {
    return currentState;
}

bool RGBLed::isSetup() const {
    return initialized && redLED.isSetup() && greenLED.isSetup() && blueLED.isSetup();
}

void RGBLed::setRed(uint8_t intensity) {
    if (!initialized) return;
    redLED.setIntensity(intensity);
}

void RGBLed::setGreen(uint8_t intensity) {
    if (!initialized) return;
    greenLED.setIntensity(intensity);
}

void RGBLed::setBlue(uint8_t intensity) {
    if (!initialized) return;
    blueLED.setIntensity(intensity);
}

void RGBLed::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    if (!initialized) return;
    redLED.setIntensity(r);
    greenLED.setIntensity(g);
    blueLED.setIntensity(b);
}

void RGBLed::showRed() {
    if (!initialized) return;
    redLED.turnOn();
    greenLED.turnOff();
    blueLED.turnOff();
}

void RGBLed::showGreen() {
    if (!initialized) return;
    redLED.turnOff();
    greenLED.turnOn();
    blueLED.turnOff();
}

void RGBLed::showBlue() {
    if (!initialized) return;
    redLED.turnOff();
    greenLED.turnOff();
    blueLED.turnOn();
}

void RGBLed::showYellow() {
    if (!initialized) return;
    redLED.turnOn();
    greenLED.turnOn();
    blueLED.turnOff();
}

void RGBLed::showCyan() {
    if (!initialized) return;
    redLED.turnOff();
    greenLED.turnOn();
    blueLED.turnOn();
}

void RGBLed::showMagenta() {
    if (!initialized) return;
    redLED.turnOn();
    greenLED.turnOff();
    blueLED.turnOn();
}

void RGBLed::showWhite() {
    if (!initialized) return;
    redLED.turnOn();
    greenLED.turnOn();
    blueLED.turnOn();
}

void RGBLed::showOff() {
    if (!initialized) return;
    redLED.turnOff();
    greenLED.turnOff();
    blueLED.turnOff();
}

void RGBLed::turnOff() {
    if (!initialized) return;
    redLED.turnOff();
    greenLED.turnOff();
    blueLED.turnOff();
}

void RGBLed::turnOn() {
    if (!initialized) return;
    redLED.turnOn();
    greenLED.turnOn();
    blueLED.turnOn();
}

void RGBLed::toggle() {
    if (!initialized) return;
    
    // Toggle all LEDs - if any is on, turn all off
    // If all are off, turn all on to remembered intensities
    if (isOn()) {
        turnOff();
    } else {
        turnOn();
    }
}

uint8_t RGBLed::getCurrentRed() const {
    return redLED.getCurrentIntensity();
}

uint8_t RGBLed::getCurrentGreen() const {
    return greenLED.getCurrentIntensity();
}

uint8_t RGBLed::getCurrentBlue() const {
    return blueLED.getCurrentIntensity();
}

bool RGBLed::isOn() const {
    return redLED.isOn() || greenLED.isOn() || blueLED.isOn();
}

LED& RGBLed::getRed() {
    return redLED;
}

LED& RGBLed::getGreen() {
    return greenLED;
}

LED& RGBLed::getBlue() {
    return blueLED;
}