#include "RandomPrintAction.h"

RandomPrintAction::RandomPrintAction()
    : PrintAction(""), messageCount(0) {
    // Initialize all message slots as inactive
    for (uint8_t i = 0; i < MAX_MESSAGES; i++) {
        messagePool[i].reset();
    }
}

bool RandomPrintAction::addMessage(const String& message) {
    if (isFull() || message.length() == 0) {
        #ifdef DEBUG
        Serial.println(F("RandomPrintAction: Cannot add message - pool full or empty message"));
        #endif
        return false;
    }

    // Find first inactive slot
    for (uint8_t i = 0; i < MAX_MESSAGES; i++) {
        if (!messagePool[i].isActive) {
            messagePool[i].message = message;
            messagePool[i].isActive = true;
            messageCount++;
            return true;
        }
    }

    return false;
}

bool RandomPrintAction::addMessage(const __FlashStringHelper* message) {
    return addMessage(String(message));
}

void RandomPrintAction::clear() {
    for (uint8_t i = 0; i < MAX_MESSAGES; i++) {
        messagePool[i].reset();
    }
    messageCount = 0;
}

uint8_t RandomPrintAction::getRandomMessageIndex() const {
    if (isEmpty()) {
        return MAX_MESSAGES; // Invalid index
    }

    // Generate random index among active messages
    uint8_t randomSelection = random(messageCount);
    uint8_t activeCount = 0;

    for (uint8_t i = 0; i < MAX_MESSAGES; i++) {
        if (messagePool[i].isActive) {
            if (activeCount == randomSelection) {
                return i;
            }
            activeCount++;
        }
    }

    return 0; // Fallback to first active message
}

void RandomPrintAction::selectRandomMessage() {
    uint8_t index = getRandomMessageIndex();
    if (index < MAX_MESSAGES && messagePool[index].isActive) {
        // Update parent PrintAction's message
        setMessage(messagePool[index].message);

        #ifdef DEBUG
        Serial.print(F("RandomPrintAction: Selected message - "));
        Serial.println(messagePool[index].message);
        #endif
    }
}

void RandomPrintAction::initializeProgress() {
    // Select random message before initializing
    selectRandomMessage();

    // Call parent's initialization
    PrintAction::initializeProgress();
}

bool RandomPrintAction::isValid() const {
    return !isEmpty();
}
