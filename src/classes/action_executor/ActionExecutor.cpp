#include "ActionExecutor.h"

ActionExecutor::ActionExecutor() 
    : bufferHead(0), bufferTail(0), bufferSize(0), totalActionsAdded(0), sequentialMode(true) {
    // Initialize all buffer slots as empty
    for (uint8_t i = 0; i < MAX_ACTIONS; i++) {
        actionBuffer[i].reset();
    }
}

bool ActionExecutor::executeAction(AbstractAction* action, bool stopOthers) {
    if (action == nullptr || !action->isValid()) {
        return false;
    }
    
    // Stop other actions if requested
    if (stopOthers) {
        stopAll();
    }
    
    // Add action to buffer and start immediately
    int8_t index = addAction(action, false);
    if (index >= 0) {
        action->start();
        actionBuffer[index].hasStarted = true;
        return true;
    }
    
    return false;
}

bool ActionExecutor::queueAction(AbstractAction* action) {
    if (action == nullptr || !action->isValid()) {
        return false;
    }
    
    return addAction(action, true) >= 0;
}

int8_t ActionExecutor::addAction(AbstractAction* action, bool queued) {
    if (isBufferFull()) {
        return INVALID_SLOT; // Buffer is full
    }
    
    // Add action to tail of circular buffer
    uint8_t index = bufferTail;
    actionBuffer[index].action = action;
    actionBuffer[index].isActive = true;
    actionBuffer[index].hasStarted = false;
    actionBuffer[index].isQueued = queued;
    
    // Update circular buffer pointers
    bufferTail = (bufferTail + 1) % MAX_ACTIONS;
    bufferSize++;
    totalActionsAdded++;
    
    return index;
}

bool ActionExecutor::isBufferFull() const {
    return bufferSize >= MAX_ACTIONS;
}

bool ActionExecutor::isBufferEmpty() const {
    return bufferSize == 0;
}

uint8_t ActionExecutor::getNextQueuedIndex() const {
    // Scan circular buffer from head to find first queued action
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && !actionBuffer[index].hasStarted && actionBuffer[index].isQueued) {
            return index;
        }
    }
    return MAX_ACTIONS; // Not found
}
void ActionExecutor::clear() {
    // Stop all running actions
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && actionBuffer[index].action != nullptr) {
            if (actionBuffer[index].action->isRunning()) {
                actionBuffer[index].action->stop();
            }
            actionBuffer[index].action->reset();
        }
        actionBuffer[index].reset();
    }
    
    // Reset circular buffer
    bufferHead = 0;
    bufferTail = 0;
    bufferSize = 0;
    totalActionsAdded = 0;
}

void ActionExecutor::update() {
    if (isBufferEmpty()) {
        return; // Nothing to do
    }
    
    // Update all running actions
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && actionBuffer[index].hasStarted && actionBuffer[index].action != nullptr) {
            if (actionBuffer[index].action->isRunning()) {
                actionBuffer[index].action->update();
            }
        }
    }
    
    // Start next queued action if in sequential mode
    if (sequentialMode && !hasRunningActions() && hasQueuedActions()) {
        startNext();
    }
    
    // Clean up completed actions
    cleanupCompletedActions();
}

void ActionExecutor::startNext() {
    uint8_t nextIndex = getNextQueuedIndex();
    if (nextIndex < MAX_ACTIONS) {
        actionBuffer[nextIndex].action->start();
        actionBuffer[nextIndex].hasStarted = true;
    }
}

void ActionExecutor::startAllQueued() {
    // Start all queued actions in the buffer
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && !actionBuffer[index].hasStarted && 
            actionBuffer[index].isQueued && actionBuffer[index].action != nullptr) {
            actionBuffer[index].action->start();
            actionBuffer[index].hasStarted = true;
        }
    }
}

void ActionExecutor::stopAll() {
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && actionBuffer[index].action != nullptr) {
            if (actionBuffer[index].action->isRunning()) {
                actionBuffer[index].action->stop();
            }
        }
    }
}

void ActionExecutor::setSequentialMode(bool sequential) {
    sequentialMode = sequential;
    
    // If switching to parallel mode, start all queued actions
    if (!sequential && hasQueuedActions()) {
        startAllQueued();
    }
}

bool ActionExecutor::isSequentialMode() const {
    return sequentialMode;
}

uint8_t ActionExecutor::getActionCount() const {
    return bufferSize;
}

uint8_t ActionExecutor::getRunningActionCount() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && actionBuffer[index].hasStarted && 
            actionBuffer[index].action != nullptr && actionBuffer[index].action->isRunning()) {
            count++;
        }
    }
    return count;
}

uint8_t ActionExecutor::getQueuedActionCount() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && !actionBuffer[index].hasStarted && actionBuffer[index].isQueued) {
            count++;
        }
    }
    return count;
}

bool ActionExecutor::hasQueuedActions() const {
    return getQueuedActionCount() > 0;
}

bool ActionExecutor::hasRunningActions() const {
    return getRunningActionCount() > 0;
}

bool ActionExecutor::hasCompletedActions() const {
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && actionBuffer[index].hasStarted && actionBuffer[index].action != nullptr) {
            if (actionBuffer[index].action->isDone()) {
                return true;
            }
        }
    }
    return false;
}

bool ActionExecutor::isEmpty() const {
    return isBufferEmpty();
}

uint8_t ActionExecutor::getTotalActionCount() const {
    return totalActionsAdded;
}

uint8_t ActionExecutor::getCompletedActionCount() const {
    uint8_t completed = totalActionsAdded - bufferSize;
    
    // Add currently completed but not yet cleaned up actions
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && actionBuffer[index].hasStarted && actionBuffer[index].action != nullptr) {
            if (actionBuffer[index].action->isDone()) {
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
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && actionBuffer[index].hasStarted && 
            actionBuffer[index].action != nullptr && actionBuffer[index].action->isRunning()) {
            return actionBuffer[index].action;
        }
    }
    return nullptr;
}

AbstractAction* ActionExecutor::getRunningAction(uint8_t targetIndex) const {
    uint8_t runningCount = 0;
    for (uint8_t i = 0; i < bufferSize; i++) {
        uint8_t index = (bufferHead + i) % MAX_ACTIONS;
        if (actionBuffer[index].isActive && actionBuffer[index].hasStarted && 
            actionBuffer[index].action != nullptr && actionBuffer[index].action->isRunning()) {
            if (runningCount == targetIndex) {
                return actionBuffer[index].action;
            }
            runningCount++;
        }
    }
    return nullptr;
}

void ActionExecutor::cleanupCompletedActions() {
    // Process completed actions from the head of the circular buffer
    while (bufferSize > 0) {
        uint8_t headIndex = bufferHead;
        ActionSlot& slot = actionBuffer[headIndex];
        
        // If the head action is done (completed or failed), remove it
        if (slot.isActive && slot.hasStarted && slot.action != nullptr && slot.action->isDone()) {
            // Reset the action for potential reuse
            slot.action->reset();
            
            // Remove from buffer by advancing head
            slot.reset();
            bufferHead = (bufferHead + 1) % MAX_ACTIONS;
            bufferSize--;
        } else {
            // Head action is not done yet, stop cleanup
            // (We only remove actions from the head to maintain order)
            break;
        }
    }
}

void ActionExecutor::printStatus() const {
    #ifdef DEBUG
    Serial.print(F("ActionExecutor Status: "));
    Serial.print(bufferSize);
    Serial.print(F(" active, "));
    Serial.print(getRunningActionCount());
    Serial.print(F(" running, "));
    Serial.print(getQueuedActionCount());
    Serial.print(F(" queued, "));
    Serial.print(getProgressPercent());
    Serial.println(F("% complete"));
    #endif
}