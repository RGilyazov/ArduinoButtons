/* 
*   BASED ON:
*   4-Way Button By Jeff Saltzman 
*   Enhanced with overflow handling, error checking, and push/pop events
*/

#include "Button.h"
#include <Arduino.h>

Button::Button() 
    : onClick(nullptr), onDoubleClick(nullptr), onHold(nullptr), onLongHold(nullptr),
      onPush(nullptr), onPop(nullptr),  // NEW: Initialize push/pop actions
      buttonPin(255), initialized(false),
      debounceTime(DEFAULT_DEBOUNCE_MS),
      doubleClickGap(DEFAULT_DOUBLE_CLICK_GAP_MS),
      holdTime(DEFAULT_HOLD_TIME_MS),
      longHoldTime(DEFAULT_LONG_HOLD_TIME_MS),
      buttonVal(HIGH), buttonLast(HIGH), DCwaiting(false), DConUp(false),
      singleOK(true), downTime(0), upTime(0), ignoreUp(false),
      waitForUp(false), holdEventPast(false), longHoldEventPast(false),
      previousButtonState(HIGH),  // NEW: Initialize previous state
      lastEventTime(0), lastEventType(NO_EVENT) {
}

Button::~Button() {
    // Cleanup - actions are managed externally, just reset pointers
    onClick = nullptr;
    onDoubleClick = nullptr; 
    onHold = nullptr;
    onLongHold = nullptr;
    onPush = nullptr;     // NEW
    onPop = nullptr;      // NEW
}

bool Button::setup(uint8_t pin) {
    buttonPin = pin;
    
    // Initialize pin
    pinMode(buttonPin, INPUT);
    digitalWrite(buttonPin, HIGH); // Enable internal pull-up
    
    // Initialize state
    buttonVal = digitalRead(buttonPin);
    buttonLast = buttonVal;
    previousButtonState = buttonVal;  // NEW: Initialize previous state
    
    // Reset all timing and state variables
    DCwaiting = false;
    DConUp = false;
    singleOK = true;
    downTime = millis();
    upTime = millis();
    ignoreUp = false;
    waitForUp = false;
    holdEventPast = false;
    longHoldEventPast = false;
    lastEventTime = millis();
    lastEventType = NO_EVENT;
    
    initialized = true;
    return true;
}

bool Button::setOnClickAction(AbstractAction* action) {
    if (action && action->isValid()) {
        onClick = action;
        return true;
    }
    onClick = nullptr;
    return false;
}

bool Button::setOnDoubleClickAction(AbstractAction* action) {
    if (action && action->isValid()) {
        onDoubleClick = action;
        return true;
    }
    onDoubleClick = nullptr;
    return false;
}

bool Button::setOnHoldAction(AbstractAction* action) {
    if (action && action->isValid()) {
        onHold = action;
        return true;
    }
    onHold = nullptr;
    return false;
}

bool Button::setOnLongHoldAction(AbstractAction* action) {
    if (action && action->isValid()) {
        onLongHold = action;
        return true;
    }
    onLongHold = nullptr;
    return false;
}

// NEW: Push/Pop action setters
bool Button::setOnPushAction(AbstractAction* action) {
    if (action && action->isValid()) {
        onPush = action;
        return true;
    }
    onPush = nullptr;
    return false;
}

bool Button::setOnPopAction(AbstractAction* action) {
    if (action && action->isValid()) {
        onPop = action;
        return true;
    }
    onPop = nullptr;
    return false;
}

void Button::setTimingParameters(unsigned long debounce, unsigned long dcGap, 
                                unsigned long hold, unsigned long longHold) {
    if (validateTimingParameters(debounce, dcGap, hold, longHold)) {
        debounceTime = debounce;
        doubleClickGap = dcGap;
        holdTime = hold;
        longHoldTime = longHold;
    }
}

bool Button::validateTimingParameters(unsigned long debounce, unsigned long dcGap,
                                    unsigned long hold, unsigned long longHold) const {
    return (debounce <= MAX_DEBOUNCE_MS &&
            dcGap <= MAX_DOUBLE_CLICK_GAP_MS &&
            hold <= MAX_HOLD_TIME_MS &&
            longHold <= MAX_LONG_HOLD_TIME_MS &&
            hold < longHold);
}

void Button::loop() {
    if (!initialized) {
        return;
    }
    
    uint8_t eventType = checkButton();
    
    switch (eventType) {
        case SINGLE_CLICK:
            clickEvent();
            break;
        case DOUBLE_CLICK:
            doubleClickEvent();
            break;
        case HOLD_EVENT:
            holdEvent();
            break;
        case LONG_HOLD_EVENT:
            longHoldEvent();
            break;
        case PUSH_EVENT:      // NEW
            pushEvent();
            break;
        case POP_EVENT:       // NEW
            popEvent();
            break;
        default:
            break;
    }
}

// Safe elapsed time calculation handling millis() overflow
unsigned long Button::getElapsedTime(unsigned long startTime) const {
    unsigned long currentTime = millis();
    
    // Handle overflow case
    if (currentTime >= startTime) {
        return currentTime - startTime;
    } else {
        // Overflow occurred - calculate correctly
        // Use ~0UL (all bits set) instead of ULONG_MAX for better portability
        return (~0UL - startTime) + currentTime + 1;
    }
}

bool Button::hasTimedOut(unsigned long startTime, unsigned long timeout) const {
    return getElapsedTime(startTime) >= timeout;
}

void Button::executeAction(AbstractAction* action, uint8_t eventType) {
    if (action && action->isValid()) {
        // For push/pop events, don't stop other actions - they can run simultaneously
        if (eventType != PUSH_EVENT && eventType != POP_EVENT) {
            stopAllActions();
        }
        
        // Start the new action
        action->start();
        
        if (action->isRunning()) {
            lastEventTime = millis();
            lastEventType = eventType;
        }
    }
}

void Button::clickEvent() {
    executeAction(onClick, SINGLE_CLICK);
}

void Button::doubleClickEvent() {
    executeAction(onDoubleClick, DOUBLE_CLICK);
}

void Button::holdEvent() {
    executeAction(onHold, HOLD_EVENT);
}

void Button::longHoldEvent() {
    executeAction(onLongHold, LONG_HOLD_EVENT);
}

// NEW: Push/Pop event handlers
void Button::pushEvent() {
    executeAction(onPush, PUSH_EVENT);
}

void Button::popEvent() {
    executeAction(onPop, POP_EVENT);
}

void Button::updateActions() {
    // Update all actions that might be running
    if (onClick && onClick->isRunning()) {
        onClick->update();
    }
    if (onDoubleClick && onDoubleClick->isRunning()) {
        onDoubleClick->update();
    }
    if (onHold && onHold->isRunning()) {
        onHold->update();
    }
    if (onLongHold && onLongHold->isRunning()) {
        onLongHold->update();
    }
    if (onPush && onPush->isRunning()) {      // NEW
        onPush->update();
    }
    if (onPop && onPop->isRunning()) {        // NEW
        onPop->update();
    }
}

bool Button::hasRunningAction() const {
    return (onClick && onClick->isRunning()) ||
           (onDoubleClick && onDoubleClick->isRunning()) ||
           (onHold && onHold->isRunning()) ||
           (onLongHold && onLongHold->isRunning()) ||
           (onPush && onPush->isRunning()) ||          // NEW
           (onPop && onPop->isRunning());              // NEW
}

AbstractAction* Button::getCurrentRunningAction() const {
    if (onClick && onClick->isRunning()) return onClick;
    if (onDoubleClick && onDoubleClick->isRunning()) return onDoubleClick;
    if (onHold && onHold->isRunning()) return onHold;
    if (onLongHold && onLongHold->isRunning()) return onLongHold;
    if (onPush && onPush->isRunning()) return onPush;      // NEW
    if (onPop && onPop->isRunning()) return onPop;        // NEW
    return nullptr;
}

void Button::stopAllActions() {
    if (onClick && onClick->isRunning()) {
        onClick->stop();
    }
    if (onDoubleClick && onDoubleClick->isRunning()) {
        onDoubleClick->stop();
    }
    if (onHold && onHold->isRunning()) {
        onHold->stop();
    }
    if (onLongHold && onLongHold->isRunning()) {
        onLongHold->stop();
    }
    if (onPush && onPush->isRunning()) {      // NEW
        onPush->stop();
    }
    if (onPop && onPop->isRunning()) {        // NEW
        onPop->stop();
    }
}

uint8_t Button::checkButton() {    
    uint8_t event = NO_EVENT;
    buttonVal = digitalRead(buttonPin);
    static bool stateChangeDetected = false;
    
    // NEW: Check for push/pop state changes FIRST (before existing logic)
    if (buttonVal != previousButtonState) {
        // State change detected - check if it's been stable long enough (debounced)
        static unsigned long stateChangeTime = 0;        
        if (!stateChangeDetected) {
            // First detection of state change
            stateChangeTime = millis();
            stateChangeDetected = true;
        } else if (millis() - stateChangeTime >= debounceTime) {
            // State change has been stable for debounce time
            if (previousButtonState == HIGH && buttonVal == LOW) {
                // Button was released, now pressed = PUSH event
                event = PUSH_EVENT;
            } else if (previousButtonState == LOW && buttonVal == HIGH) {
                // Button was pressed, now released = POP event  
                event = POP_EVENT;
            }
            
            // Update previous state and reset detection
            previousButtonState = buttonVal;
            stateChangeDetected = false;
            
            // Return push/pop event immediately (don't process other events)
            if (event == PUSH_EVENT || event == POP_EVENT) {
                return event;
            }
        }
        // If still in debounce period, don't process other events
        return NO_EVENT;
    };
    
    // EXISTING LOGIC CONTINUES (for click, double-click, hold, long-hold)
    // Button pressed down
    if (buttonVal == LOW && buttonLast == HIGH && hasTimedOut(upTime, debounceTime)) {
        downTime = millis();
        ignoreUp = false;
        waitForUp = false;
        singleOK = true;
        holdEventPast = false;
        longHoldEventPast = false;
        
        if (hasTimedOut(upTime, doubleClickGap) == false && DConUp == false && DCwaiting == true) {
            DConUp = true;
        } else {
            DConUp = false;
        }
        DCwaiting = false;
    }
    // Button released
    else if (buttonVal == HIGH && buttonLast == LOW && hasTimedOut(downTime, debounceTime)) {        
        if (!ignoreUp) {
            upTime = millis();
            if (DConUp == false) {
                DCwaiting = true;
            } else {
                event = DOUBLE_CLICK;
                DConUp = false;
                DCwaiting = false;
                singleOK = false;
            }
        }
    }
    // Test for normal click event: doubleClickGap expired
    else if (buttonVal == HIGH && hasTimedOut(upTime, doubleClickGap) && 
             DCwaiting == true && DConUp == false && singleOK == true && event != DOUBLE_CLICK) {
        event = SINGLE_CLICK;
        DCwaiting = false;
    }
    // Test for hold events
    else if (buttonVal == LOW && hasTimedOut(downTime, holdTime)) {
        // Trigger "normal" hold
        if (!holdEventPast) {
            event = HOLD_EVENT;
            waitForUp = true;
            ignoreUp = true;
            DConUp = false;
            DCwaiting = false;
            holdEventPast = true;
        }
        // Trigger "long" hold
        else if (hasTimedOut(downTime, longHoldTime) && !longHoldEventPast) {
            event = LONG_HOLD_EVENT;
            longHoldEventPast = true;
        }
    }
    
    buttonLast = buttonVal;
    return event;
}