#ifndef INSTANT_ACTION_H
#define INSTANT_ACTION_H

#include "BaseAction.h"

class InstantAction : public BaseAction {
public:
    InstantAction() = default;
    virtual ~InstantAction() = default;
    
    void start() override {
        if (!isValid()) {
            currentState = ActionState::FAILED;
            return;
        }
        
        currentState = ActionState::IN_PROGRESS;
        execute();
        currentState = ActionState::COMPLETED;
    }
    
    ActionState update() override { return currentState; }
    
protected:
    virtual void execute() = 0;
};

#endif