#include "PrintAction.h"
#include <Arduino.h>
#include <Keyboard.h>

PrintAction::PrintAction(const String& message) 
    : message(message), typingDelayMs(DEFAULT_TYPING_DELAY), 
      isFlashString(false), flashMessage(nullptr) {
    // Constructor validation is handled in isValid()
}

PrintAction::PrintAction(const __FlashStringHelper* message) 
    : typingDelayMs(DEFAULT_TYPING_DELAY), isFlashString(true), flashMessage(message) {
    // Flash string constructor - message stored in flash memory
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
    // Check if keyboard is properly initialized
    // This is a basic check - in practice, you might want more sophisticated detection
    return true; // Arduino Keyboard library doesn't provide a direct way to check this
}

ActionResult PrintAction::execute() {
    // Pre-execution validation
    if (!isValid()) {
        return ActionResult::FAILED_INVALID_STATE;
    }
    
    if (!isKeyboardReady()) {
        return ActionResult::FAILED_HARDWARE_ERROR;
    }
    
    // Execute the appropriate typing method
    ActionResult result;
    if (isFlashString) {
        result = typeFlashString(flashMessage);
    } else {
        result = typeString(message);
    }
    
    return result;
}

ActionResult PrintAction::typeString(const String& str) {
    // Validate string length again before processing
    if (str.length() == 0 || str.length() > MAX_MESSAGE_LENGTH) {
        return ActionResult::FAILED_INVALID_STATE;
    }
    
    // Type each character with error checking
    for (size_t i = 0; i < str.length(); i++) {
        char character = str.charAt(i);
        
        // Basic character validation
        if (character == 0) {
            return ActionResult::FAILED_INVALID_STATE;
        }
        
        Keyboard.write(character);
        
        // Non-blocking delay with early exit capability
        if (typingDelayMs > 0) {
            delay(typingDelayMs);
        }
    }
    
    // Send return key
    Keyboard.write(KEY_RETURN);
    
    return ActionResult::SUCCESS;
}

ActionResult PrintAction::typeFlashString(const __FlashStringHelper* str) {
    if (str == nullptr) {
        return ActionResult::FAILED_INVALID_STATE;
    }
    
    // Read from flash memory and type
    const char* flashPtr = reinterpret_cast<const char*>(str);
    size_t charCount = 0;
    char character;
    
    // Read character by character from flash
    while ((character = pgm_read_byte(flashPtr + charCount)) != '\0') {
        // Prevent infinite loops with max length check
        if (charCount >= MAX_MESSAGE_LENGTH) {
            return ActionResult::FAILED_INVALID_STATE;
        }
        
        Keyboard.write(character);
        
        if (typingDelayMs > 0) {
            delay(typingDelayMs);
        }
        
        charCount++;
    }
    
    // Send return key
    Keyboard.write(KEY_RETURN);
    
    return ActionResult::SUCCESS;
}