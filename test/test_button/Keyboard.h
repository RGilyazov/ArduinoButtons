#ifndef KEYBOARD_MOCK_H
#define KEYBOARD_MOCK_H

#include "Arduino.h"

// Keyboard key constants
#define KEY_RETURN 0x0A

// Mock Keyboard library
class Keyboard_ {
public:
    void begin() {}
    void end() {}
    void press(uint8_t k) {}
    void release(uint8_t k) {}
    void write(uint8_t k) {}
    void print(const char* str) {}
    void print(const String& str) {}
    void println(const char* str) {}
    void println(const String& str) {}
};

extern Keyboard_ Keyboard;

#endif
