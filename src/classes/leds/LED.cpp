#include "LED.h"

LED::LED() 
    : pin(255), rememberedIntensity(255), currentIntensity(0), initialized(false) {
    currentState = LEDState::off();
}

bool LED::setup(uint8_t ledPin) {
    pin = ledPin;
    pinMode(pin, OUTPUT);
    
    // Initialize LED to off state
    applyIntensity(0);
    currentIntensity = 0;
    
    initialized = true;
    return true;
}

bool LED::setState(const LEDState& state) {
    if (!initialized) return false;
    
    currentState = state;
    
    if (state.type == LEDState::INTENT) {
        switch (state.intent) {
            case LEDIntent::OFF:
                turnOff();
                break;
            case LEDIntent::RED:
            case LEDIntent::GREEN:
            case LEDIntent::YELLOW:
            case LEDIntent::WHITE:
                turnOn();
                break;
        }
    } else {
        if (state.precise.count >= 1) {
            setIntensity(state.precise.values[0]);
        }
    }
    
    return true;
}

LEDState LED::getState() const {
    return currentState;
}

bool LED::isSetup() const {
    return initialized;
}

void LED::setIntensity(uint8_t intensity) {
    if (!initialized) return;
    
    // Remember the intensity for future turnOn() calls
    rememberedIntensity = intensity;
    
    // Apply it immediately
    applyIntensity(intensity);
    currentIntensity = intensity;
}

void LED::turnOn() {
    if (!initialized) return;
    
    // Apply the remembered intensity
    applyIntensity(rememberedIntensity);
    currentIntensity = rememberedIntensity;
}

void LED::turnOff() {
    if (!initialized) return;
    
    // Turn off but keep remembered intensity unchanged
    applyIntensity(0);
    currentIntensity = 0;
}

void LED::toggle() {
    if (!initialized) return;
    
    if (currentIntensity > 0) {
        turnOff();
    } else {
        turnOn();
    }
}

uint8_t LED::getCurrentIntensity() const {
    return currentIntensity;
}

uint8_t LED::getRememberedIntensity() const {
    return rememberedIntensity;
}

bool LED::isOn() const {
    return currentIntensity > 0;
}

uint8_t LED::getPin() const {
    return pin;
}

void LED::applyIntensity(uint8_t intensity) {
    if (!initialized) return;
    
    if (intensity == 0) {
        // Digital write for complete off (more reliable than analogWrite(0))
        digitalWrite(pin, LOW);
    } else if (intensity == 255) {
        // Digital write for full on (more reliable than analogWrite(255))
        digitalWrite(pin, HIGH);
    } else {
        // Use PWM for intermediate values
        analogWrite(pin, intensity);
    }
}