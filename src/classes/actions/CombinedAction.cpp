#include "CombinedAction.h"

CombinedAction::CombinedAction() 
    : actionCount(0), stopOnFirstFailure(true) {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        actions[i] = nullptr;
    }
}

bool CombinedAction::addAction(AbstractAction* action) {
    if (action == nullptr || !action->isValid()) {
        return false;
    }
    
    if (actionCount >= MAX_ACTIONS) {
        return false;
    }
    
    actions[actionCount] = action;
    actionCount++;
    return true;
}

void CombinedAction::clearActions() {
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr) {
            actions[i]->reset();
            actions[i] = nullptr;
        }
    }
    actionCount = 0;
    currentState = ActionState::NOT_STARTED;
}

uint8_t CombinedAction::getActionCount() const {
    return actionCount;
}

AbstractAction* CombinedAction::getAction(uint8_t index) const {
    if (index >= actionCount) {
        return nullptr;
    }
    return actions[index];
}

void CombinedAction::start() {
    if (actionCount == 0 || !isValid()) {
        currentState = ActionState::FAILED;
        return;
    }
    
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr) {
            actions[i]->start();
        }
    }
    
    currentState = ActionState::IN_PROGRESS;
}

ActionState CombinedAction::update() {
    if (currentState != ActionState::IN_PROGRESS) {
        return currentState;
    }
    
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && actions[i]->isActive()) {
            actions[i]->update();
        }
    }
    
    if (stopOnFirstFailure && hasAnyFailedAction()) {
        stopAllActions();
        currentState = ActionState::FAILED;
        return currentState;
    }
    
    updateState();
    return currentState;
}

bool CombinedAction::isValid() const {
    if (actionCount == 0) {
        return false;
    }
    
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] == nullptr || !actions[i]->isValid()) {
            return false;
        }
    }
    
    return true;
}

uint8_t CombinedAction::getCompletedActionCount() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && actions[i]->isComplete()) {
            count++;
        }
    }
    return count;
}

uint8_t CombinedAction::getRunningActionCount() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && actions[i]->isRunning()) {
            count++;
        }
    }
    return count;
}

uint8_t CombinedAction::getFailedActionCount() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && actions[i]->hasFailed()) {
            count++;
        }
    }
    return count;
}

uint8_t CombinedAction::getProgressPercent() const {
    if (actionCount == 0) return 0;
    
    uint8_t completedCount = getCompletedActionCount();
    return (uint8_t)((completedCount * 100) / actionCount);
}

void CombinedAction::updateState() {
    if (areAllActionsComplete()) {
        currentState = ActionState::COMPLETED;
    } else if (hasAnyFailedAction() && !hasAnyRunningAction()) {
        currentState = ActionState::FAILED;
    } else if (hasAnyRunningAction()) {
        currentState = ActionState::IN_PROGRESS;
    }
}

void CombinedAction::stopAllActions() {
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && actions[i]->isRunning()) {
            actions[i]->stop();
        }
    }
}

bool CombinedAction::hasAnyRunningAction() const {
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && actions[i]->isRunning()) {
            return true;
        }
    }
    return false;
}

bool CombinedAction::hasAnyFailedAction() const {
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && actions[i]->hasFailed()) {
            return true;
        }
    }
    return false;
}

bool CombinedAction::areAllActionsComplete() const {
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && !actions[i]->isComplete()) {
            return false;
        }
    }
    return true;
}