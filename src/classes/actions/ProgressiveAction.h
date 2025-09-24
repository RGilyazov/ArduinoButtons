#ifndef PROGRESSIVE_ACTION_H
#define PROGRESSIVE_ACTION_H

#include "BaseAction.h"

class ProgressiveAction : public BaseAction {
public:
    ProgressiveAction() = default;
    virtual ~ProgressiveAction() = default;
    
    void start() override {
        if (!isValid()) {
            currentState = ActionState::FAILED;
            return;
        }
        
        initializeProgress();
        currentState = ActionState::IN_PROGRESS;
    }
    
    ActionState update() override {
        if (currentState != ActionState::IN_PROGRESS) {
            return currentState;
        }
        
        if (stepProgress()) {
            currentState = ActionState::COMPLETED;
        }
        
        return currentState;
    }

protected:
    virtual void initializeProgress() = 0;
    virtual bool stepProgress() = 0;
};

#endif