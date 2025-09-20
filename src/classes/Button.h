#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "actions/AbstractAction.h"

class Button {
public:
    Button();
    ~Button(); // Destructor for cleanup
    
    // Setup and main loop
    bool setup(uint8_t buttonPin);
    void loop();
    
    // Action assignments (now using stack allocation)
    bool setOnClickAction(AbstractAction* action);
    bool setOnDoubleClickAction(AbstractAction* action);
    bool setOnHoldAction(AbstractAction* action);
    bool setOnLongHoldAction(AbstractAction* action);
    
    // Getters for current actions
    AbstractAction* getOnClickAction() const { return onClick; }
    AbstractAction* getOnDoubleClickAction() const { return onDoubleClick; }
    AbstractAction* getOnHoldAction() const { return onHold; }
    AbstractAction* getOnLongHoldAction() const { return onLongHold; }
    
    // Configuration
    void setTimingParameters(unsigned long debounce, unsigned long dcGap, 
                           unsigned long hold, unsigned long longHold);
    
    // Status checking
    bool isInitialized() const { return initialized; }
    uint8_t getButtonPin() const { return buttonPin; }
    
    // Debugging/monitoring
    unsigned long getLastEventTime() const { return lastEventTime; }
    uint8_t getLastEventType() const { return lastEventType; }

    // Button timing constants
    static constexpr unsigned long DEFAULT_DEBOUNCE_MS = 20;
    static constexpr unsigned long DEFAULT_DOUBLE_CLICK_GAP_MS = 250;
    static constexpr unsigned long DEFAULT_HOLD_TIME_MS = 1000;
    static constexpr unsigned long DEFAULT_LONG_HOLD_TIME_MS = 3000;
    
    // Maximum reasonable values for validation
    static constexpr unsigned long MAX_DEBOUNCE_MS = 100;
    static constexpr unsigned long MAX_DOUBLE_CLICK_GAP_MS = 1000;
    static constexpr unsigned long MAX_HOLD_TIME_MS = 10000;
    static constexpr unsigned long MAX_LONG_HOLD_TIME_MS = 30000;

private:
    // Action pointers
    AbstractAction* onClick;
    AbstractAction* onDoubleClick;
    AbstractAction* onHold;
    AbstractAction* onLongHold;
    
    // Hardware
    uint8_t buttonPin;
    bool initialized;
    
    // Timing parameters (configurable)
    unsigned long debounceTime;
    unsigned long doubleClickGap;
    unsigned long holdTime;
    unsigned long longHoldTime;
    
    // State variables (volatile for interrupt safety)
    volatile bool buttonVal;
    volatile bool buttonLast;
    volatile bool DCwaiting;
    volatile bool DConUp;
    volatile bool singleOK;
    volatile unsigned long downTime;
    volatile unsigned long upTime;
    volatile bool ignoreUp;
    volatile bool waitForUp;
    volatile bool holdEventPast;
    volatile bool longHoldEventPast;
    
    // Monitoring
    unsigned long lastEventTime;
    uint8_t lastEventType; // 0=none, 1=click, 2=double, 3=hold, 4=longhold
    
    // Event codes
    enum EventType : uint8_t {
        NO_EVENT = 0,
        SINGLE_CLICK = 1,
        DOUBLE_CLICK = 2,
        HOLD_EVENT = 3,
        LONG_HOLD_EVENT = 4
    };
    
    // Private methods
    uint8_t checkButton();
    void executeAction(AbstractAction* action, uint8_t eventType);
    bool validateTimingParameters(unsigned long debounce, unsigned long dcGap,
                                 unsigned long hold, unsigned long longHold) const;
    
    // Timing utilities
    unsigned long getElapsedTime(unsigned long startTime) const;
    bool hasTimedOut(unsigned long startTime, unsigned long timeout) const;
    
    // Event handlers
    void clickEvent();
    void doubleClickEvent();
    void holdEvent();
    void longHoldEvent();
};

#endif