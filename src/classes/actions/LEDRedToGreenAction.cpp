#include "LEDRedToGreenAction.h"

LEDRedToGreenAction::LEDRedToGreenAction(ILed* ledPtr)
    : led(ledPtr), startTime(0), actionDurationMs(DEFAULT_DURATION_MS),
      lastFlashTime(0), ledCurrentlyOn(false), 
      redPhasePercent(DEFAULT_RED_PHASE_PERCENT) {
}

bool LEDRedToGreenAction::isValid() const {
    return led != nullptr && led->isSetup();
}

void LEDRedToGreenAction::initializeProgress() {
    turnOffLED();
    startTime = millis();
    lastFlashTime = startTime;
    ledCurrentlyOn = false;
}

bool LEDRedToGreenAction::stepProgress() {
    unsigned long elapsed = getElapsedTime();
    
    if (elapsed >= actionDurationMs) {
        setGreenLED();
        ledCurrentlyOn = true;
        return true;
    }
    
    updateFlashing();
    return false;
}

void LEDRedToGreenAction::updateFlashing() {
    unsigned long currentTime = millis();
    uint16_t flashInterval = calculateCurrentFlashInterval();
    
    if (currentTime - lastFlashTime >= flashInterval) {
        if (ledCurrentlyOn) {
            turnOffLED();
            ledCurrentlyOn = false;
        } else {
            if (isInRedPhase()) {
                setRedLED();
            } else {
                setGreenLED();
            }
            ledCurrentlyOn = true;
        }
        
        lastFlashTime = currentTime;
    }
}

uint16_t LEDRedToGreenAction::calculateCurrentFlashInterval() const {
    uint8_t progress = getProgressPercent();
    
    if (isInRedPhase()) {
        uint8_t redProgress = (progress * 100) / redPhasePercent;
        if (redProgress > 100) redProgress = 100;
        return calculateRedPhaseInterval(redProgress);
    } else {
        uint8_t greenStart = redPhasePercent;
        uint8_t greenRange = 100 - redPhasePercent;
        uint8_t greenProgress = ((progress - greenStart) * 100) / greenRange;
        if (greenProgress > 100) greenProgress = 100;
        return calculateGreenPhaseInterval(greenProgress);
    }
}

uint16_t LEDRedToGreenAction::calculateRedPhaseInterval(uint8_t phaseProgress) const {
    uint16_t startInterval = SLOWEST_FLASH_MS;
    uint16_t endInterval = 400;
    
    return startInterval - ((startInterval - endInterval) * phaseProgress) / 100;
}

uint16_t LEDRedToGreenAction::calculateGreenPhaseInterval(uint8_t phaseProgress) const {
    uint16_t startInterval = 400;
    uint16_t endInterval = FASTEST_FLASH_MS;
    
    return startInterval - ((startInterval - endInterval) * phaseProgress) / 100;
}

unsigned long LEDRedToGreenAction::getElapsedTime() const {
    if (currentState == ActionState::NOT_STARTED) {
        return 0;
    }
    
    unsigned long currentTime = millis();
    
    if (currentTime >= startTime) {
        return currentTime - startTime;
    } else {
        return (~0UL - startTime) + currentTime + 1;
    }
}

uint8_t LEDRedToGreenAction::getProgressPercent() const {
    if (currentState == ActionState::NOT_STARTED) {
        return 0;
    }
    if (currentState == ActionState::COMPLETED) {
        return 100;
    }
    
    unsigned long elapsed = getElapsedTime();
    if (elapsed >= actionDurationMs) {
        return 100;
    }
    
    return (uint8_t)((elapsed * 100) / actionDurationMs);
}

bool LEDRedToGreenAction::isInRedPhase() const {
    return getProgressPercent() < redPhasePercent;
}

bool LEDRedToGreenAction::isInGreenPhase() const {
    return getProgressPercent() >= redPhasePercent;
}

uint16_t LEDRedToGreenAction::getCurrentFlashInterval() const {
    return calculateCurrentFlashInterval();
}

bool LEDRedToGreenAction::isLEDCurrentlyOn() const {
    return ledCurrentlyOn;
}

void LEDRedToGreenAction::setRedLED() {
    if (led) {
        led->setState(LEDState::red());
    }
}

void LEDRedToGreenAction::setGreenLED() {
    if (led) {
        led->setState(LEDState::green());
    }
}

void LEDRedToGreenAction::turnOffLED() {
    if (led) {
        led->setState(LEDState::off());
    }
}