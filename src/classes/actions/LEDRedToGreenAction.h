#ifndef LED_RED_TO_GREEN_ACTION_H
#define LED_RED_TO_GREEN_ACTION_H

#include <Arduino.h>
#include "ProgressiveAction.h"
#include "../leds/ILed.h"

class LEDRedToGreenAction : public ProgressiveAction {
public:
    explicit LEDRedToGreenAction(ILed* led);
    
    bool isValid() const override;
    
    void setDuration(unsigned long durationMs) { actionDurationMs = durationMs; }
    unsigned long getDuration() const { return actionDurationMs; }
    void setRedPhaseRatio(uint8_t ratio) { redPhasePercent = ratio; }
    
    unsigned long getElapsedTime() const;
    uint8_t getProgressPercent() const;
    bool isInRedPhase() const;
    bool isInGreenPhase() const;
    uint16_t getCurrentFlashInterval() const;
    bool isLEDCurrentlyOn() const;

protected:
    void initializeProgress() override;
    bool stepProgress() override;

private:
    ILed* led;
    unsigned long startTime;
    unsigned long actionDurationMs;
    unsigned long lastFlashTime;
    bool ledCurrentlyOn;
    uint8_t redPhasePercent;
    
    static constexpr unsigned long DEFAULT_DURATION_MS = 8000;
    static constexpr uint8_t DEFAULT_RED_PHASE_PERCENT = 50;
    static constexpr uint16_t SLOWEST_FLASH_MS = 1500;
    static constexpr uint16_t FASTEST_FLASH_MS = 100;
    
    uint16_t calculateCurrentFlashInterval() const;
    uint16_t calculateRedPhaseInterval(uint8_t phaseProgress) const;
    uint16_t calculateGreenPhaseInterval(uint8_t phaseProgress) const;
    void updateFlashing();
    void setRedLED();
    void setGreenLED(); 
    void turnOffLED();
};

#endif