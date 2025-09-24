#ifndef LED_TOGGLE_ACTION_H
#define LED_TOGGLE_ACTION_H

#include <Arduino.h>
#include "AbstractAction.h"
#include "../leds/RGLed.h"

// Simple action that toggles RG LED through red -> green -> yellow sequence
class LEDToggleAction : public AbstractAction {
public:
    explicit LEDToggleAction(RGLed* led);
    
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
    bool isValid() const override { return led != nullptr; }
    
    // State inquiry
    const char* getCurrentColorName() const;
        ExecutionBehavior getExecutionBehavior() const override {
        return executionBehavior;
    }
    
    void setExecutionBehavior(ExecutionBehavior behavior) {
        executionBehavior = behavior;
    }
    
private:
    RGLed* led;
    ActionState currentState;
    ExecutionBehavior executionBehavior;   
    
    bool isCurrentlyRed() const;
    bool isCurrentlyGreen() const;
    bool isCurrentlyYellow() const;
};

// Implementation
LEDToggleAction::LEDToggleAction(RGLed* ledPtr)
    : led(ledPtr), currentState(ActionState::NOT_STARTED) {
}

void LEDToggleAction::start() {
    if (!isValid()) {
        currentState = ActionState::FAILED;
        return;
    }
    
    currentState = ActionState::IN_PROGRESS;
    
    // Toggle logic: Red -> Green -> Yellow -> Red...
    if (isCurrentlyRed()) {
        led->showGreen();       // Red -> Green
    } else if (isCurrentlyGreen()) {
        led->showYellow();      // Green -> Yellow (red + green)
    } else if (isCurrentlyYellow()) {
        led->showRed();         // Yellow -> Red
    } else {
        // Off or unknown state - default to red
        led->showRed();
    }
    
    currentState = ActionState::COMPLETED; // Instant action
}

ActionState LEDToggleAction::update() {
    // This is an instant action - no ongoing updates needed
    return currentState;
}

void LEDToggleAction::reset() {
    currentState = ActionState::NOT_STARTED;
}

void LEDToggleAction::stop() {
    currentState = ActionState::FAILED;
}

bool LEDToggleAction::isCurrentlyRed() const {
    if (!led) return false;
    return led->getCurrentRed() > 0 && led->getCurrentGreen() == 0;
}

bool LEDToggleAction::isCurrentlyGreen() const {
    if (!led) return false;
    return led->getCurrentGreen() > 0 && led->getCurrentRed() == 0;
}

bool LEDToggleAction::isCurrentlyYellow() const {
    if (!led) return false;
    return led->getCurrentRed() > 0 && led->getCurrentGreen() > 0;
}

const char* LEDToggleAction::getCurrentColorName() const {
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
