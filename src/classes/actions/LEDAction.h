#ifndef LED_ACTION_H
#define LED_ACTION_H

#include <Arduino.h>
#include "AbstractAction.h"
#include "../leds/RGLed.h"

// Example of a non-blocking LED action that can be combined with other actions
class LEDAction : public AbstractAction {
public:
    enum LEDEffect : uint8_t {
        SOLID_COLOR,        // Just turn on a solid color
        BLINK,             // Blink the LED
        FADE,              // Fade in/out effect
        SEQUENCE           // Color sequence (red->yellow->green->off)
    };
    
    // Constructors for different effects
    LEDAction(RGLed* led, LEDEffect effect, uint16_t durationMs = 1000);
    LEDAction(RGLed* led, uint8_t red, uint8_t green, uint16_t durationMs = 1000); // Solid color
    LEDAction(RGLed* led, LEDEffect effect, uint16_t onTimeMs, uint16_t offTimeMs, uint8_t blinkCount = 3); // Blink
    
    // State machine interface
    void start() override;
    ActionState update() override;
    void reset() override;
    void stop() override;
    
    // State inquiry
    bool isRunning() const override { return currentState == ActionState::IN_PROGRESS; }
    bool isComplete() const override { return currentState == ActionState::COMPLETED; }
    bool hasFailed() const override { return currentState == ActionState::FAILED; }
    ActionState getState() const override { return currentState; }
    bool isValid() const override { return led != nullptr && led->isSetup(); }
    
    // Progress tracking
    uint8_t getProgressPercent() const;
    uint16_t getRemainingTimeMs() const;
    
private:
    RGLed* led;
    LEDEffect effect;
    ActionState currentState;
    
    // Timing
    unsigned long startTime;
    unsigned long lastUpdateTime;
    uint16_t totalDurationMs;
    uint16_t onTimeMs;
    uint16_t offTimeMs;
    
    // Color settings
    uint8_t targetRed;
    uint8_t targetGreen;
    
    // Effect state
    uint8_t currentStep;        // Current step in multi-step effects
    uint8_t maxSteps;           // Total steps for effect
    bool ledIsOn;               // Current LED state for blink effect
    uint8_t blinkCount;         // Number of blinks to do
    uint8_t blinksCompleted;    // Blinks completed so far
    
    // Helper methods
    void updateSolidColor();
    void updateBlink();
    void updateFade();
    void updateSequence();
    void setLEDColor(uint8_t red, uint8_t green);
    unsigned long getElapsedTime() const;
};

// Implementation
LEDAction::LEDAction(RGLed* ledPtr, LEDEffect eff, uint16_t duration)
    : led(ledPtr), effect(eff), currentState(ActionState::NOT_STARTED),
      startTime(0), lastUpdateTime(0), totalDurationMs(duration),
      onTimeMs(500), offTimeMs(500), targetRed(255), targetGreen(0),
      currentStep(0), maxSteps(1), ledIsOn(false), blinkCount(3), blinksCompleted(0) {
}

LEDAction::LEDAction(RGLed* ledPtr, uint8_t red, uint8_t green, uint16_t duration)
    : led(ledPtr), effect(SOLID_COLOR), currentState(ActionState::NOT_STARTED),
      startTime(0), lastUpdateTime(0), totalDurationMs(duration),
      onTimeMs(500), offTimeMs(500), targetRed(red), targetGreen(green),
      currentStep(0), maxSteps(1), ledIsOn(false), blinkCount(3), blinksCompleted(0) {
}

LEDAction::LEDAction(RGLed* ledPtr, LEDEffect eff, uint16_t onTime, uint16_t offTime, uint8_t blinks)
    : led(ledPtr), effect(eff), currentState(ActionState::NOT_STARTED),
      startTime(0), lastUpdateTime(0), totalDurationMs(0),
      onTimeMs(onTime), offTimeMs(offTime), targetRed(255), targetGreen(0),
      currentStep(0), maxSteps(1), ledIsOn(false), blinkCount(blinks), blinksCompleted(0) {
    
    // Calculate total duration for blink effect
    totalDurationMs = (onTime + offTime) * blinks;
}

void LEDAction::start() {
    if (!isValid()) {
        currentState = ActionState::FAILED;
        return;
    }
    
    startTime = millis();
    lastUpdateTime = startTime;
    currentStep = 0;
    blinksCompleted = 0;
    ledIsOn = false;
    currentState = ActionState::IN_PROGRESS;
    
    // Set max steps based on effect
    switch (effect) {
        case SOLID_COLOR:
            maxSteps = 1;
            break;
        case BLINK:
            maxSteps = blinkCount * 2; // On and off for each blink
            break;
        case FADE:
            maxSteps = 20; // 20 fade steps
            break;
        case SEQUENCE:
            maxSteps = 4; // Red, Yellow, Green, Off
            break;
    }
}

ActionState LEDAction::update() {
    if (currentState != ActionState::IN_PROGRESS) {
        return currentState;
    }
    
    unsigned long elapsed = getElapsedTime();
    
    // Check if effect is complete
    if (elapsed >= totalDurationMs && totalDurationMs > 0) {
        led->turnOff();
        currentState = ActionState::COMPLETED;
        return currentState;
    }
    
    // Update based on effect type
    switch (effect) {
        case SOLID_COLOR:
            updateSolidColor();
            break;
        case BLINK:
            updateBlink();
            break;
        case FADE:
            updateFade();
            break;
        case SEQUENCE:
            updateSequence();
            break;
    }
    
    lastUpdateTime = millis();
    return currentState;
}

void LEDAction::reset() {
    currentState = ActionState::NOT_STARTED;
    if (led) led->turnOff();
}

void LEDAction::stop() {
    currentState = ActionState::FAILED;
    if (led) led->turnOff();
}

uint8_t LEDAction::getProgressPercent() const {
    if (totalDurationMs == 0 || currentState == ActionState::NOT_STARTED) {
        return 0;
    }
    
    unsigned long elapsed = getElapsedTime();
    if (elapsed >= totalDurationMs) {
        return 100;
    }
    
    return (uint8_t)((elapsed * 100) / totalDurationMs);
}

uint16_t LEDAction::getRemainingTimeMs() const {
    if (totalDurationMs == 0 || currentState != ActionState::IN_PROGRESS) {
        return 0;
    }
    
    unsigned long elapsed = getElapsedTime();
    if (elapsed >= totalDurationMs) {
        return 0;
    }
    
    return totalDurationMs - elapsed;
}

void LEDAction::updateSolidColor() {
    // Simple: just set the color once and wait
    if (currentStep == 0) {
        setLEDColor(targetRed, targetGreen);
        currentStep = 1;
    }
}

void LEDAction::updateBlink() {
    unsigned long elapsed = getElapsedTime();
    uint16_t cycleDuration = onTimeMs + offTimeMs;
    uint16_t currentCycleTime = elapsed % cycleDuration;
    
    bool shouldBeOn = currentCycleTime < onTimeMs;
    
    if (shouldBeOn != ledIsOn) {
        ledIsOn = shouldBeOn;
        if (ledIsOn) {
            setLEDColor(targetRed, targetGreen);
        } else {
            led->turnOff();
            blinksCompleted++;
        }
    }
    
    // Check if we've completed all blinks
    if (blinksCompleted >= blinkCount && !ledIsOn) {
        currentState = ActionState::COMPLETED;
    }
}

void LEDAction::updateFade() {
    // Simple fade in/out over the duration
    uint8_t fadeStep = (uint8_t)((getElapsedTime() * maxSteps) / totalDurationMs);
    if (fadeStep >= maxSteps) fadeStep = maxSteps - 1;
    
    uint8_t intensity;
    if (fadeStep < maxSteps / 2) {
        // Fade in
        intensity = (uint8_t)((fadeStep * 255) / (maxSteps / 2));
    } else {
        // Fade out  
        intensity = (uint8_t)(((maxSteps - fadeStep) * 255) / (maxSteps / 2));
    }
    
    uint8_t red = (uint8_t)((targetRed * intensity) / 255);
    uint8_t green = (uint8_t)((targetGreen * intensity) / 255);
    setLEDColor(red, green);
}

void LEDAction::updateSequence() {
    // Change color every quarter of the duration
    uint8_t sequenceStep = (uint8_t)((getElapsedTime() * 4) / totalDurationMs);
    if (sequenceStep >= 4) sequenceStep = 3;
    
    if (sequenceStep != currentStep) {
        currentStep = sequenceStep;
        switch (sequenceStep) {
            case 0: led->showRed(); break;      // Red
            case 1: led->showYellow(); break;   // Yellow  
            case 2: led->showGreen(); break;    // Green
            case 3: led->turnOff(); break;      // Off
        }
    }
}

void LEDAction::setLEDColor(uint8_t red, uint8_t green) {
    if (led) {
        led->setRG(red, green);
    }
}

unsigned long LEDAction::getElapsedTime() const {
    return millis() - startTime;
}

#endif
