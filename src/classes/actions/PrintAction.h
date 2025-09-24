#ifndef PRINT_ACTION_H
#define PRINT_ACTION_H

#include <Arduino.h>
#include "ProgressiveAction.h"

class PrintAction : public ProgressiveAction {
public:
    explicit PrintAction(const String& message);
    explicit PrintAction(const __FlashStringHelper* message);
    
    bool isValid() const override;
    
    void setTypingDelay(uint8_t delayMs) { 
        if (delayMs <= MAX_TYPING_DELAY) {
            typingDelayMs = delayMs; 
        }
    }
    
    uint8_t getTypingDelay() const { return typingDelayMs; }
    
    size_t getCurrentPosition() const { return currentPosition; }
    size_t getTotalLength() const;
    uint8_t getProgressPercent() const;

protected:
    void initializeProgress() override;
    bool stepProgress() override;

private:
    String message;
    bool isFlashString;
    const __FlashStringHelper* flashMessage;
    
    size_t currentPosition;
    unsigned long lastCharTime;
    uint8_t typingDelayMs;
    
    static constexpr uint8_t DEFAULT_TYPING_DELAY = 20;
    static constexpr uint8_t MAX_TYPING_DELAY = 100;
    static constexpr size_t MAX_MESSAGE_LENGTH = 200;
    
    bool validateMessage(const String& msg) const;
    bool isKeyboardReady() const;
    char getCurrentChar() const;
    bool hasMoreCharacters() const;
    void typeCurrentCharacter();
    void sendReturn();
};

#endif