#ifndef RGBLED_H
#define RGBLED_H

#include <Arduino.h>
#include "LED.h"
#include "ILed.h"

class RGBLed : public ILed {
public:
    RGBLed();
    ~RGBLed() = default;
    
    // Setup
    bool setup(uint8_t redPin, uint8_t greenPin, uint8_t bluePin);
    
    // ILed interface
    bool setState(const LEDState& state) override;
    LEDState getState() const override;
    bool isSetup() const override;
    
    // Individual LED control
    void setRed(uint8_t intensity);                 // 0-255
    void setGreen(uint8_t intensity);               // 0-255
    void setBlue(uint8_t intensity);                // 0-255
    void setRGB(uint8_t r, uint8_t g, uint8_t b);   // Set all three
    
    // Predefined colors
    void showRed();                                 // Pure red
    void showGreen();                               // Pure green
    void showBlue();                                // Pure blue
    void showYellow();                              // Red + Green
    void showCyan();                                // Green + Blue  
    void showMagenta();                             // Red + Blue
    void showWhite();                               // All three
    void showOff();                                 // All off
    
    // Control
    void turnOff();                                 // All off, keep memory
    void turnOn();                                  // Restore last RGB combination
    void toggle();                                  // Smart toggle
    
    // State inquiry
    uint8_t getCurrentRed() const;                  // Current red output
    uint8_t getCurrentGreen() const;                // Current green output  
    uint8_t getCurrentBlue() const;                 // Current blue output
    bool isOn() const;                              // Any LED on?
    
    // Direct access
    LED& getRed();                                  // Direct red LED access
    LED& getGreen();                                // Direct green LED access
    LED& getBlue();                                 // Direct blue LED access

private:
    LED redLED;
    LED greenLED;
    LED blueLED;
    bool initialized;
    LEDState currentState;
};

#endif