#ifndef ILED_H
#define ILED_H

#include <Arduino.h>

enum class LEDIntent : uint8_t {
    OFF, RED, GREEN, YELLOW, WHITE
};

struct LEDState {
    enum Type { INTENT, PRECISE } type;
    
    union {
        LEDIntent intent;
        
        struct {
            uint8_t values[3];
            uint8_t count;
        } precise;
    };
    
    static LEDState off() { 
        LEDState state;
        state.type = INTENT;
        state.intent = LEDIntent::OFF;
        return state;
    }
    
    static LEDState red() {
        LEDState state;
        state.type = INTENT;
        state.intent = LEDIntent::RED;
        return state;
    }
    
    static LEDState green() {
        LEDState state;
        state.type = INTENT;
        state.intent = LEDIntent::GREEN;
        return state;
    }
    
    static LEDState yellow() {
        LEDState state;
        state.type = INTENT;
        state.intent = LEDIntent::YELLOW;
        return state;
    }
    
    static LEDState white() {
        LEDState state;
        state.type = INTENT;
        state.intent = LEDIntent::WHITE;
        return state;
    }
    
    static LEDState custom(uint8_t r, uint8_t g = 0, uint8_t b = 0) {
        LEDState state;
        state.type = PRECISE;
        state.precise.values[0] = r;
        state.precise.values[1] = g;
        state.precise.values[2] = b;
        state.precise.count = (b > 0) ? 3 : (g > 0) ? 2 : 1;
        return state;
    }
};

class ILed {
public:
    virtual ~ILed() = default;
    virtual bool setState(const LEDState& state) = 0;
    virtual LEDState getState() const = 0;
    virtual bool isSetup() const = 0;
};

#endif