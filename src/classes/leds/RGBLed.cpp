#include "RGBLed.h"

RGBLed::RGBLed() : initialized(false) {
}

void RGBLed::setup(uint8_t redPin, uint8_t greenPin, uint8_t bluePin) {
    redLED.setup(redPin);
    greenLED.setup(greenPin);
    blueLED.setup(bluePin);
    initialized = true;
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

bool RGBLed::isSetup() const {
    return initialized && redLED.isSetup() && greenLED.isSetup() && blueLED.isSetup();
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
