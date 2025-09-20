#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LED {
public:
    LED();
    ~LED() = default;
    
    // Setup
    void setup(uint8_t pin);
    
    // Control methods
    void setIntensity(uint8_t intensity);   // Remember AND apply (0-255)
    void turnOn();                          // Apply remembered intensity
    void turnOff();                         // Set to 0, keep remembered value
    void toggle();                          // Switch between off/remembered
    
    // State inquiry
    uint8_t getCurrentIntensity() const;    // What's actually output now
    uint8_t getRememberedIntensity() const; // What it will return to on turnOn()
    bool isOn() const;                      // true if currentIntensity > 0
    uint8_t getPin() const;                 // Get the pin number
    bool isSetup() const;                   // Check if LED was properly initialized

private:
    uint8_t pin;
    uint8_t rememberedIntensity;            // Stored brightness level
    uint8_t currentIntensity;               // Actual current output state
    bool initialized;                       // Track initialization state
    
    // Helper method to actually set the hardware
    void applyIntensity(uint8_t intensity);
};

#endif
