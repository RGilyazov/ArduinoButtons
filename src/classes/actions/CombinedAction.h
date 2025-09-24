#ifndef COMBINED_ACTION_H
#define COMBINED_ACTION_H

#include <Arduino.h>
#include "AbstractAction.h"

class CombinedAction : public AbstractAction {
public:
    CombinedAction();
    ~CombinedAction() = default;
    
    // Action management
    bool addAction(AbstractAction* action);     // Add an action to the combination
    void clearActions();                        // Remove all actions
    uint8_t getActionCount() const;             // Get number of actions
    AbstractAction* getAction(uint8_t index) const;  // Get action by index
    
    // State machine interface
    void start() override;
    ActionState update() override;
    void reset() override;
    void stop() override;
    
    // State inquiry
    bool isRunning() const override;
    bool isComplete() const override;
    bool hasFailed() const override;
    ActionState getState() const override;
    bool isValid() const override;
    
    // NEW: Execution behavior - configurable for combined actions
    ExecutionBehavior getExecutionBehavior() const override {
        return executionBehavior;
    }
    
    void setExecutionBehavior(ExecutionBehavior behavior) {
        executionBehavior = behavior;
    }
    
    // Progress and monitoring
    uint8_t getCompletedActionCount() const;    // How many actions have completed
    uint8_t getRunningActionCount() const;      // How many actions are currently running
    uint8_t getFailedActionCount() const;       // How many actions have failed
    uint8_t getProgressPercent() const;         // Overall progress percentage
    
    // Configuration
    void setStopOnFirstFailure(bool stop) { stopOnFirstFailure = stop; }
    bool getStopOnFirstFailure() const { return stopOnFirstFailure; }
    
private:
    static constexpr uint8_t MAX_ACTIONS = 8;   // Maximum actions in combination
    
    AbstractAction* actions[MAX_ACTIONS];       // Array of action pointers
    uint8_t actionCount;                        // Number of actions added
    ActionState currentState;                   // Current combined state
    bool stopOnFirstFailure;                    // Whether to stop all actions on first failure
    ExecutionBehavior executionBehavior;        // How this combined action should execute
    
    // Helper methods
    void updateState();                         // Calculate combined state from individual actions
    void stopAllActions();                      // Force stop all actions
    bool hasAnyRunningAction() const;           // Check if any action is running
    bool hasAnyFailedAction() const;            // Check if any action failed
    bool areAllActionsComplete() const;         // Check if all actions completed successfully
};

#endif