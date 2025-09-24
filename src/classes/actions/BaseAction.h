#ifndef BASE_ACTION_H
#define BASE_ACTION_H

#include "AbstractAction.h"

class BaseAction : public AbstractAction {
public:
    BaseAction() : currentState(ActionState::NOT_STARTED), 
                   executionBehavior(ExecutionBehavior::IMMEDIATE_PARALLEL) {}
    
    virtual ~BaseAction() = default;
    
    // Common state inquiry methods
    bool isRunning() const override { return currentState == ActionState::IN_PROGRESS; }
    bool isComplete() const override { return currentState == ActionState::COMPLETED; }
    bool hasFailed() const override { return currentState == ActionState::FAILED; }
    ActionState getState() const override { return currentState; }
    
    // Common execution behavior methods
    ExecutionBehavior getExecutionBehavior() const override { return executionBehavior; }
    void setExecutionBehavior(ExecutionBehavior behavior) { executionBehavior = behavior; }
    
    // Default implementations
    void reset() override { currentState = ActionState::NOT_STARTED; }
    void stop() override { currentState = ActionState::FAILED; }

protected:
    ActionState currentState;
    ExecutionBehavior executionBehavior;
};

#endif