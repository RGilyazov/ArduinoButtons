#include "LEDRedToGreenAction.h"

LEDRedToGreenAction::LEDRedToGreenAction(RGLed* ledPtr)
    : led(ledPtr), currentState(ActionState::NOT_STARTED), 
      startTime(0), actionDurationMs(DEFAULT_DURATION_MS),
      lastFlashTime(0), ledCurrentlyOn(false), 
      redPhasePercent(DEFAULT_RED_PHASE_PERCENT) {
}

void LEDRedToGreenAction::start() {
    if (!isValid()) {
        currentState = ActionState::FAILED;
        return;
    }
    
    // Turn off LED initially
    turnOffLED();
    
    // Record start time
    startTime = millis();
    lastFlashTime = startTime;
    ledCurrentlyOn = false;
    currentState = ActionState::IN_PROGRESS;
}

ActionState LEDRedToGreenAction::update() {
    if (currentState != ActionState::IN_PROGRESS) {
        return currentState;
    }
    
    unsigned long elapsed = getElapsedTime();
    
    // Check if action duration has elapsed
    if (elapsed >= actionDurationMs) {
        // End with solid green LED
        setGreenLED();
        ledCurrentlyOn = true;
        currentState = ActionState::COMPLETED;
        return currentState;
    }
    
    // Update flashing pattern
    updateFlashing();
    
    return currentState;
}

void LEDRedToGreenAction::updateFlashing() {
    unsigned long currentTime = millis();
    uint16_t flashInterval = calculateCurrentFlashInterval();
    
    // Check if it's time to toggle LED
    if (currentTime - lastFlashTime >= flashInterval) {
        if (ledCurrentlyOn) {
            // Turn LED off
            turnOffLED();
            ledCurrentlyOn = false;
        } else {
            // Turn appropriate color on
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
        // Red phase: 0% to redPhasePercent% of total duration
        uint8_t redProgress = (progress * 100) / redPhasePercent;
        if (redProgress > 100) redProgress = 100;
        return calculateRedPhaseInterval(redProgress);
    } else {
        // Green phase: redPhasePercent% to 100% of total duration
        uint8_t greenStart = redPhasePercent;
        uint8_t greenRange = 100 - redPhasePercent;
        uint8_t greenProgress = ((progress - greenStart) * 100) / greenRange;
        if (greenProgress > 100) greenProgress = 100;
        return calculateGreenPhaseInterval(greenProgress);
    }
}

uint16_t LEDRedToGreenAction::calculateRedPhaseInterval(uint8_t phaseProgress) const {
    // Red phase: Start very slow, get moderately faster
    // Very slow flash (1500ms) → Medium-slow flash (400ms) over 50% of duration
    uint16_t startInterval = SLOWEST_FLASH_MS;    // 1500ms
    uint16_t endInterval = 400;                   // 400ms (medium-slow speed)
    
    // Linear interpolation from very slow to medium-slow speed
    return startInterval - ((startInterval - endInterval) * phaseProgress) / 100;
}

uint16_t LEDRedToGreenAction::calculateGreenPhaseInterval(uint8_t phaseProgress) const {
    // Green phase: Start medium-slow, get moderately fast
    // Medium-slow flash (400ms) → Moderately fast flash (100ms) over last 50% of duration
    uint16_t startInterval = 400;                 // 400ms (medium-slow speed)
    uint16_t endInterval = FASTEST_FLASH_MS;      // 100ms (moderately fast)
    
    // Linear interpolation from medium-slow to moderately fast
    return startInterval - ((startInterval - endInterval) * phaseProgress) / 100;
}

void LEDRedToGreenAction::reset() {
    currentState = ActionState::NOT_STARTED;
    startTime = 0;
    lastFlashTime = 0;
    ledCurrentlyOn = false;
}

void LEDRedToGreenAction::stop() {
    turnOffLED();
    currentState = ActionState::FAILED;
}

bool LEDRedToGreenAction::isRunning() const {
    return currentState == ActionState::IN_PROGRESS;
}

bool LEDRedToGreenAction::isComplete() const {
    return currentState == ActionState::COMPLETED;
}

bool LEDRedToGreenAction::hasFailed() const {
    return currentState == ActionState::FAILED;
}

ActionState LEDRedToGreenAction::getState() const {
    return currentState;
}

bool LEDRedToGreenAction::isValid() const {
    return led != nullptr && led->isSetup();
}

unsigned long LEDRedToGreenAction::getElapsedTime() const {
    if (currentState == ActionState::NOT_STARTED) {
        return 0;
    }
    
    unsigned long currentTime = millis();
    
    // Handle millis() overflow
    if (currentTime >= startTime) {
        return currentTime - startTime;
    } else {
        // Overflow occurred
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
        led->showRed();
    }
}

void LEDRedToGreenAction::setGreenLED() {
    if (led) {
        led->showGreen();
    }
}

void LEDRedToGreenAction::turnOffLED() {
    if (led) {
        led->turnOff();
    }
}