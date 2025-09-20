#ifndef PRINT_ACTION_H
#define PRINT_ACTION_H

#include <Arduino.h>
#include "AbstractAction.h"

class PrintAction : public AbstractAction {
public:
    // Constructor with validation
    explicit PrintAction(const String& message);
    explicit PrintAction(const __FlashStringHelper* message); // For F() strings
    
    ActionResult execute() override;
    bool isValid() const override;
    
    // Configuration
    void setTypingDelay(uint8_t delayMs) { 
        if (delayMs <= MAX_TYPING_DELAY) {
            typingDelayMs = delayMs; 
        }
    }
    
    uint8_t getTypingDelay() const { return typingDelayMs; }
    
private:
    String message;
    uint8_t typingDelayMs;
    bool isFlashString;
    const __FlashStringHelper* flashMessage;
    
    // Constants
    static constexpr uint8_t DEFAULT_TYPING_DELAY = 10;
    static constexpr uint8_t MAX_TYPING_DELAY = 100;
    static constexpr size_t MAX_MESSAGE_LENGTH = 200;
    
    // Helper methods
    ActionResult typeString(const String& str);
    ActionResult typeFlashString(const __FlashStringHelper* str);
    bool validateMessage(const String& msg) const;
    bool isKeyboardReady() const;
};

#endif