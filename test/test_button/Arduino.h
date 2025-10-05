#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <cstring>

// Arduino constants
#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

// PROGMEM macro
#define PROGMEM
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(string_literal))

// Flash string helper
class __FlashStringHelper;

// Arduino String class mock
class String {
private:
    std::string data;

public:
    String() : data("") {}
    String(const char* str) : data(str ? str : "") {}
    String(const std::string& str) : data(str) {}
    String(const __FlashStringHelper* str) : data((const char*)str) {}
    String(int val) : data(std::to_string(val)) {}
    String(unsigned int val) : data(std::to_string(val)) {}
    String(long val) : data(std::to_string(val)) {}
    String(unsigned long val) : data(std::to_string(val)) {}

    const char* c_str() const { return data.c_str(); }
    size_t length() const { return data.length(); }

    String operator+(const String& other) const {
        return String((data + other.data).c_str());
    }

    String operator+(const char* str) const {
        return String((data + std::string(str)).c_str());
    }

    bool operator==(const String& other) const {
        return data == other.data;
    }

    bool operator!=(const String& other) const {
        return data != other.data;
    }

    char operator[](size_t index) const {
        return data[index];
    }

    char charAt(size_t index) const {
        if (index < data.length()) {
            return data[index];
        }
        return '\0';
    }
};

// PROGMEM functions
inline uint8_t pgm_read_byte(const void* addr) {
    return *((const uint8_t*)addr);
}

// Mock Arduino functions (declared here, defined in test file)
extern unsigned long millis();
extern void pinMode(uint8_t pin, uint8_t mode);
extern void digitalWrite(uint8_t pin, uint8_t value);
extern int digitalRead(uint8_t pin);
extern void delay(unsigned long ms);
extern void analogWrite(uint8_t pin, int value);

// Serial mock
class SerialMock {
public:
    void begin(unsigned long baud) {}
    void println(const char* str) {}
    void println(const __FlashStringHelper* str) {}
    void print(const char* str) {}
    void print(const __FlashStringHelper* str) {}
    void print(int val) {}
    void println(int val) {}
};

extern SerialMock Serial;

#endif
