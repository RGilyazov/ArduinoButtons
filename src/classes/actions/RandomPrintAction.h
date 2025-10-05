#ifndef RANDOM_PRINT_ACTION_H
#define RANDOM_PRINT_ACTION_H

#include "PrintAction.h"
#include <Arduino.h>

class RandomPrintAction : public PrintAction {
public:
    RandomPrintAction();
    ~RandomPrintAction() = default;

    // Add message to the pool
    bool addMessage(const String& message);
    bool addMessage(const __FlashStringHelper* message);

    // Remove all messages
    void clear();

    // Get current message count
    uint8_t getMessageCount() const { return messageCount; }

    // Check if pool is full
    bool isFull() const { return messageCount >= MAX_MESSAGES; }
    bool isEmpty() const { return messageCount == 0; }

    // Override to validate messages exist
    bool isValid() const override;

protected:
    // Override to select random message before starting
    void initializeProgress() override;

private:
    static constexpr uint8_t MAX_MESSAGES = 10;

    struct MessageSlot {
        String message;
        bool isActive;

        MessageSlot() : message(""), isActive(false) {}

        void reset() {
            message = "";
            isActive = false;
        }
    };

    MessageSlot messagePool[MAX_MESSAGES];
    uint8_t messageCount;

    // Select and set random message
    void selectRandomMessage();
    uint8_t getRandomMessageIndex() const;
};

#endif
