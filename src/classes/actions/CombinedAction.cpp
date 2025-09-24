#include "CombinedAction.h"

CombinedAction::CombinedAction() 
    : actionCount(0), currentState(ActionState::NOT_STARTED), stopOnFirstFailure(true),
      executionBehavior(ExecutionBehavior::IMMEDIATE_PARALLEL) {  // Default to parallel
    // Initialize action array to null pointers
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        actions[i] = nullptr;
    }
}

bool CombinedAction::addAction(AbstractAction* action) {
    if (action == nullptr || !action->isValid()) {
        return false;
    }
    
    if (actionCount >= MAX_ACTIONS) {
        return false; // No more space
    }
    
    actions[actionCount] = action;
    actionCount++;
    return true;
}

void CombinedAction::clearActions() {
    // Reset all actions to not started state
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
    
    // Start all actions simultaneously
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
    
    // Update all actions
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr && actions[i]->isActive()) {
            actions[i]->update();
        }
    }
    
    // Check for failure condition
    if (stopOnFirstFailure && hasAnyFailedAction()) {
        stopAllActions();
        currentState = ActionState::FAILED;
        return currentState;
    }
    
    // Update combined state based on individual action states
    updateState();
    return currentState;
}

void CombinedAction::reset() {
    // Reset all actions
    for (uint8_t i = 0; i < actionCount; i++) {
        if (actions[i] != nullptr) {
            actions[i]->reset();
        }
    }
    currentState = ActionState::NOT_STARTED;
}

void CombinedAction::stop() {
    stopAllActions();
    currentState = ActionState::FAILED;
}

bool CombinedAction::isRunning() const {
    return currentState == ActionState::IN_PROGRESS;
}

bool CombinedAction::isComplete() const {
    return currentState == ActionState::COMPLETED;
}

bool CombinedAction::hasFailed() const {
    return currentState == ActionState::FAILED;
}

ActionState CombinedAction::getState() const {
    return currentState;
}

bool CombinedAction::isValid() const {
    if (actionCount == 0) {
        return false;
    }
    
    // Check that all actions are valid
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
        // All actions are done, but some failed
        currentState = ActionState::FAILED;
    } else if (hasAnyRunningAction()) {
        currentState = ActionState::IN_PROGRESS;
    }
    // If no actions are running and none completed/failed, stay in current state
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
    return true; // All actions are complete (or no actions)
}