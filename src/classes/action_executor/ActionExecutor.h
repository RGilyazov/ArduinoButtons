#ifndef ACTION_EXECUTOR_H
#define ACTION_EXECUTOR_H

#include <Arduino.h>
#include "../actions/AbstractAction.h"

class ActionExecutor {
public:
    ActionExecutor();
    ~ActionExecutor() = default;
    
    // Action management
    bool addAction(AbstractAction* action);         // Add action to queue
    void clear();                                   // Clear all actions (stop and remove)
    uint8_t getActionCount() const;                 // Get number of queued actions
    uint8_t getRunningActionCount() const;          // Get number of currently running actions
    
    // Execution control
    void update();                                  // Update all actions (call in main loop)
    void startNext();                               // Start next queued action
    void startAll();                                // Start all queued actions simultaneously
    void stopAll();                                 // Stop all running actions
    
    // Execution modes
    void setSequentialMode(bool sequential);        // true = one at a time, false = all at once
    bool isSequentialMode() const;
    
    // State inquiry
    bool hasQueuedActions() const;                  // Any actions waiting to start?
    bool hasRunningActions() const;                 // Any actions currently running?
    bool hasCompletedActions() const;               // Any actions finished (ready for cleanup)?
    bool isEmpty() const;                           // No actions at all?
    
    // Progress tracking
    uint8_t getTotalActionCount() const;            // Total actions added (including completed)
    uint8_t getCompletedActionCount() const;        // How many actions have finished
    uint8_t getProgressPercent() const;             // Overall progress percentage
    
    // Debugging/monitoring
    AbstractAction* getCurrentRunningAction() const; // Get first running action
    void printStatus() const;                       // Debug output (if Serial available)

private:
    static constexpr uint8_t MAX_ACTIONS = 8;       // Maximum actions in queue
    
    struct ActionSlot {
        AbstractAction* action;
        bool isActive;                              // Is this slot in use?
        bool hasStarted;                            // Has this action been started?
        
        ActionSlot() : action(nullptr), isActive(false), hasStarted(false) {}
    };
    
    ActionSlot actions[MAX_ACTIONS];                // Action queue
    uint8_t actionCount;                            // Number of active actions
    uint8_t totalActionsAdded;                      // Total actions added (for progress tracking)
    bool sequentialMode;                            // Execute one at a time vs all at once
    
    // Private methods
    void cleanupCompletedActions();                 // Remove finished actions
    uint8_t findNextQueuedAction() const;           // Find next action to start
    uint8_t findEmptySlot() const;                  // Find empty slot for new action
    bool isSlotActive(uint8_t index) const;         // Check if slot is in use
    void removeAction(uint8_t index);               // Remove action from specific slot
};

#endif
