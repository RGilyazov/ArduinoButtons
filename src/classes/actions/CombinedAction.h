#ifndef COMBINED_ACTION_H
#define COMBINED_ACTION_H

#include <Arduino.h>
#include "BaseAction.h"

class CombinedAction : public BaseAction {
public:
    CombinedAction();
    ~CombinedAction() = default;
    
    bool addAction(AbstractAction* action);
    void clearActions();
    uint8_t getActionCount() const;
    AbstractAction* getAction(uint8_t index) const;
    
    void start() override;
    ActionState update() override;
    bool isValid() const override;
    
    uint8_t getCompletedActionCount() const;
    uint8_t getRunningActionCount() const;
    uint8_t getFailedActionCount() const;
    uint8_t getProgressPercent() const;

    void setStopOnFirstFailure(bool stop) { stopOnFirstFailure = stop; }
    bool getStopOnFirstFailure() const { return stopOnFirstFailure; }

    // Execution mode: sequential (one at a time) or parallel (all at once)
    void setSequentialMode(bool sequential) { sequentialMode = sequential; }
    bool isSequentialMode() const { return sequentialMode; }

private:
    static constexpr uint8_t MAX_ACTIONS = 8;
    
    AbstractAction* actions[MAX_ACTIONS];
    uint8_t actionCount;
    bool stopOnFirstFailure;
    bool sequentialMode;
    uint8_t currentActionIndex;  // For sequential mode: which action is currently running
    
    void updateState();
    void stopAllActions();
    bool hasAnyRunningAction() const;
    bool hasAnyFailedAction() const;
    bool areAllActionsComplete() const;
};

#endif