#include "PrintAction.h"
#include <Arduino.h>
#include <Keyboard.h>

PrintAction::PrintAction(const String& message) 
    : message(message), isFlashString(false), flashMessage(nullptr),
      currentPosition(0), lastCharTime(0), typingDelayMs(DEFAULT_TYPING_DELAY) {
}

PrintAction::PrintAction(const __FlashStringHelper* message) 
    : isFlashString(true), flashMessage(message),
      currentPosition(0), lastCharTime(0), typingDelayMs(DEFAULT_TYPING_DELAY) {
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
    return true;
}

void PrintAction::initializeProgress() {
    currentPosition = 0;
    lastCharTime = millis();
}

bool PrintAction::stepProgress() {
    unsigned long currentTime = millis();
    if (currentTime - lastCharTime < typingDelayMs) {
        return false;
    }
    
    if (hasMoreCharacters()) {
        typeCurrentCharacter();
        currentPosition++;
        lastCharTime = currentTime;
        return false;
    } else {
        sendReturn();
        return true;
    }
}

size_t PrintAction::getTotalLength() const {
    if (isFlashString) {
        if (flashMessage == nullptr) return 0;
        
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