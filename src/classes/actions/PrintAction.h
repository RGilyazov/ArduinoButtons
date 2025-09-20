#ifndef PRINT_ACTION_H
#define PRINT_ACTION_H

#include <Arduino.h>
#include "AbstractAction.h"

class PrintAction : public AbstractAction {
public:
    // Constructor with validation
    explicit PrintAction(const String& message);
    explicit PrintAction(const __FlashStringHelper* message); // For F() strings
    
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
    void setTypingDelay(uint8_t delayMs) { 
        if (delayMs <= MAX_TYPING_DELAY) {
            typingDelayMs = delayMs; 
        }
    }
    
    uint8_t getTypingDelay() const { return typingDelayMs; }
    
    // Progress inquiry
    size_t getCurrentPosition() const { return currentPosition; }
    size_t getTotalLength() const;
    uint8_t getProgressPercent() const;
    
private:
    // Message storage
    String message;
    bool isFlashString;
    const __FlashStringHelper* flashMessage;
    
    // State machine variables
    ActionState currentState;
    size_t currentPosition;         // Current character being typed
    unsigned long lastCharTime;     // When last character was typed
    uint8_t typingDelayMs;         // Delay between characters in milliseconds
    
    // Constants
    static constexpr uint8_t DEFAULT_TYPING_DELAY = 10;
    static constexpr uint8_t MAX_TYPING_DELAY = 100;
    static constexpr size_t MAX_MESSAGE_LENGTH = 200;
    
    // Helper methods
    bool validateMessage(const String& msg) const;
    bool isKeyboardReady() const;
    char getCurrentChar() const;
    bool hasMoreCharacters() const;
    void typeCurrentCharacter();
    void sendReturn();
    void initializeState();
};

#endif