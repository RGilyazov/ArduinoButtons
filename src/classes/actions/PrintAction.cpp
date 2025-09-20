#include "PrintAction.h"
#include <Arduino.h>
#include <Keyboard.h>

PrintAction::PrintAction(const String& message) 
    : message(message), isFlashString(false), flashMessage(nullptr),
      currentState(ActionState::NOT_STARTED), currentPosition(0), 
      lastCharTime(0), typingDelayMs(DEFAULT_TYPING_DELAY) {
}

PrintAction::PrintAction(const __FlashStringHelper* message) 
    : isFlashString(true), flashMessage(message),
      currentState(ActionState::NOT_STARTED), currentPosition(0),
      lastCharTime(0), typingDelayMs(DEFAULT_TYPING_DELAY) {
}

bool PrintAction::isValid() const {
    if (isFlashString) {
        return flashMessage != nullptr;
    } else {
        return validateMessage(message);
    }
}

bool PrintAction::validateMessage(const String& msg) const {
    return msg.length() > 0 && msg.length() <= MAX_MESSAGE_LENGTH;
}

bool PrintAction::isKeyboardReady() const {
    // Basic check - Arduino Keyboard library doesn't provide direct status
    return true;
}

void PrintAction::start() {
    if (!isValid() || !isKeyboardReady()) {
        currentState = ActionState::FAILED;
        return;
    }
    
    initializeState();
    currentState = ActionState::IN_PROGRESS;
}

ActionState PrintAction::update() {
    if (currentState != ActionState::IN_PROGRESS) {
        return currentState;
    }
    
    // Check if enough time has passed for next character
    unsigned long currentTime = millis();
    if (currentTime - lastCharTime < typingDelayMs) {
        return currentState; // Still waiting for delay
    }
    
    // Type the next character
    if (hasMoreCharacters()) {
        typeCurrentCharacter();
        currentPosition++;
        lastCharTime = currentTime;
    } else {
        // All characters typed, send return
        sendReturn();
        currentState = ActionState::COMPLETED;
    }
    
    return currentState;
}

void PrintAction::reset() {
    currentState = ActionState::NOT_STARTED;
    initializeState();
}

void PrintAction::stop() {
    currentState = ActionState::FAILED;
}

bool PrintAction::isRunning() const {
    return currentState == ActionState::IN_PROGRESS;
}

bool PrintAction::isComplete() const {
    return currentState == ActionState::COMPLETED;
}

bool PrintAction::hasFailed() const {
    return currentState == ActionState::FAILED;
}

ActionState PrintAction::getState() const {
    return currentState;
}

size_t PrintAction::getTotalLength() const {
    if (isFlashString) {
        if (flashMessage == nullptr) return 0;
        
        // Count characters in flash string
        const char* flashPtr = reinterpret_cast<const char*>(flashMessage);
        size_t len = 0;
        while (pgm_read_byte(flashPtr + len) != '\0' && len < MAX_MESSAGE_LENGTH) {
            len++;
        }
        return len;
    } else {
        return message.length();
    }
}

uint8_t PrintAction::getProgressPercent() const {
    size_t totalLen = getTotalLength();
    if (totalLen == 0) return 0;
    
    return (uint8_t)((currentPosition * 100) / totalLen);
}

char PrintAction::getCurrentChar() const {
    if (isFlashString) {
        if (flashMessage == nullptr) return '\0';
        
        const char* flashPtr = reinterpret_cast<const char*>(flashMessage);
        return pgm_read_byte(flashPtr + currentPosition);
    } else {
        if (currentPosition >= message.length()) return '\0';
        return message.charAt(currentPosition);
    }
}

bool PrintAction::hasMoreCharacters() const {
    if (isFlashString) {
        return getCurrentChar() != '\0' && currentPosition < MAX_MESSAGE_LENGTH;
    } else {
        return currentPosition < message.length();
    }
}

void PrintAction::typeCurrentCharacter() {
    char character = getCurrentChar();
    if (character != '\0') {
        Keyboard.write(character);
    }
}

void PrintAction::sendReturn() {
    Keyboard.write(KEY_RETURN);
}

void PrintAction::initializeState() {
    currentPosition = 0;
    lastCharTime = 0;
}