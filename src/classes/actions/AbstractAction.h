#ifndef ABSTRACT_ACTION_H
#define ABSTRACT_ACTION_H

#include <Arduino.h>

// Action execution state
enum class ActionState : uint8_t {
    NOT_STARTED = 0,    // Action hasn't been started yet
    IN_PROGRESS = 1,    // Action is currently running
    COMPLETED = 2,      // Action completed successfully
    FAILED = 3          // Action failed during execution
};

// Non-blocking action base class
class AbstractAction {
public:
    virtual ~AbstractAction() = default;
    
    // State machine interface
    virtual void start() = 0;                    // Initialize and start the action
    virtual ActionState update() = 0;            // Step forward (non-blocking)
    virtual void reset() = 0;                    // Reset to initial state
    virtual void stop() = 0;                     // Force stop the action
    
    // State inquiry
    virtual bool isRunning() const = 0;          // Check if currently executing
    virtual bool isComplete() const = 0;         // Check if completed successfully
    virtual bool hasFailed() const = 0;          // Check if failed
    virtual ActionState getState() const = 0;    // Get current state
    
    // Validation (kept from original design)
    virtual bool isValid() const = 0;            // Check if action is properly configured
    
    // Utility methods
    bool isIdle() const { return getState() == ActionState::NOT_STARTED; }
    bool isActive() const { return getState() == ActionState::IN_PROGRESS; }
    bool isDone() const { 
        ActionState state = getState();
        return state == ActionState::COMPLETED || state == ActionState::FAILED; 
    }
};

#endif