#ifndef LED_RED_TO_GREEN_ACTION_H
#define LED_RED_TO_GREEN_ACTION_H

#include <Arduino.h>
#include "AbstractAction.h"
#include "../leds/RGLed.h"

class LEDRedToGreenAction : public AbstractAction {
public:
    explicit LEDRedToGreenAction(RGLed* led);
    
    // State machine interface
    void start() override;
    ActionState update() override;
    void reset() override;
    void stop() override;
    
    // State inquiry
    bool isRunning() const override;
    bool isComplete() const override;
    bool hasFailed() const override;
    ActionState getState() const override;
    bool isValid() const override;
    
    // Configuration
    void setDuration(unsigned long durationMs) { actionDurationMs = durationMs; }
    unsigned long getDuration() const { return actionDurationMs; }
    void setRedPhaseRatio(uint8_t ratio) { redPhasePercent = ratio; } // 0-100, default 50
    
    // Progress inquiry
    unsigned long getElapsedTime() const;
    uint8_t getProgressPercent() const;
    
    // State inquiry
    bool isInRedPhase() const;
    bool isInGreenPhase() const;
    uint16_t getCurrentFlashInterval() const;
    bool isLEDCurrentlyOn() const;

    ExecutionBehavior getExecutionBehavior() const override {
        return executionBehavior;
    }
    
    void setExecutionBehavior(ExecutionBehavior behavior) {
        executionBehavior = behavior;
    }


private:
    RGLed* led;
    ActionState currentState;
    unsigned long startTime;
    unsigned long actionDurationMs;
    unsigned long lastFlashTime;
    bool ledCurrentlyOn;
    uint8_t redPhasePercent;  // What % of duration is red phase (default 50%)
    
    static constexpr unsigned long DEFAULT_DURATION_MS = 8000; // 8 seconds (was 5)
    static constexpr uint8_t DEFAULT_RED_PHASE_PERCENT = 50;   // 50% red, 50% green
    static constexpr uint16_t SLOWEST_FLASH_MS = 1500;         // Slowest flash interval (was 800)
    static constexpr uint16_t FASTEST_FLASH_MS = 100;          // Fastest flash interval (was 50)
    
    // Helper methods
    uint16_t calculateCurrentFlashInterval() const;
    uint16_t calculateRedPhaseInterval(uint8_t phaseProgress) const;
    uint16_t calculateGreenPhaseInterval(uint8_t phaseProgress) const;
    void updateFlashing();
    void setRedLED();
    void setGreenLED(); 
    void turnOffLED();
    ExecutionBehavior executionBehavior;   
};

#endif