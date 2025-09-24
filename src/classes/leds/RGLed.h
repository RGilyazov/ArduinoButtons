#ifndef RGLED_H
#define RGLED_H

#include <Arduino.h>
#include "LED.h"
#include "ILed.h"

class RGLed : public ILed {
public:
    RGLed();
    ~RGLed() = default;
    
    // Setup
    bool setup(uint8_t redPin, uint8_t greenPin);
    
    // ILed interface
    bool setState(const LEDState& state) override;
    LEDState getState() const override;
    bool isSetup() const override;
    
    // Individual LED control
    void setRed(uint8_t intensity);                 // 0-255
    void setGreen(uint8_t intensity);               // 0-255
    void setRG(uint8_t red, uint8_t green);         // Set both at once
    
    // Predefined colors
    void showRed();                                 // Red only
    void showGreen();                               // Green only
    void showYellow();                              // Red + Green = Yellow/Orange
    void showOff();                                 // Both off
    
    // Control
    void turnOff();                                 // Both off, keep memory
    void turnOn();                                  // Restore last RG combination
    void toggle();                                  // Smart toggle
    
    // State inquiry
    uint8_t getCurrentRed() const;                  // Current red output
    uint8_t getCurrentGreen() const;                // Current green output
    bool isOn() const;                              // Any LED on?
    
    // Direct access
    LED& getRed();                                  // Direct red LED access
    LED& getGreen();                                // Direct green LED access

private:
    LED redLED;
    LED greenLED;
    bool initialized;
    LEDState currentState;
};

#endif