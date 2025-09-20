#include "ActionExecutor.h"

ActionExecutor::ActionExecutor() 
    : actionCount(0), totalActionsAdded(0), sequentialMode(true) {
    // Initialize all slots as empty
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        actions[i] = ActionSlot();
    }
}

bool ActionExecutor::addAction(AbstractAction* action) {
    if (action == nullptr || !action->isValid()) {
        return false;
    }
    
    uint8_t emptySlot = findEmptySlot();
    if (emptySlot >= MAX_ACTIONS) {
        return false; // No empty slots
    }
    
    // Add action to queue
    actions[emptySlot].action = action;
    actions[emptySlot].isActive = true;
    actions[emptySlot].hasStarted = false;
    
    actionCount++;
    totalActionsAdded++;
    
    return true;
}

void ActionExecutor::clear() {
    // Stop all running actions
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && actions[i].action != nullptr) {
            if (actions[i].action->isRunning()) {
                actions[i].action->stop();
            }
            actions[i].action->reset();
        }
        actions[i] = ActionSlot();
    }
    
    actionCount = 0;
    totalActionsAdded = 0;
}

void ActionExecutor::update() {
    if (actionCount == 0) {
        return; // Nothing to do
    }
    
    // Update all running actions
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && actions[i].hasStarted && actions[i].action != nullptr) {
            if (actions[i].action->isRunning()) {
                actions[i].action->update();
            }
        }
    }
    
    // Start next action if in sequential mode
    if (sequentialMode && !hasRunningActions() && hasQueuedActions()) {
        startNext();
    }
    
    // Clean up completed actions
    cleanupCompletedActions();
}

void ActionExecutor::startNext() {
    uint8_t nextIndex = findNextQueuedAction();
    if (nextIndex < MAX_ACTIONS) {
        actions[nextIndex].action->start();
        actions[nextIndex].hasStarted = true;
    }
}

void ActionExecutor::startAll() {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && !actions[i].hasStarted && actions[i].action != nullptr) {
            actions[i].action->start();
            actions[i].hasStarted = true;
        }
    }
}

void ActionExecutor::stopAll() {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && actions[i].action != nullptr) {
            if (actions[i].action->isRunning()) {
                actions[i].action->stop();
            }
        }
    }
}

void ActionExecutor::setSequentialMode(bool sequential) {
    sequentialMode = sequential;
    
    // If switching to parallel mode, start all queued actions
    if (!sequential && hasQueuedActions()) {
        startAll();
    }
}

bool ActionExecutor::isSequentialMode() const {
    return sequentialMode;
}

uint8_t ActionExecutor::getActionCount() const {
    return actionCount;
}

uint8_t ActionExecutor::getRunningActionCount() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && actions[i].hasStarted && 
            actions[i].action != nullptr && actions[i].action->isRunning()) {
            count++;
        }
    }
    return count;
}

bool ActionExecutor::hasQueuedActions() const {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && !actions[i].hasStarted) {
            return true;
        }
    }
    return false;
}

bool ActionExecutor::hasRunningActions() const {
    return getRunningActionCount() > 0;
}

bool ActionExecutor::hasCompletedActions() const {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && actions[i].hasStarted && actions[i].action != nullptr) {
            if (actions[i].action->isDone()) {
                return true;
            }
        }
    }
    return false;
}

bool ActionExecutor::isEmpty() const {
    return actionCount == 0;
}

uint8_t ActionExecutor::getTotalActionCount() const {
    return totalActionsAdded;
}

uint8_t ActionExecutor::getCompletedActionCount() const {
    uint8_t completed = totalActionsAdded - actionCount;
    
    // Add currently completed but not yet cleaned up actions
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && actions[i].hasStarted && actions[i].action != nullptr) {
            if (actions[i].action->isDone()) {
                completed++;
            }
        }
    }
    
    return completed;
}

uint8_t ActionExecutor::getProgressPercent() const {
    if (totalActionsAdded == 0) {
        return 0;
    }
    
    uint8_t completed = getCompletedActionCount();
    return (uint8_t)((completed * 100) / totalActionsAdded);
}

AbstractAction* ActionExecutor::getCurrentRunningAction() const {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && actions[i].hasStarted && 
            actions[i].action != nullptr && actions[i].action->isRunning()) {
            return actions[i].action;
        }
    }
    return nullptr;
}

void ActionExecutor::cleanupCompletedActions() {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && actions[i].hasStarted && actions[i].action != nullptr) {
            if (actions[i].action->isDone()) {
                // Reset the action for potential reuse
                actions[i].action->reset();
                
                // Remove from queue
                removeAction(i);
            }
        }
    }
}

uint8_t ActionExecutor::findNextQueuedAction() const {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (isSlotActive(i) && !actions[i].hasStarted) {
            return i;
        }
    }
    return MAX_ACTIONS; // Not found
}

uint8_t ActionExecutor::findEmptySlot() const {
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        if (!isSlotActive(i)) {
            return i;
        }
    }
    return MAX_ACTIONS; // Not found
}

bool ActionExecutor::isSlotActive(uint8_t index) const {
    return index < MAX_ACTIONS && actions[index].isActive;
}

void ActionExecutor::removeAction(uint8_t index) {
    if (index < MAX_ACTIONS && isSlotActive(index)) {
        actions[index] = ActionSlot();
        actionCount--;
    }
}

void ActionExecutor::printStatus() const {
    #ifdef DEBUG
    Serial.print(F("ActionExecutor Status: "));
    Serial.print(actionCount);
    Serial.print(F(" active, "));
    Serial.print(getRunningActionCount());
    Serial.print(F(" running, "));
    Serial.print(getProgressPercent());
    Serial.println(F("% complete"));
    #endif
}
