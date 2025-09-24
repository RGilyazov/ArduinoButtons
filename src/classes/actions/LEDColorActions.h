#ifndef LED_COLOR_ACTIONS_H
#define LED_COLOR_ACTIONS_H

#include <Arduino.h>
#include "AbstractAction.h"
#include "../leds/RGLed.h"

// Simple action to set LED to red
class LEDRedAction : public AbstractAction {
public:
    explicit LEDRedAction(RGLed* led) : led(led), currentState(ActionState::NOT_STARTED) {}
    
    void start() override {
        if (!isValid()) {
            currentState = ActionState::FAILED;
            return;
        }
        
        currentState = ActionState::IN_PROGRESS;
        led->showRed();
        currentState = ActionState::COMPLETED;  // Instant completion
    }
    
    ActionState update() override { return currentState; }
    void reset() override { currentState = ActionState::NOT_STARTED; }
    void stop() override { currentState = ActionState::FAILED; }
    
    bool isRunning() const override { return currentState == ActionState::IN_PROGRESS; }
    bool isComplete() const override { return currentState == ActionState::COMPLETED; }
    bool hasFailed() const override { return currentState == ActionState::FAILED; }
    ActionState getState() const override { return currentState; }
    bool isValid() const override { return led != nullptr; }

        
    // NEW: Execution behavior - configurable for combined actions
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
};

class LEDYellowAction : public AbstractAction {
public:
    explicit LEDYellowAction(RGLed* led) : led(led), currentState(ActionState::NOT_STARTED) {}
    
    void start() override {
        if (!isValid()) {
            currentState = ActionState::FAILED;
            return;
        }
        
        currentState = ActionState::IN_PROGRESS;
        led-> showYellow();  // Red + Green = Yellow
        currentState = ActionState::COMPLETED;  // Instant completion
    }
    
    ActionState update() override { return currentState; }
    void reset() override { currentState = ActionState::NOT_STARTED; }
    void stop() override { currentState = ActionState::FAILED; }
    
    bool isRunning() const override { return currentState == ActionState::IN_PROGRESS; }
    bool isComplete() const override { return currentState == ActionState::COMPLETED; }
    bool hasFailed() const override { return currentState == ActionState::FAILED; }
    ActionState getState() const override { return currentState; }
    bool isValid() const override { return led != nullptr; }
            
    // NEW: Execution behavior - configurable for combined actions
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
};

// Simple action to set LED to green
class LEDGreenAction : public AbstractAction {
public:
    explicit LEDGreenAction(RGLed* led) : led(led), currentState(ActionState::NOT_STARTED) {}
    
    void start() override {
        if (!isValid()) {
            currentState = ActionState::FAILED;
            return;
        }
        
        currentState = ActionState::IN_PROGRESS;
        led->showGreen();
        currentState = ActionState::COMPLETED;  // Instant completion
    }
    
    ActionState update() override { return currentState; }
    void reset() override { currentState = ActionState::NOT_STARTED; }
    void stop() override { currentState = ActionState::FAILED; }
    
    bool isRunning() const override { return currentState == ActionState::IN_PROGRESS; }
    bool isComplete() const override { return currentState == ActionState::COMPLETED; }
    bool hasFailed() const override { return currentState == ActionState::FAILED; }
    ActionState getState() const override { return currentState; }
    bool isValid() const override { return led != nullptr; }
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
};

#endif
