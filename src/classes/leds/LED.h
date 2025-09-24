#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "ILed.h"

class LED : public ILed {
public:
    LED();
    ~LED() = default;
    
    // Setup
    bool setup(uint8_t pin);
    
    // ILed interface
    bool setState(const LEDState& state) override;
    LEDState getState() const override;
    bool isSetup() const override;
    
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

private:
    uint8_t pin;
    uint8_t rememberedIntensity;            // Stored brightness level
    uint8_t currentIntensity;               // Actual current output state
    bool initialized;                       // Track initialization state
    LEDState currentState;                  // Current state
    
    // Helper method to actually set the hardware
    void applyIntensity(uint8_t intensity);
};

#endif